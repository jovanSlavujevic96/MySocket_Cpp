#pragma once

#include <string>
#include "SocketVal.h"

class ServerSocket;

class Socket
{
public:
	Socket(_SocketVal val, ServerSocket* server);
	virtual ~Socket();

	Socket& operator<<(const char* data); // send msg via socket
	Socket& operator>>(char* data); // get msg from socket
protected:
	_SocketVal m_ClientSocket;
	ServerSocket* m_ServerSocket = nullptr;
};

