#pragma once

#include "SocketVal.h"
#include <string>

class ServerSocket;

class Socket
{
public:
	Socket() = delete;
	Socket(const Socket& socket) = delete;
	Socket(_SocketVal val, ServerSocket* server);
	virtual ~Socket();

	static inline void sendData(const char* data, const _SocketVal& socket);
	static inline void receiveData(char* data, int bufferSize, const _SocketVal& socket);

	Socket& operator<<(const char* data); // send msg via socket
	Socket& operator<<(const std::string& data);
	Socket& operator>>(char* data); // get msg from socket
	Socket& operator>>(std::string& data);
protected:
	_SocketVal m_ClientSocket;
	ServerSocket* m_ServerSocketParent = nullptr;
};
