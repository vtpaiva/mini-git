#include "../header/header.hpp"

std::string get_ip() {
    std::string hostname(BUFFER_SIZE, '\0');
    exit_if_error(gethostname(hostname.data(), 1023), "hostname");

    struct addrinfo hints = {0}, *res;

    hints = {ai_family: AF_INET, ai_socktype: SOCK_STREAM};

    exit_if_error(getaddrinfo(hostname.data(), NULL, &hints, &res), "adress info");

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

void comm_output(SOCKET socket) {
    ssize_t bytes_received;
    std::string buffer(BUFFER_SIZE, '\0');

    while ((bytes_received = recv(socket, buffer.data(), BUFFER_SIZE - 1, 0)) > 0) {
        resize_till_null(buffer);

        send(socket, &RECEIVED, sizeof(RECEIVED), 0);

        if (!std::strcmp(buffer.c_str(), "EOF"))
            break;

        std::cout << buffer;

        resize_till_null(buffer);
        fill_string(buffer);
    }
}

static inline void handle_command(SOCKET socket, comm_line command) {
    if(command.comm == "push")
        send_files(socket, LOCAL_DIR, command);
    else if(command.comm == "pull")
        receive_files(socket, LOCAL_DIR);
    else if(command.comm == "comm" && command.arg == "ls")
        comm_output(socket);
}

void send_messages(const SOCKET &network_socket, std::string &client_buffer, std::string &server_buffer) {
    comm_line command = comm_line();
    get_login(network_socket, client_buffer.data());

    recv(network_socket, server_buffer.data(), BUFFER_SIZE, 0);
    std::cout << server_buffer << std::endl;

    std::cout << "\nWrite your command: (create, or delete 'file_name')" << std::endl;

    while (std::getline(std::cin, client_buffer)) {
        send(network_socket, client_buffer.data(), BUFFER_SIZE, 0);

        if(client_buffer == "exit") 
            return;

        command.clean_fields();
        command.from_line(client_buffer);
        command.resize_fields();

        handle_command(network_socket, command);
    }
}

int main(int argc, char **argv) {
    SOCKET network_socket = 0;
    std::string server_buffer(BUFFER_SIZE, '\0'), client_buffer(BUFFER_SIZE, '\0');

    struct sockaddr_in serv_addr;

    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    serv_addr = {sin_family: AF_INET, sin_port: htons(PORT)};

    std::string ip_address = get_ip();

    exit_if_error(inet_pton(AF_INET, ip_address.c_str(), &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    send_messages(network_socket, client_buffer, server_buffer);

    close(network_socket);

    exit(EXIT_SUCCESS);
}