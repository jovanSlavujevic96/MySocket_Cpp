#pragma once

#include "Socket.h"

class ClientSocket : public Socket
{
public:
	ClientSocket(_SocketVal val);
	~ClientSocket();
};