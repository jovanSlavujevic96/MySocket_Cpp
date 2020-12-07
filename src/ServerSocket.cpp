#include "ServerSocket.h"

#include <iostream>
#include <memory.h>
#include <vector>
#include "Socket.h"
#include "SocketException.h"

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <netdb.h> 
#endif

#define IP_ADDR_DELIMITER 4
#define MAX_IP_NUMBER_LEN 3

#define DEFAULT_IP_ADDR "127.0.0.1"
#define DEFAULT_PORT_VAL 54000u

#if defined(_MSC_VER)
static inline const wchar_t* GetWinWStringError(void)
{
	wchar_t* wstrError = NULL;
	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&wstrError, 0, NULL);
	return wstrError;
}
#endif

class ServerSocket::ServerSocketImpl
{
private:
	ServerSocket* m_ServerSocketParent = NULL;
	_SocketVal m_ListeningSocket;
	const char* m_IP;
	uint32_t m_Port;
	std::vector<Socket*> m_ActiveSockets;

	void initSocket();
	uint32_t getIPlval() const;
public:
	explicit ServerSocketImpl() = delete;
	explicit ServerSocketImpl(const char* IP, uint32_t port, ServerSocket* serverSocket);
	~ServerSocketImpl();

	Socket* getNewClient();
	const char* getIP() const;
	const uint32_t& getPort() const;
};
 
uint32_t ServerSocket::ServerSocketImpl::getIPlval() const
{
	static uint8_t values[IP_ADDR_DELIMITER];
	static char currentNumber[MAX_IP_NUMBER_LEN + 1];
	memset(values, 0, IP_ADDR_DELIMITER);
	currentNumber[MAX_IP_NUMBER_LEN] = '\0';
	static uint8_t beginIdx, endIdx;
	endIdx = 0;
	for (uint8_t i = 0; i < IP_ADDR_DELIMITER; ++i)
	{
		for (beginIdx = endIdx = endIdx; m_IP[endIdx] != '.' && endIdx < strlen(m_IP); endIdx++);
		memset(currentNumber, 0, MAX_IP_NUMBER_LEN);
		memcpy(currentNumber, m_IP + beginIdx, endIdx - beginIdx);
		values[i] = atoi(currentNumber);
		endIdx++;
	}
	return *(uint32_t*)(values);
}

void ServerSocket::ServerSocketImpl::initSocket()
{
#if defined(_MSC_VER)
	{
		// Initialize winsock
		WSADATA wsaData;
		WORD ver = MAKEWORD(2, 2); // 0x0202 // 514

		int wsOk = WSAStartup(ver, &wsaData);
		if (wsOk != 0)
		{
			const wchar_t* wstrError = GetWinWStringError();
			throw SocketException(L"WSAStartup() :: Exception on server socket %llu : port %u. Error: %s.", m_ListeningSocket, m_Port, wstrError);
			if (wstrError != (const wchar_t*)NULL)
			{
				free((void*)wstrError);
			}
			WSACleanup();
		}
	}
#endif
	// Create a socket
	m_ListeningSocket = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP
	if (m_ListeningSocket == INVALID_SOCKET)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"socket() :: Exception on server socket %llu : port %u. Error: %s.", m_ListeningSocket, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"socket() :: Exception on server socket %lld : port %u. Error: %s.", m_ListeningSocket, m_Port, strerror(errno));
#endif
	}

	// Bind the socket to an IP address and port
	static sockaddr_in hint;
	memset(&hint, 0, sizeof(hint));
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_Port); //enter desired port
	hint.sin_addr.s_addr = getIPlval();

	if (bind(m_ListeningSocket, (sockaddr*)&hint, sizeof(hint)) < 0)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"bind() :: Exception on server socket %llu : port %u. Error: %s.", m_ListeningSocket, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"bind() :: Exception on server socket %lld : port %u. Error: %s.", m_ListeningSocket, m_Port, strerror(errno));
#endif
	}

	// Tell Winsock the socket is for listening
	if (listen(m_ListeningSocket, SOMAXCONN) < 0)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"listen() :: Exception on server socket %llu : port %u. Error: %s.", m_ListeningSocket, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"listen() :: Exception on server socket %lld : port %u. Error: %s.", m_ListeningSocket, m_Port, strerror(errno));
#endif
	}
}

ServerSocket::ServerSocketImpl::ServerSocketImpl(const char* IP, uint32_t port, ServerSocket* serverSocket) :
	m_IP{ IP != NULL ? IP : DEFAULT_IP_ADDR },
	m_Port{ port != 0 ? port : DEFAULT_PORT_VAL },
	m_ServerSocketParent{ serverSocket }
{
	initSocket();
}

ServerSocket::ServerSocketImpl::~ServerSocketImpl()
{
	for (auto socket : m_ActiveSockets)
	{
		delete socket;
	}
#if defined(_MSC_VER)
	// Cleanup winsock
	WSACleanup();
	closesocket(m_ListeningSocket);
#elif defined(__unix)
	close(m_ListeningSocket);
#endif
}

Socket* ServerSocket::ServerSocketImpl::getNewClient()
{
	static _SocketVal clientSocket;

	// Wait for a connection
	static sockaddr_in client;
	static int clientSize = sizeof(client);

	memset(&client, 0u, clientSize);

	clientSocket = accept(m_ListeningSocket, (sockaddr*)&client, (socklen_t*)&clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"accept() :: Exception on server socket %llu : port %u. Error: %s.", m_ListeningSocket, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"accept() :: Exception on server socket %lld : port %u. Error: %s.", m_ListeningSocket, m_Port, strerror(errno));
#endif
	}

	static char host[NI_MAXHOST];		// Client's remote name
	static char service[NI_MAXHOST];	// Service (i.e. port) the client is connected on

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << host << " connected on port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connect on port " <<
			ntohs(client.sin_port) << std::endl;
	}

	m_ActiveSockets.push_back(new Socket(clientSocket, m_ServerSocketParent));
	return m_ActiveSockets[m_ActiveSockets.size()-1];
}

const char* ServerSocket::ServerSocketImpl::getIP() const
{
	return m_IP;
}

const uint32_t& ServerSocket::ServerSocketImpl::getPort() const
{
	return m_Port;
}

ServerSocket::ServerSocket() :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>((const char*)NULL, 0, this) }
{

}

ServerSocket::ServerSocket(size_t bufferSize) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>((const char*)NULL, 0, this) },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::ServerSocket(uint32_t port) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>((const char*)NULL, port, this) }
{

}

ServerSocket::ServerSocket(size_t bufferSize, uint32_t port) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>((const char*)NULL, port, this) },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::ServerSocket(const char* IP) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP, 0, this) }
{

}

ServerSocket::ServerSocket(const char* IP, size_t bufferSize) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP, 0, this) },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::ServerSocket(const char* IP, uint32_t port) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP, port, this) }
{

}

ServerSocket::ServerSocket(const char* IP, size_t bufferSize, uint32_t port) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP, port, this) },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::~ServerSocket() = default;

Socket* ServerSocket::getNewClientPtr()
{
	return m_ServerSocketPimpl->getNewClient(); 
}

Socket& ServerSocket::getNewClientRef()
{
	return *m_ServerSocketPimpl->getNewClient();
}

const uint32_t& ServerSocket::getPort() const
{
	return m_ServerSocketPimpl->getPort();
}

const size_t& ServerSocket::getBufferSize() const
{
	return m_BufferSize;
}

const char* ServerSocket::getIP_str() const
{
	return m_ServerSocketPimpl->getIP();
}

