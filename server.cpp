#include "header.hpp"

int main(int argc, char **argv) {
    SOCKET serverSocket, clientSocket;
    int nBind, nListen;
    char buffer[BUFFER_SIZE] = {0};

    struct sockaddr_in serv_addr;
    socklen_t addr_len = sizeof(serv_addr);

    exitIfError(serverSocket = socket(AF_INET, SOCK_STREAM, 0), 
                SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, 
                 sin_port: htons(PORT),
                 sin_addr: {s_addr: INADDR_ANY}};
    memset(&serv_addr.sin_zero, '0', 8);

    exitIfError(nBind = bind(serverSocket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                BIND_ERROR);

    exitIfError(nListen = listen(serverSocket, MAX_BACKLOG), 
                LISTEN_ERROR);

    std::cout << "Servidor esperando por conexões..." << std::endl;

    exitIfError(clientSocket = accept(serverSocket, (struct sockaddr *)&serv_addr, &addr_len),
                ACCEPT_ERROR);

    read(clientSocket, buffer, BUFFER_SIZE);
    std::cout << "Mensagem recebida do cliente: " << buffer << std::endl;

    send(clientSocket, DEFAULT_MESSAGE, strlen(DEFAULT_MESSAGE), 0);
    std::cout << "Mensagem enviada para o cliente" << std::endl;

    close(clientSocket);
    close(serverSocket);

    return EXIT_SUCCESS;
}