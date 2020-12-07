#pragma once

#include "SocketVal.h"
#include "ISocket.h"
#include <memory>

class Socket;

class ServerSocket : public ISocket
{
public:
	ServerSocket();
	ServerSocket(size_t bufferSize);
	ServerSocket(uint32_t port);
	ServerSocket(size_t bufferSize, uint32_t port);
	ServerSocket(const char* IP);
	ServerSocket(const char* IP, size_t bufferSize);
	ServerSocket(const char* IP, uint32_t port);
	ServerSocket(const char* IP, size_t bufferSize, uint32_t port);
	~ServerSocket();

	Socket* getNewClientPtr();
	Socket& getNewClientRef();

	const uint32_t& getPort() const override; // default 54000
	const size_t& getBufferSize() const override; // default 1024b <=> 1kB
	const char* getIP_str() const override; // default "127.0.0.1"
private:
	class ServerSocketImpl;
	std::unique_ptr<ServerSocketImpl> m_ServerSocketPimpl;
	const size_t m_BufferSize = 1024; // 1 kB
};
