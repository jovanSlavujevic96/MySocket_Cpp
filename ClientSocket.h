#pragma once

#include <memory>
#include <string>
#include "SocketVal.h"
#include "ServerSocket.h"

class ClientSocket
{
public:
	ClientSocket(_SocketVal val, ServerSocket* server);
	~ClientSocket();

	ClientSocket& operator<<(const char* data); // send msg via socket
	ClientSocket& operator>>(char* data); // get msg from socket
private:
	_SocketVal m_ClientSocket;
	ServerSocket* m_ServerSocket = nullptr;
};

