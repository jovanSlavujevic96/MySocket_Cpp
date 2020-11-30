#pragma once

#include "Socket.h"
#include <memory>

class ClientSocket : public Socket
{
public:
	ClientSocket();
	ClientSocket(size_t bufferSize);
	ClientSocket(uint32_t port);
	ClientSocket(size_t bufferSize, uint32_t port);
	ClientSocket(const char* IP);
	ClientSocket(const char* IP, uint32_t port);
	ClientSocket(const char* IP, size_t bufferSize);
	ClientSocket(const char* IP, size_t bufferSize, uint32_t port);
	~ClientSocket();

	size_t getBufferSize() const; // default 1024b <=> 1kB
	const char* getIP() const; // default "127.0.0.1"
	uint32_t getPort() const; // default 54000

private:
	class ClientSocketImpl;
	std::unique_ptr<ClientSocketImpl> m_ClientSocketPimpl;
	size_t m_BufferSize = 1024; // 1 kB
};
