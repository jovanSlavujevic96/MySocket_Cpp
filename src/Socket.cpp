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

void Socket::sendData(const char* data, const _SocketVal& socket)
{
	if (send(socket, data, (int)strlen(data), 0) < 0)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"Exception on socket %lu. sendData() failed. Error: %s.", socket, wstrError);
		if (wstrError == (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
#elif defined(__unix)
		throw SocketException(L"Exception on socket %ld. sendData() failed. Error: %s.", socket, strerror(errno));
#endif
	}
}

void Socket::receiveData(char* data, int bufferSize, const _SocketVal& socket)
{
	static int bytesReceived;
	if ((bytesReceived = recv(socket, data, bufferSize, 0)) <= SOCKET_ERROR)
	{
#if defined(_MSC_VER)
		const wchar_t* wstrError = GetWinWStringError();
		throw SocketException(L"Exception on socket %lu. receiveData() failed. Error: %s.", socket, wstrError);
		if (wstrError == (const wchar_t*)NULL)
		{
			free((void*)wstrError);
		}
#elif defined(__unix)
		throw SocketException(L"Exception on socket %ld. receiveData() failed. Error: %s.", socket, strerror(errno));
#endif
	}
	else if (bytesReceived == 0)
	{
#if defined(_MSC_VER)
		throw SocketException(L"Exception on socket %lu. receiveData() failed. The other side closed the connection.", socket);
#elif defined(__unix)
		throw SocketException(L"Exception on socket %ld. receiveData() failed. The other side closed the connection.", socket);
#endif
	}
	else if (bytesReceived < bufferSize && bytesReceived > 0)
	{
		data[bytesReceived] = '\0';
	}
}

Socket& Socket::operator<<(const char* data)
{
	sendData(data, m_ClientSocket);
	return *this;
}

Socket& Socket::operator<<(const std::string& data)
{
	sendData(data.c_str(), m_ClientSocket);
	return *this;
}

Socket& Socket::operator>>(char* data)
{
	if (NULL == m_ServerSocketParent)
	{
		receiveData(data, (int)reinterpret_cast<ClientSocket*>(this)->getBufferSize(), m_ClientSocket);
	}
	else
	{
		receiveData(data, (int)m_ServerSocketParent->getBufferSize(), m_ClientSocket);
	}
	return *this;
}

Socket& Socket::operator>>(std::string& data)
{
	static size_t dataAddr;
	static char* dataPtr;

	dataAddr = reinterpret_cast<size_t>(data.c_str());
	dataPtr = reinterpret_cast<char*>(dataAddr);

	//memset(dataPtr, 0, data.size());

	receiveData(dataPtr, (int)data.size(), m_ClientSocket);

	return *this;
}