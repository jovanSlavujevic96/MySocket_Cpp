#include <iostream>

#if defined(_MSC_VER)

#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#elif defined(__unix)

#endif

#define BUFFER_SIZE 4096 // 4 Mb

// https://www.geeksforgeeks.org/nat-hole-punching-in-computer-network/
// https://stackoverflow.com/questions/55885656/is-it-possible-to-address-a-server-client-socket-in-an-another-network-c

#include "ServerSocket.h"
#include "ClientSocket.h"

int main()
{
	ServerSocket socket("192.168.0.200");
	ClientSocket clientSocket = socket.getNewClient();
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