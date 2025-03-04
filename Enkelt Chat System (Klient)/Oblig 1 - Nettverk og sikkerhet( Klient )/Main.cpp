#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Bruker: " << argv[0] << " <server IP> <port>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = atoi(argv[2]);
    std::cout << "Kobler til server paa " << server_ip << " paa port " << port << "\n";

    // Initialiser Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup feilet." << std::endl;
        return 1;
    }

    // Opprett klient-socket
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Klarte ikke lage socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Definer serveradresse
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // Koble til serveren
    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Kobling feilet: " << WSAGetLastError() << "\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    std::cout << "Koblet til server." << "\n";

    // Kommunikasjonsløkke
    char buffer[1024];
    std::string user_input;
    while (true) {
        std::cout << "Angi melding til server (skriv 'avslutt' for aa avslutte): ";
        std::getline(std::cin, user_input);

        if (user_input == "avslutt") {
            std::cout << "Avslutter." << "\n";
            break;
        }

        // Send melding til server
        if (send(client_socket, user_input.c_str(), user_input.length(), 0) == SOCKET_ERROR) {
            std::cerr << "Sending feilet: " << WSAGetLastError() << "\n";
            break;
        }

        // Motta svar fra serveren
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            std::cout << "Server: " << buffer << "\n";
        }
        else if (bytes_received == 0) {
            std::cout << "Server avkoblet." << std::endl;
            break;
        }
        else {
            std::cerr << "Motta error: " << WSAGetLastError() << "\n";
            break;
        }
    }

    // Lukk socket og opprydding
    closesocket(client_socket);
    WSACleanup();
    return 0;
}