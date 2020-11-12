#include <iostream>

#define BUFFER_SIZE 4096u // 4 Mb

// https://www.geeksforgeeks.org/nat-hole-punching-in-computer-network/
// https://stackoverflow.com/questions/55885656/is-it-possible-to-address-a-server-client-socket-in-an-another-network-c

#include "ServerSocket.h"
#include "Socket.h"

int main()
{
	ServerSocket socket("192.168.0.200", (size_t)BUFFER_SIZE);
	Socket clientSocket = socket.getNewClient();
	// While loop: accept and echo message back to client
	char buf[BUFFER_SIZE];

	while (true)
	{
		memset(buf, 0, BUFFER_SIZE);

		// Wait for client to send data

		clientSocket >> buf;
		{
			//buf[bytesReceived] = '\0';
			std::cout << buf << std::endl;
		}

		clientSocket << buf;
	}
	return 0;
}