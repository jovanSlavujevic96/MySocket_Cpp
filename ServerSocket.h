#pragma once

#include "SocketVal.h"
#include <memory>

class ClientSocket;

class ServerSocket
{
public:
	ServerSocket();
	ServerSocket(size_t bufferSize);
	ServerSocket(const char* IP);
	ServerSocket(const char* IP, size_t bufferSize);
	~ServerSocket();

	ClientSocket getNewClient();
	size_t getBufferSize() const;
	const char* getIP() const; // default "127.0.0.1"
	uint32_t getPort() const; // default 54000

private:
	class ServerSocketImpl;
	std::unique_ptr<ServerSocketImpl> m_ServerSocketPimpl;
	size_t m_BufferSize = 1024; // 1 kB
};