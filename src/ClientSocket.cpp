#include "ClientSocket.h"

#define DEFAULT_IP_ADDR "127.0.0.1"
#define DEFAULT_PORT_VAL 54000u

#define IP_ADDR_DELIMITER 4
#define MAX_IP_NUMBER_LEN 3

#include <memory.h>
#include <iostream>
#include "SocketException.h"

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#endif

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

class ClientSocket::ClientSocketImpl
{
private:
	const char* m_IP;
	uint32_t m_Port;
	_SocketVal* m_SocketVal = NULL;

	void initSocket();
	uint32_t getIPlval() const;

public:
	explicit ClientSocketImpl() = delete;
	explicit ClientSocketImpl(const char* IP, uint32_t port, const _SocketVal* clientSocket);
	~ClientSocketImpl();

	const char* getIP() const;
	const uint32_t& getPort() const;
};

uint32_t ClientSocket::ClientSocketImpl::getIPlval() const
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

void ClientSocket::ClientSocketImpl::initSocket()
{
	if (NULL == m_SocketVal)
	{
		return;
	}
#if defined(_MSC_VER)
	{
		// Initialize winsock
		WSADATA wsaData;
		WORD ver = MAKEWORD(2, 2); // 0x0202 // 514

		int wsOk = WSAStartup(ver, &wsaData);
		if (wsOk != 0)
		{
			const wchar_t* wstrError = GetWinWStringError();
			throw SocketException(L"WSAStartup() :: Exception on client socket %llu : port %u. Error: %s.", *m_SocketVal, m_Port, wstrError);
			if (wstrError != (const wchar_t*)NULL)
			{
				free((void*)wstrError);
			}
			WSACleanup();
		}
	}
#endif
	// Create a SOCKET for connecting to server
	*m_SocketVal = socket(AF_INET, SOCK_STREAM, 0);
	if (*m_SocketVal == INVALID_SOCKET) {
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"socket() :: Exception on client socket %llu : port %u. Error: %s.", *m_SocketVal, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"socket() :: Exception on client socket %lld : port %u. Error: %s.", *m_SocketVal, m_Port, strerror(errno));
#endif
	}
	
	// Bind the socket to an IP address and port
	static sockaddr_in hint;
	memset(&hint, 0, sizeof(hint));
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_Port); //enter desired port
	hint.sin_addr.s_addr = getIPlval();

	if (connect(*m_SocketVal, (sockaddr*)&hint, sizeof(hint)) < 0)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"connect() :: Exception on client socket %llu : port %u. Error: %s.", *m_SocketVal, m_Port, wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
		WSACleanup();
#elif defined(__unix)
		throw SocketException(L"connect() :: Exception on client socket %lld : port %u. Error: %s.", *m_SocketVal, m_Port, strerror(errno));
#endif
	}
}

ClientSocket::ClientSocketImpl::ClientSocketImpl(const char* IP, uint32_t port, const _SocketVal* clientSocket) :
	m_SocketVal{ (NULL == clientSocket) ? NULL : reinterpret_cast<_SocketVal*>(reinterpret_cast<size_t>(clientSocket)) },
	m_IP{ IP != (const char*)NULL ? IP : DEFAULT_IP_ADDR },
	m_Port { port != 0 ? port : DEFAULT_PORT_VAL }
{
	initSocket();
}

ClientSocket::ClientSocketImpl::~ClientSocketImpl()
{
#if defined(_MSC_VER)
	// Cleanup winsock
	WSACleanup();
#endif
}

const char* ClientSocket::ClientSocketImpl::getIP() const
{
	return m_IP;
}

const uint32_t& ClientSocket::ClientSocketImpl::getPort() const
{
	return m_Port;
}

ClientSocket::ClientSocket() :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>((const char*)NULL, 0, &m_ClientSocket) }
{

}

ClientSocket::ClientSocket(uint32_t port) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl { std::make_unique<ClientSocketImpl>((const char*)NULL, port, &m_ClientSocket) }
{

}

ClientSocket::ClientSocket(size_t bufferSize) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl { std::make_unique<ClientSocketImpl>((const char*)NULL, 0, &m_ClientSocket) },
	m_BufferSize{ bufferSize }
{

}

ClientSocket::ClientSocket(size_t bufferSize, uint32_t port) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>((const char*)NULL, port, &m_ClientSocket) },
	m_BufferSize{ bufferSize }
{

}

ClientSocket::ClientSocket(const char* IP) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>(IP, 0, &m_ClientSocket) }
{

}

ClientSocket::ClientSocket(const char* IP, size_t bufferSize) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>(IP, 0, &m_ClientSocket) },
	m_BufferSize{ bufferSize }
{

}

ClientSocket::ClientSocket(const char* IP, uint32_t port) :
	Socket::Socket(0, NULL), 
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>(IP, port, &m_ClientSocket) }
{

}

ClientSocket::ClientSocket(const char* IP, size_t bufferSize, uint32_t port) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>(IP, port, &m_ClientSocket) },
	m_BufferSize{ bufferSize }
{

}

ClientSocket::~ClientSocket()
{

}

const size_t& ClientSocket::getBufferSize() const
{
	return m_BufferSize;
}

const char* ClientSocket::getIP_str() const
{
	return m_ClientSocketPimpl->getIP();
}

const uint32_t& ClientSocket::getPort() const
{
	return m_ClientSocketPimpl->getPort();
}
