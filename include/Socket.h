#pragma once

#include "SocketVal.h"
#include "ISocket.h"
#include <string>

class ServerSocket;

class Socket : public ISocket
{
public:
	Socket() = delete;
	Socket(const Socket& socket) = delete;
	Socket(_SocketVal val, ServerSocket* server);
	~Socket();

	const Socket& operator<<(const char* data) const;// send msg via socket
	const Socket& operator<<(const std::string& data) const; 
	const Socket& operator>>(char* data) const; // get msg from socket
	const Socket& operator>>(std::string& data) const;

	const uint32_t& getPort() const override;
	const size_t& getBufferSize() const override;
	const char* getIP_str() const override;
private:
	ServerSocket* m_ServerSocketParent = nullptr;
	
	inline void sendData(const char* data) const;
	inline void receiveData(char* data) const;
protected:
	const _SocketVal m_ClientSocket;
};
