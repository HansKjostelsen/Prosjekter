#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <csignal>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
SOCKET server_socket = INVALID_SOCKET;
void cleanup() {
	if (server_socket != INVALID_SOCKET) {
		closesocket(server_socket);
	}
	WSACleanup();
	std::cout << "Serveren er avsluttet." << std::endl;
}
void signalHandler(int signum) {
	std::cout << "Signal " << signum << " mottatt. Avslutter serveren..." << std::endl;
	cleanup();
	exit(signum);
}
int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Bruk: " << argv[0] << " <port>" << std::endl;
		return 1;
	}
	int port = atoi(argv[1]);
	std::cout << "Valgt port er " << port << "\n";
	// Initialiser Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup feilet." << std::endl;
		return 1;
	}
	// Opprett server-socket
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		std::cerr << "Klarte ikke lage socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	// Binder socket til porten
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);
	if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) ==
		SOCKET_ERROR) {
		std::cerr << "Bind feilet: " << WSAGetLastError() << std::endl;
		cleanup();
		return 1;
	}
	// Sett socket til å lytte
	if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Lytting feilet: " << WSAGetLastError() << std::endl;
		cleanup();
		return 1;
	}
	std::cout << "Serveren lytter paa port " << port << std::endl;
	// Registrer signalhåndterer for Ctrl+C (SIGINT) og SIGTERM
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	while (true) {
		// Aksepter en klient
		sockaddr_in client_addr;
		int client_size = sizeof(client_addr);
		SOCKET client_socket = accept(server_socket, (sockaddr*)&
			client_addr, &client_size);
		if (client_socket == INVALID_SOCKET) {
			std::cerr << "Accept feilet: " << WSAGetLastError() << std::endl;
			continue; // Gå tilbake til lytting hvis en feil oppstår
		}
		std::cout << "Klient koblet til." << std::endl;
		char buffer[1024];
		while (true) {
			int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
			if (bytes_received > 0) {
				buffer[bytes_received] = '\0';
				std::cout << "Melding fra klient: " << buffer << std::endl;
				// Send bekreftelse tilbake til klienten
				std::string response = "Melding mottatt: " + std::string(buffer);
				send(client_socket, response.c_str(), response.length(), 0);
			}
			else if (bytes_received == 0) {
				std::cout << "Klient koblet fra." << std::endl;
				closesocket(client_socket);
				break; // Gå tilbake til å vente på en ny klient
			}
			else {
				std::cerr << "Feil i mottak: " << WSAGetLastError() << std::endl;
				closesocket(client_socket);
				break; // Gå tilbake til lytting
			}
		}
	}
	cleanup();
	return 0;