#include "../header/header.hpp"

void accept_client(SOCKET server_socket, SOCKET client_socket, char server_buffer[], char client_buffer[], int id) {
    read(client_socket, client_buffer, BUFFER_SIZE);
    std::cout << "Client message: " << client_buffer;

    fgets(server_buffer, BUFFER_SIZE, stdin);

    send(client_socket, server_buffer, BUFFER_SIZE, 0);
    std::cout << "Message sent to client.\n" << std::endl;
}

int main(int argc, char **argv) {
    SOCKET server_socket, client_socket;
    int n_bind, n_listen;
    char server_buffer[BUFFER_SIZE] = {0}, client_buffer[BUFFER_SIZE] = {0};

    std::vector<std::thread> threads;

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

    std::cout << "Waiting for connections..." << std::endl;

    int count = 0;

    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, server_socket, client_socket, server_buffer, client_buffer, count++));
    }

    for (std::thread &t : threads) {
        t.join();
    }

    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;
}