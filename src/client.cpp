#include "../header/header.hpp"

void send_message(SOCKET network_socket, char client_buffer[], char server_buffer[]) {
    std::cout << "Write your message:" << std::endl;

    fgets(client_buffer, BUFFER_SIZE, stdin);

    write(network_socket, client_buffer, BUFFER_SIZE);
    std::cout << "Message sent to server." << std::endl;

    read(network_socket, server_buffer, BUFFER_SIZE);
    std::cout << "Server message: " << server_buffer << std::endl;
}

int main(int argc, char **argv) {
    SOCKET network_socket = 0;
    char server_buffer[BUFFER_SIZE] = {0}, client_buffer[BUFFER_SIZE] = {0};

    struct sockaddr_in serv_addr;

    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, sin_port: htons(PORT)};

    exit_if_error(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    send_message(network_socket, client_buffer, server_buffer);

    close(network_socket);

    return EXIT_SUCCESS;
}
