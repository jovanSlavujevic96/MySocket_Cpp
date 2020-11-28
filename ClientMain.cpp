#include <iostream>

#define BUFFER_SIZE 4096u // 4 Mb

// https://www.geeksforgeeks.org/nat-hole-punching-in-computer-network/
// https://stackoverflow.com/questions/55885656/is-it-possible-to-address-a-server-client-socket-in-an-another-network-c

#include "ClientSocket.h"

int main()
{
	ClientSocket socket("192.168.0.197");
	// While loop: accept and echo message back to client
	char buf[BUFFER_SIZE];
	const char* msgToSend = "Hello from client";

	while (true)
	{
		std::cin.get();

		// client to send data
		socket << msgToSend;

		memset(buf, 0, BUFFER_SIZE);
		socket >> buf;
		{
			//buf[bytesReceived] = '\0';
			std::cout << buf << std::endl;
		}

	}
	return 0;
}