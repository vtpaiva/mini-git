#include "../header/header.hpp"

constexpr int MAX_CLIENTS =  5;
const char* message = "Hi, client";

void accept_client(SOCKET server_socket, SOCKET client_socket, char buffer[]) {
        exit_if_error(client_socket = accept(server_socket, nullptr, nullptr),
                      ACCEPT_ERROR);

        read(client_socket, buffer, BUFFER_SIZE);
        std::cout << "Client message: " << buffer << std::endl;

        send(client_socket, message, strlen(message), 0);
        std::cout << "Message sent to client." << std::endl;
}

int main(int argc, char **argv) {
    SOCKET server_socket, client_socket;
    int n_bind, n_listen;
    char buffer[BUFFER_SIZE] = {0};

    exit_if_error(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
                SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = {sin_family: AF_INET, 
                                    sin_port: htons(PORT),
                                    sin_addr: {s_addr: INADDR_ANY}};

    socklen_t addr_len = sizeof(serv_addr);
    memset(&serv_addr.sin_zero, '0', 8);

    exit_if_error(n_bind = bind(server_socket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                  BIND_ERROR);

    exit_if_error(n_listen = listen(server_socket, MAX_BACKLOG), 
                  LISTEN_ERROR);

    std::cout << "Waiting for conections..." << std::endl;

    each(MAX_CLIENTS) {
        accept_client(server_socket, client_socket, buffer);
    }

    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;
}