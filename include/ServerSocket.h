#pragma once

#include "SocketVal.h"
#include <memory>

class Socket;

class ServerSocket
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
	size_t getBufferSize() const; // default 1024b <=> 1kB
	const char* getIP() const; // default "127.0.0.1"
	uint32_t getPort() const; // default 54000

private:
	class ServerSocketImpl;
	std::unique_ptr<ServerSocketImpl> m_ServerSocketPimpl;
	size_t m_BufferSize = 1024; // 1 kB
};
