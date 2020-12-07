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

	const size_t& getBufferSize() const override; // default 1024b <=> 1kB
	const char* getIP_str() const override; // default "127.0.0.1"
	const uint32_t& getPort() const override; // default 54000
private:
	class ClientSocketImpl;
	std::unique_ptr<ClientSocketImpl> m_ClientSocketPimpl;
	size_t m_BufferSize = 1024; // 1 kB
};
