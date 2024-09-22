#include "../header/header.hpp"

const std::string DIR = "local/";

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

void send_file(SOCKET network_socket, const std::string& file_name) {
    int bytes_sent, file_size;
    std::ifstream file(DIR + file_name, std::ios::binary | std::ios::ate);

    if (!file) {
        perror("File not found");
        return;
    }

    file_size = htonl(file.tellg());
    file.seekg(0, std::ios::beg);

    send(network_socket, &file_size, sizeof(file_size), 0);

    std::string buffer(BUFFER_SIZE, '\0'); 

    while(file.read(buffer.data(), buffer.size())) {
        if ((bytes_sent = send(network_socket, buffer.data(), BUFFER_SIZE, 0)) == -1) 
            return perror("Erro ao enviar últimos dados");
    }

    if(file.gcount() > 0) {
        if ((bytes_sent = send(network_socket, buffer.data(), file.gcount(), 0)) == -1) 
            return perror("Erro ao enviar últimos dados");
    }

    file.close();
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
inline void handle_command(SOCKET socket, comm_line command) {
    if(command.comm == "push") {
        send_file(socket, command.arg);
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