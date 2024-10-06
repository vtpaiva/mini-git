#include "../header/client.hpp"

int main(int argc, char **argv) {
    SOCKET network_socket = 0;
    std::string client_buffer(BUFFER_SIZE, '\0');

    // Initalizes the network socket.
    exit_if_error(network_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = default_client_addr;

    // Put your ip adress in here.
    const char *client_ip = "127.0.0.1";

    exit_if_error(inet_pton(AF_INET, client_ip, &serv_addr.sin_addr), 
                  ADRESS_ERROR);

    exit_if_error(connect(network_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)),
                  CONNECT_ERROR);

    handle_connection(network_socket, client_buffer);

    close(network_socket);

    exit(EXIT_SUCCESS);
}

// Function to print the output from a command executed by the server.
void comm_output(SOCKET socket) {
    ssize_t bytes_received;
    std::string buffer(BUFFER_SIZE, '\0');

    PRINT_GREEN;

    // While the server keep sending data, print it.
    for(int n = 0; (bytes_received = recv(socket, buffer.data(), BUFFER_SIZE, 0)) > 0; n++) {
        resize_till_null(buffer);

        // While the server has not send the stop flag, continue.
        if (buffer.c_str()[0] == END) {
            if(!n)
                std::cout << "*Empty*" << std::endl;

            return;
        }

        // Send the receive confirmation to the server.
        send(socket, &RECEIVED, sizeof(RECEIVED), 0);

        std::cout << buffer;

        buffer.resize(BUFFER_SIZE);
        fill_string(buffer);
    }
}

// Send a certain input to the server from the stdin.
void get_client_input(const char* input, const SOCKET &socket, char buffer[], const int input_size) {
    std::cout << "Write your " << input << ": " << std::endl;
    fgets(buffer, input_size, stdin);
    send(socket, buffer, strlen(buffer), 0);
}

// Function to execute the remove file/directory/repository command.
void remove_comm(SOCKET socket) {
    std::string buffer(BUFFER_SIZE, '\0');

    int received_bytes = recv(socket, buffer.data(), BUFFER_SIZE, 0);
    buffer.resize(received_bytes);

    // If the deletion did not cause an error, return.
    if(buffer == OK_MESSAGE)
        return;

    // If the user tried to remove the main repository, throw an error.
    if(buffer == MAIN_EXCEPTION) {
        std::cout << "Main repository cannot be deleted." << std::endl;
        return;
    }

    // If the user tried to delete his current repository, ask which one to go.
    std::cout <<  "You're removing the current repository, tell somewhere else to go:\n" << std::endl;

    std::getline(std::cin, buffer);

    send(socket, buffer.data(), BUFFER_SIZE, 0);
}

// Print the client's current repository.
void show_repo(SOCKET socket) {
    std::string buffer(ARG_SIZE, '\0');

    recv(socket, buffer.data(), ARG_SIZE, 0);

    resize_till_null(buffer);

    std::cout << "Current repository: "  + buffer << std::endl;
}

// Function to handle the client's command.
void handle_command(SOCKET socket, command_line command) {
    if(command.comm == "create")
        std::cout << "File created!" << std::endl;
    else if(command.comm == "delete") {
        remove_comm(socket);
        std::cout << "Succesfully deleted!" << std::endl;
    }
    else if(command.comm == "push") {
        send_files(socket, LOCAL_DIR, command);
        std::cout << "Files sent!" << std::endl;
    }
    else if(command.comm == "pull") {
        receive_files(socket, LOCAL_DIR);
        std::cout << "Files received!" << std::endl;
    }
    else if(command.comm == "files")
        comm_output(socket);
    else if(command.comm == "show")
        comm_output(socket);
    else if(command.comm == "comm")
        comm_output(socket);
    else if(command.comm == "repo") {
        if(command.arg == "") 
            show_repo(socket);
        else 
            std::cout << "Repository changed to: " + command.arg << std::endl;
    }
    else if(command.comm == "repos")
        comm_output(socket);
    else if(command.comm == "xrepo") 
        std::cout << "Repository changed to: " + command.arg << std::endl;
    else if(command.comm == "fork")
        std::cout << "Repository " + command.arg + " forked!" << std::endl;
    else {
        PRINT_RED;
        system((command.comm + " " + LOCAL_DIR + command.arg).c_str());
    }

    PRINT_DEFAULT;
}

// Function to handle the client's command sending to the server.
void handle_connection(const SOCKET &socket, std::string &buffer) {
    command_line command = command_line();
    get_client_input("name", socket, buffer.data(), NAME_SIZE);

    buffer.resize(BUFFER_SIZE);
    fill_string(buffer);

    recv(socket, buffer.data(), BUFFER_SIZE, 0);

    std::cout << "\nWrite your command:" << std::endl;

    // While the client doesn't want to 'exit, keep sending commands.
    while (std::getline(std::cin, buffer)) {
        send(socket, buffer.data(), BUFFER_SIZE, 0);

        if(buffer == "exit") 
            return;

        command.format_from_buffer(buffer);

        handle_command(socket, command);
    }
}