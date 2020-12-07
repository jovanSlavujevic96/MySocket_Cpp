 #include <iostream>

#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "SocketException.h"
#include "string.h"

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)
#include <sys/socket.h> 
#include <unistd.h> 
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

void Socket::sendData(const char* data) const
{
	if (send(m_ClientSocket, data, (int)strlen(data), 0) < 0)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"send() :: Exception on socket %llu : port %u. Error: %s.", socket, this->getPort(), wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
#elif defined(__unix)
		throw SocketException(L"send() :: Exception on socket %lld : port %u. Error: %s.", socket, this->getPort(), strerror(errno));
#endif
	}
}

void Socket::receiveData(char* data) const
{
	static int bytesReceived;
	if ((bytesReceived = recv(m_ClientSocket, data, (const int&)this->getBufferSize(), 0)) <= SOCKET_ERROR)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"recv() :: Exception on socket %llu : port %u. Error: %s.", socket, this->getPort(), wstrError);
		if (wstrError != (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
#elif defined(__unix)
		throw SocketException(L"recv() :: Exception on socket %lld : port %u. Error: %s.", socket, this->getPort(), strerror(errno));
#endif
	}
	else if (bytesReceived == 0)
	{
#if defined(_MSC_VER)
		throw SocketException(L"recv() :: Exception on socket %llu : port %u. The other side closed the connection.", socket, this->getPort());
#elif defined(__unix)
		throw SocketException(L"recv() :: Exception on socket %lld : port %u. The other side closed the connection.", socket, this->getPort());
#endif
	}
	else if (bytesReceived < this->getBufferSize() && bytesReceived > 0)
	{
		data[bytesReceived] = '\0';
	}
}

Socket::Socket(_SocketVal val, ServerSocket* server) :
	m_ClientSocket{ val },
	m_ServerSocketParent{ server }
{

}

Socket::~Socket()
{
#if defined(_MSC_VER)
	closesocket(m_ClientSocket);
#elif defined(__unix)
	close(m_ClientSocket);
#endif
}

const Socket& Socket::operator<<(const char* data) const
{
	sendData(data);
	return *this;
}

const Socket& Socket::operator<<(const std::string& data) const
{
	sendData(data.c_str());
	return *this;
}

const Socket& Socket::operator>>(char* data) const
{
	receiveData(data);
	return *this;
}

const Socket& Socket::operator>>(std::string& data) const
{
	static size_t dataAddr;
	static char* dataPtr;

	dataAddr = reinterpret_cast<size_t>(data.c_str());
	dataPtr = reinterpret_cast<char*>(dataAddr);

	//memset(dataPtr, 0, data.size());

	receiveData(dataPtr);

	return *this;
}

const uint32_t& Socket::getPort() const
{
	return (NULL == m_ServerSocketParent) ? reinterpret_cast<ClientSocket*>(reinterpret_cast<size_t>(this))->getPort() : m_ServerSocketParent->getPort();
}

const size_t& Socket::getBufferSize() const
{
	return (NULL == m_ServerSocketParent) ? reinterpret_cast<ClientSocket*>(reinterpret_cast<size_t>(this))->getBufferSize() : m_ServerSocketParent->getBufferSize();
}

const char* Socket::getIP_str() const
{
	return (NULL == m_ServerSocketParent) ? reinterpret_cast<ClientSocket*>(reinterpret_cast<size_t>(this))->getIP_str() : m_ServerSocketParent->getIP_str();
}
