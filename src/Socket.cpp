 #include <iostream>

#include "Socket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "string.h"

#if defined(_MSC_VER)
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
#elif defined(__unix)
#include <sys/socket.h> 
#include <unistd.h> 
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
		std::cerr << "Error in send(). Quitting\n";
		std::exit(-1); // TO DO
	}
}

void Socket::receiveData(char* data, int bufferSize, const _SocketVal& socket)
{
	static int bytesReceived;
	if ((bytesReceived = recv(socket, data, bufferSize, 0)) == SOCKET_ERROR)
	{
		std::cerr << "Error in recv(). Quitting\n";
		std::exit(-1); // TO DO
	}
	else if (bytesReceived == 0)
	{
		std::cout << "Client disconnected\n";
		std::exit(-1); // TO DO
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

	receiveData(dataPtr, data.size(), m_ClientSocket);

	return *this;
}