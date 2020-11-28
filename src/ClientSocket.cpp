#include "ClientSocket.h"

#define DEFAULT_IP_ADDR "127.0.0.1"
#define DEFAULT_PORT_VAL 54000u

#define IP_ADDR_DELIMITER 4
#define MAX_IP_NUMBER_LEN 3

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)

#endif

#include <iostream>

class ClientSocket::ClientSocketImpl
{
private:
	const char* m_IP = DEFAULT_IP_ADDR;
	uint32_t m_Port = DEFAULT_PORT_VAL;
	_SocketVal* m_SocketVal = NULL;

	void initSocket();
	uint32_t getIPlval() const;

public:
	ClientSocketImpl(const char* IP, uint32_t port, _SocketVal* clientSocket);
	~ClientSocketImpl();

	const _SocketVal* getSocketVal() const;
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
		std::cerr << "SocketVal ptr is NULL!\n";
		std::exit(-1);
	}
#if defined(_MSC_VER)
	{
		// Initialize winsock
		WSADATA wsaData;
		WORD ver = MAKEWORD(2, 2); // 0x0202 // 514

		int wsOk = WSAStartup(ver, &wsaData);
		if (wsOk != 0)
		{
			std::cerr << "Can't Initialize winsock! Quitting!\n";
			std::exit(-1);
		}
	}
#endif
	// Create a SOCKET for connecting to server
	*m_SocketVal = socket(AF_INET, SOCK_STREAM, 0);
	if (*m_SocketVal == INVALID_SOCKET) {
#if defined(_MSC_VER)
		std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
#endif
		std::exit(-1);
	}
	
	// Bind the socket to an IP address and port
	static sockaddr_in hint;
	memset(&hint, 0, sizeof(hint));
	hint.sin_family = AF_INET;
	hint.sin_port = htons(m_Port); //enter desired port
	hint.sin_addr.s_addr = getIPlval();

	connect(*m_SocketVal, (sockaddr*)&hint, sizeof(hint));
}

ClientSocket::ClientSocketImpl::ClientSocketImpl(const char* IP, uint32_t port, _SocketVal* clientSocket) :
	m_SocketVal{ clientSocket }, 
	m_IP{ IP != (const char*)NULL ? IP : m_IP },
	m_Port { port != 0 ? port : m_Port }
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

const _SocketVal* ClientSocket::ClientSocketImpl::getSocketVal() const
{
	return m_SocketVal;
}

ClientSocket::ClientSocket() :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>((const char*)NULL, 0, &m_ClientSocket) }
{

}

ClientSocket::ClientSocket(const char* IP) :
	Socket::Socket(0, NULL),
	m_ClientSocketPimpl{ std::make_unique<ClientSocketImpl>(IP, 0, &m_ClientSocket) }
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

size_t ClientSocket::getBufferSize() const
{
	return m_BufferSize;
}

const char* ClientSocket::getIP() const
{
	return m_ClientSocketPimpl->getIP();
}

uint32_t ClientSocket::getPort() const
{
	return m_ClientSocketPimpl->getPort();
}