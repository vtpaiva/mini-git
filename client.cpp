#include "header.hpp"

int main() {
    SOCKET clientSocket = 0;
    char buffer[BUFFER_SIZE] = {0};

    struct sockaddr_in serv_addr;

    exitIfError(clientSocket = socket(AF_INET, SOCK_STREAM, 0), 
                SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, sin_port: htons(PORT)};

    exitIfError(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr), 
                ADRESS_ERROR);

    exitIfError(connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                CONNECT_ERROR);

    send(clientSocket, DEFAULT_MESSAGE, strlen(DEFAULT_MESSAGE), 0);
    std::cout << "Mensagem enviada para o servidor" << std::endl;

    read(clientSocket, buffer, BUFFER_SIZE);
    std::cout << "Mensagem recebida do servidor: " << buffer << std::endl;

    close(clientSocket);

    return EXIT_SUCCESS;
}
