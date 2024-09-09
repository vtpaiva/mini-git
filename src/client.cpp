#include "../header/header.hpp"

int main() {
    SOCKET network_socket = 0;
    char buffer[BUFFER_SIZE] = {0};

    const char* message = "Hi, server!";

    struct sockaddr_in serv_addr;

    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, sin_port: htons(PORT)};

    exit_if_error(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    write(network_socket, message, strlen(message));
    std::cout << "Mensagem enviada para o servidor" << std::endl;

    read(network_socket, buffer, BUFFER_SIZE);
    std::cout << "Mensagem recebida do servidor: " << buffer << std::endl;

    close(network_socket);

    return EXIT_SUCCESS;
}
