#include "../header/header.hpp"

constexpr int N_CLIENTS_ACCEPT =  5;
const char* message = "Hi, client";

int main(int argc, char **argv) {
    SOCKET server_socket, client_socket;
    int n_bind, n_listen;
    char buffer[BUFFER_SIZE] = {0};

    struct sockaddr_in serv_addr;
    socklen_t addr_len = sizeof(serv_addr);

    exit_if_error(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
                SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, 
                 sin_port: htons(PORT),
                 sin_addr: {s_addr: INADDR_ANY}};

    memset(&serv_addr.sin_zero, '0', 8);

    exit_if_error(n_bind = bind(server_socket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                  BIND_ERROR);

    exit_if_error(n_listen = listen(server_socket, MAX_BACKLOG), 
                  LISTEN_ERROR);

    std::cout << "Servidor esperando por conexões..." << std::endl;

    for(int i = 0; i < N_CLIENTS_ACCEPT; i++) {
        exit_if_error(client_socket = accept(server_socket, nullptr, nullptr),
                      ACCEPT_ERROR);

        read(client_socket, buffer, BUFFER_SIZE);
        std::cout << "Mensagem recebida do cliente: " << buffer << std::endl;

        send(client_socket, message, strlen(message), 0);
        std::cout << "Mensagem enviada para o cliente" << std::endl;
    }

    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;
}