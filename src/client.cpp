#include "../header/header.hpp"

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

    PRINT_GREEN;

    while ((bytes_received = recv(socket, buffer.data(), BUFFER_SIZE, 0)) > 0) {
        resize_till_null(buffer);

        if (!std::strcmp(buffer.c_str(), "EOF"))
            return;

        send(socket, &RECEIVED, sizeof(RECEIVED), 0);

        std::cout << buffer;

        buffer.resize(BUFFER_SIZE);
        fill_string(buffer);
    }

}

static inline void handle_command(SOCKET socket, comm_line command) {
    if(command.comm == "create" || command.comm == "delete")
        /*IGNORE*/;
    else if(command.comm == "push") 
        send_files(socket, LOCAL_DIR, command);
    else if(command.comm == "pull")
        receive_files(socket, LOCAL_DIR);
    else if(command.comm == "files")
        comm_output(socket);
    else if(command.comm == "show")
        comm_output(socket);
    else if(command.comm == "comm")
        comm_output(socket);
    else {
        PRINT_RED;
        system((command.comm + " " + LOCAL_DIR + command.arg).c_str());
    }

    PRINT_DEFAULT;
}

void send_messages(const SOCKET &network_socket, std::string &client_buffer, std::string &server_buffer) {
    comm_line command = comm_line();
    get_login(network_socket, client_buffer.data());

    recv(network_socket, server_buffer.data(), BUFFER_SIZE, 0);

    std::cout << "\nWrite your command:" << std::endl;

    while (std::getline(std::cin, client_buffer)) {
        send(network_socket, client_buffer.data(), BUFFER_SIZE, 0);

        if(client_buffer == "exit") 
            return;

        command.format_from_buffer(client_buffer);

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

    exit_if_error(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    send_messages(network_socket, client_buffer, server_buffer);

    close(network_socket);

    exit(EXIT_SUCCESS);
}