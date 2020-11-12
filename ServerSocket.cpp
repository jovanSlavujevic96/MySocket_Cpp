#include "ServerSocket.h"

#include <iostream>
#include <memory.h>
#include "ClientSocket.h"

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)

#endif

#define IP_ADDR_DELIMITER 4
#define MAX_IP_NUMBER_LEN 3

#define DEFAULT_IP_ADDR "127.0.0.1"
#define DEFAULT_PORT_VAL 54000u

class ServerSocket::ServerSocketImpl
{
private:
	_SocketVal m_ListeningSocket;
	const char* m_IP = DEFAULT_IP_ADDR;
	uint32_t m_Port = DEFAULT_PORT_VAL;

	void initSocket();
	uint32_t getIPlval() const;
public:
	ServerSocketImpl();
	ServerSocketImpl(const char* IP);
	~ServerSocketImpl();
	_SocketVal getNewClient();
	const char* getIP() const;
	uint32_t getPort() const;
};

uint32_t ServerSocket::ServerSocketImpl::getIPlval() const
{
	static uint8_t values[IP_ADDR_DELIMITER];
	static char currentNumber[MAX_IP_NUMBER_LEN + 1];
	memset(values, 0, IP_ADDR_DELIMITER);
	currentNumber[MAX_IP_NUMBER_LEN] = '\0';
	for (uint8_t i = 0; i < IP_ADDR_DELIMITER; ++i)
	{
		static uint8_t beginIdx, endIdx = 0;
		for (beginIdx = endIdx = endIdx; m_IP[endIdx] != '.' && endIdx < strlen(m_IP); endIdx++);
		memset(currentNumber, 0, MAX_IP_NUMBER_LEN);
		memcpy(currentNumber, m_IP + beginIdx, endIdx - beginIdx);
		values[i] = atoi(currentNumber);
		endIdx++;
	}
	return  *(uint32_t*)(values);
}

void ServerSocket::ServerSocketImpl::initSocket()
{
#if defined(_MSC_VER)
	{
		// Initialize winsock
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2); // 0x0202 // 514

		int wsOk = WSAStartup(ver, &wsData);
		if (wsOk != 0)
		{
			std::cerr << "Can't Initialize winsock! Quitting!\n";
			std::exit(-1);
		}
	}
#endif
	// Create a socket
	m_ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListeningSocket == INVALID_SOCKET)
	{
		std::cerr << "Can't create a socket! Quitting!\n";
		std::exit(-1);
	}

	// Bind the socket to an IP address and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_Port); //enter desired port
#if defined(_MSC_VER)
	hint.sin_addr.S_un.S_addr = getIPlval(); // INADDR_ANY; // Could also use inet_pton
#elif defined(__unix)
	address.sin_addr.s_addr = m_IP;
#endif

	bind(m_ListeningSocket, (sockaddr*)&hint, sizeof(hint));

	// Tell Winsock the socket is for listening
	listen(m_ListeningSocket, SOMAXCONN);
}

ServerSocket::ServerSocketImpl::ServerSocketImpl()
{
	initSocket();
}

ServerSocket::ServerSocketImpl::ServerSocketImpl(const char* IP) :
	m_IP{ IP }
{
	initSocket();
}

ServerSocket::ServerSocketImpl::~ServerSocketImpl()
{
#if defined(_MSC_VER)
	// Cleanup winsock
	WSACleanup();
	closesocket(m_ListeningSocket);
#elif defined(__unix)
	close(m_ListeningSocket);
#endif
}

_SocketVal ServerSocket::ServerSocketImpl::getNewClient()
{
	_SocketVal clientSocket;

	// Wait for a connection
	static sockaddr_in client;
	static int clientSize = sizeof(client);

	clientSocket = accept(m_ListeningSocket, (sockaddr*)&client, &clientSize);

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

	return clientSocket;
}

const char* ServerSocket::ServerSocketImpl::getIP() const
{
	return m_IP;
}

uint32_t ServerSocket::ServerSocketImpl::getPort() const
{
	return m_Port;
}

ServerSocket::ServerSocket() :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>() }
{

}

ServerSocket::ServerSocket(size_t bufferSize) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>() },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::ServerSocket(const char* IP) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP) }
{

}

ServerSocket::ServerSocket(const char* IP, size_t bufferSize) :
	m_ServerSocketPimpl{ std::make_unique<ServerSocketImpl>(IP) },
	m_BufferSize{ bufferSize }
{

}

ServerSocket::~ServerSocket()
{

}

ClientSocket ServerSocket::getNewClient()
{
	return ClientSocket(m_ServerSocketPimpl->getNewClient(), this);
}

size_t ServerSocket::getBufferSize() const
{
	return m_BufferSize;
}

const char* ServerSocket::getIP() const
{
	return m_ServerSocketPimpl->getIP();
}

uint32_t ServerSocket::getPort() const
{
	return m_ServerSocketPimpl->getPort();
}
