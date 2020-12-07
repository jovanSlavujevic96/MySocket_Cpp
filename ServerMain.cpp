#include <iostream>

#define BUFFER_SIZE 4096u // 4 Mb

// https://www.geeksforgeeks.org/nat-hole-punching-in-computer-network/
// https://stackoverflow.com/questions/55885656/is-it-possible-to-address-a-server-client-socket-in-an-another-network-c

#include "ServerSocket.h"
#include "Socket.h"
#include "SocketException.h"
#include <memory.h>
#include <string>

int main()
{
	ServerSocket socket((size_t)BUFFER_SIZE);
	Socket& clientSocket = socket.getNewClientRef();
	// While loop: accept and echo message back to client
	// char buf[BUFFER_SIZE];
	std::string buf;
	buf.resize(BUFFER_SIZE);
	while (true)
	{
		// Wait for client to send data
		try
		{
			clientSocket >> buf;
		}
		catch (const SocketException& exception)
		{
			//std::cout << exception.what() << std::endl;
			std::wcout << exception.wwhat() << L'\n';
			std::exit(-1);
		}
		std::cout << buf << std::endl;
		try
		{
			clientSocket << buf;
		}
		catch (const SocketException& exception)
		{
			std::wcout << (const wchar_t*)exception.wwhat() << L'\n';
			std::exit(-1);
		}
	}
	return 0;
}
