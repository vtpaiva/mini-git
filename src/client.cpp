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

ssize_t send_all(int socket, const void* buffer, size_t length) {
    ssize_t total_sent = 0;
    ssize_t sent;
    const char* ptr = (const char*) buffer;

    while (total_sent < length) {
        sent = send(socket, ptr + total_sent, length - total_sent, 0);
        if (sent == -1) {
            return -1;  // erro no envio
        }
        total_sent += sent;
    }

    return total_sent;
}

static inline void handle_command(SOCKET socket, comm_line command) {
    if(command.comm == "push") {
        char confirm;   
        int number_files = 0;
        std::string curr_file;

        if(fs::is_directory(LOCAL_DIR + command.arg)) {

            for (const auto& entry : fs::directory_iterator(LOCAL_DIR + command.arg))
               number_files++;

            number_files = htonl(number_files);

            send(socket, &number_files, sizeof(number_files), 0);

            for (const auto& entry : fs::directory_iterator(LOCAL_DIR + command.arg)) {
                curr_file = entry.path().filename().string();

                send(socket, curr_file.data(), sizeof(curr_file.data()), 0);
                recv(socket, &confirm, sizeof(confirm), 0);

                if(confirm)
                    send_file(socket, LOCAL_DIR + command.arg + "/" + entry.path().filename().string());
            }
        } else {
                number_files = htonl(1);
                send(socket, &number_files, sizeof(int), 0);

                send(socket, command.arg.data(), size(command.arg), 0);
                recv(socket, &confirm, sizeof(confirm), 0);

                if(confirm)
                    send_file(socket, LOCAL_DIR + command.arg);
        }
    }
}

void send_message(const SOCKET &network_socket, std::string &client_buffer, std::string &server_buffer) {
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

        command.clean_fields();
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

    send_message(network_socket, client_buffer, server_buffer);

    close(network_socket);

    exit(EXIT_SUCCESS);
}