#include <iostream>
#include <WinSock2.h>
#include <string>
#include <cstdio>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class Socketfunk
{

public:


private:

};


int main()
{

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup feilet." << std::endl;
		return 1;
	}

	char buffer[1024];
	SOCKET server;
	SOCKET client_socket;
	int port_adress;
	std::cout << "Angi port: ";
	std::cin >> port_adress;

	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server == SOCKET_ERROR)
	{
		perror("Socket error");
		return 1;
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port_adress);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		perror("Bind error");
		return 1;

	}
	if (listen(server, 5) == SOCKET_ERROR)
	{
		perror("Lytte error");
		return 1;
	}
	std::cout << "Server lytter paa port " << port_adress << std::endl;

	while (true)
	{
		sockaddr_in client_addr;
		int client_len = sizeof(client_addr);

		client_socket = accept(server, (struct sockaddr*)&client_addr, &client_len);

		if (client_socket == SOCKET_ERROR)
		{
			perror("Acccept error");
			continue;
		}
		int bytes_motatt = recv(client_socket, buffer, sizeof(buffer), 0);

		if (bytes_motatt == SOCKET_ERROR)
		{
			perror("Mottak error");
			continue;
		}
		send(client_socket, buffer, bytes_motatt, 0);


	}
}