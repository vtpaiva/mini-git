#include "../header/header.hpp"

std::string get_ip() {
    std::string hostname(BUFFER_SIZE, '\0');
    exit_if_error(gethostname(&hostname[0], 1023), "hostname");

    struct addrinfo hints = {0}, *res;

    hints = {ai_family: AF_INET, ai_socktype: SOCK_STREAM};

    exit_if_error(getaddrinfo(&hostname[0], NULL, &hints, &res), "adress info");

    char ip_str[INET_ADDRSTRLEN];
    struct sockaddr_in *ipv4 = (struct sockaddr_in*) res->ai_addr;

    inet_ntop(AF_INET, &(ipv4->sin_addr), ip_str, INET_ADDRSTRLEN);

    freeaddrinfo(res);
    return std::string(ip_str);
}

void send_client_input(const char* input, const SOCKET &socket, char buffer[]) {
    std::cout << "Write your " << input << ": " << std::endl;
    fgets(buffer, NAME_SIZE, stdin);
    send(socket, buffer, strlen(buffer), 0);
}

void get_login(const SOCKET &socket, char buffer[]) {
    send_client_input("name", socket, buffer);

    send_client_input("password", socket, buffer);
}

void send_message(const SOCKET &network_socket, char client_buffer[], char server_buffer[]) {
    get_login(network_socket, client_buffer);

    recv(network_socket, server_buffer, BUFFER_SIZE, 0);
    std::cout << server_buffer << std::endl;

    std::cout << "Write a file name:" << std::endl;
    fgets(client_buffer, PASSWORD_SIZE, stdin);
    send(network_socket, client_buffer, strlen(client_buffer), 0);
}

int main(int argc, char **argv) {
    SOCKET network_socket = 0;
    char server_buffer[BUFFER_SIZE] = {0}, client_buffer[BUFFER_SIZE] = {0};

    std::string hostname(BUFFER_SIZE, '\0');

    struct sockaddr_in serv_addr;

    exit_if_error(gethostname(&hostname[0], 1023),
                  "hostname");

    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, sin_port: htons(PORT)};

    std::string ip_address = get_ip();

    exit_if_error(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    send_message(network_socket, client_buffer, server_buffer);

    close(network_socket);

    return EXIT_SUCCESS;
}