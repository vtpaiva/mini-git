#include "../header/header.hpp"

void comm_output(SOCKET socket) {
    ssize_t bytes_received;
    std::string buffer(BUFFER_SIZE, '\0');

    PRINT_GREEN;

    for(int n = 0; (bytes_received = recv(socket, buffer.data(), BUFFER_SIZE, 0)) > 0; n++) {
        resize_till_null(buffer);

        if (buffer.c_str()[0] == END) {
            if(!n)
                std::cout << "*Empty*" << std::endl;

            return;
        }

        send(socket, &RECEIVED, sizeof(RECEIVED), 0);

        std::cout << buffer;

        buffer.resize(BUFFER_SIZE);
        fill_string(buffer);
    }
}

void send_client_input(const char* input, const SOCKET &socket, char buffer[], const int input_size = BUFFER_SIZE) {
    std::cout << "Write your " << input << ": " << std::endl;
    fgets(buffer, input_size, stdin);
    send(socket, buffer, strlen(buffer), 0);
}

void remove_comm(SOCKET socket) {
    std::string buffer(BUFFER_SIZE, '\0');

    int received_bytes = recv(socket, buffer.data(), BUFFER_SIZE, 0);
    buffer.resize(received_bytes);

    if(buffer == OK_MESSAGE)
        return;

    if(buffer == MAIN_EXCEPTION) {
        std::cout << "Main repository cannot be deleted." << std::endl;
        return;
    }

    std::cout <<  "You're removing the current repository, tell somewhere else to go:\n" << std::endl;

    std::getline(std::cin, buffer);

    send(socket, buffer.data(), BUFFER_SIZE, 0);
}

void show_repo(SOCKET socket) {
    std::string buffer(ARG_SIZE, '\0');

    recv(socket, buffer.data(), ARG_SIZE, 0);

    resize_till_null(buffer);

    std::cout << buffer << std::endl;
}

static inline void handle_command(SOCKET socket, comm_line command) {
    if(command.comm == "create" || command.comm == "xrepo")
        /*IGNORE*/;
    else if(command.comm == "delete") 
        remove_comm(socket);
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
    else if(command.comm == "repo") {
        if(command.arg == "") 
            show_repo(socket);
    }
    else if(command.comm == "repos")
        comm_output(socket);
    else {
        PRINT_RED;
        system((command.comm + " " + LOCAL_DIR + command.arg).c_str());
    }

    PRINT_DEFAULT;
}

void send_messages(const SOCKET &socket, std::string &buffer) {
    comm_line command = comm_line();
    send_client_input("name", socket, buffer.data(), NAME_SIZE);

    buffer.resize(BUFFER_SIZE);
    fill_string(buffer);

    recv(socket, buffer.data(), BUFFER_SIZE, 0);

    std::cout << "\nWrite your command:" << std::endl;

    while (std::getline(std::cin, buffer)) {
        send(socket, buffer.data(), BUFFER_SIZE, 0);

        if(buffer == "exit") 
            return;

        command.format_from_buffer(buffer);

        handle_command(socket, command);
    }
}

int main(int argc, char **argv) {
    SOCKET network_socket = 0;
    std::string server_buffer(BUFFER_SIZE, '\0'), client_buffer(BUFFER_SIZE, '\0');

    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = default_client_addr;

    // Put your ip adress in here.
    const char *client_ip = "127.0.0.1";

    exit_if_error(inet_pton(AF_INET, client_ip, &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    send_messages(network_socket, client_buffer);

    close(network_socket);

    exit(EXIT_SUCCESS);
}