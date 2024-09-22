#include "../header/header.hpp"

const std::string REPOS_DIR = "repos/";
const std::string LOGIN_MESSAGE = "Login completed!";

namespace fs = std::filesystem;

void receive_file(SOCKET client_socket, const std::string& file_name) {
    int bytesReceived, file_size;
    std::ofstream file(file_name, std::ios::binary);

    if (!file) 
        return perror("Could not open file");

    read(client_socket, &file_size, sizeof(file_size));

    std::string buffer(BUFFER_SIZE, '\0');

    for (int total = 0; total < bytesReceived; total += bytesReceived) {
        bytesReceived = recv(client_socket, buffer.data(), BUFFER_SIZE, 0);

        file.write(buffer.data(), bytesReceived);
    }

    if (bytesReceived == -1) 
        perror("Erro ao receber dados");

    file.close();
}

void handle_command(comm_line line, client curr_client, SOCKET client_socket) {
    std::string dir_path = REPOS_DIR + curr_client.name;
    std::string filename = dir_path + "/" + line.arg;

    if (line.comm == "create") {
        std::ofstream client_file(filename);

        if (!client_file) {
            std::cerr << "File create error: " << strerror(errno) << std::endl;
            return;
        }

        std::cout << "File created: " << filename << std::endl;
        client_file.close();
    } else if (line.comm == "delete") {
        if (fs::exists(filename)) {
            if (!fs::remove(filename)) {
                std::cerr << "Removing file error: " << strerror(errno) << std::endl;
            } else {
                std::cout << "File removed: " << filename << std::endl;
            }
        } else {
            std::cerr << "File not found: " << filename << std::endl;
        }
    } else if(line.comm == "push") {
        receive_file(client_socket, filename);
    }
}

int accept_client(SOCKET server_socket, SOCKET client_socket, std::string buffer) {
    comm_line command = comm_line();
    client *new_client = new client(client_socket);

    send(client_socket, LOGIN_MESSAGE.data(), BUFFER_SIZE, 0);
    std::cout << "Login: \"" << new_client -> name << "\"!" << std::endl;

    std::filesystem::create_directory(REPOS_DIR + new_client->name);

    for(int received_bytes; (received_bytes = recv(client_socket, buffer.data(), BUFFER_SIZE, 0)) > -1; buffer.resize(BUFFER_SIZE)) {
        resize_till_null(buffer);
        command.clean_fields();

        if(buffer == "exit") 
            break;

        command.from_line(buffer);
        command.resize_fields();

        handle_command(command, *new_client, client_socket);
    }

    std::cout << "Logout: \"" << new_client -> name << "\"!" << std::endl;

    return EXIT_SUCCESS;
}

void handle_connect(SOCKET server_socket, SOCKET client_socket, std::string buffer, std::vector<std::thread> &threads) {
    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, server_socket, client_socket, buffer));
    }
}

void handle_input() {
    std::string input(COMMAND_SIZE, '\0');

    while(std::getline(std::cin, input)) {
        if (input == "exit")
            return;            
    }
}

int main(int argc, char **argv) {
    SOCKET server_socket, client_socket;
    int n_bind, n_listen;
    std::string buffer(1024, '\0');

    std::vector<std::thread> threads;

    exit_if_error(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = {sin_family: AF_INET, 
                                    sin_port: htons(PORT),
                                    sin_addr: {s_addr: INADDR_ANY}};
    memset(&serv_addr.sin_zero, '0', 8);

    exit_if_error(n_bind = bind(server_socket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                  BIND_ERROR);

    exit_if_error(n_listen = listen(server_socket, MAX_BACKLOG), 
                  LISTEN_ERROR);

    std::cout << "Waiting for connections..." << std::endl;

    std::thread input_thread(handle_input);
    std::thread connect_thread(handle_connect, server_socket, client_socket, buffer, std::ref(threads));

    input_thread.join();

    for(auto& th: threads) 
        th.join();

    std::cout << "Shutting down the server..." << std::endl;

    close(client_socket);
    close(server_socket);

    exit(EXIT_SUCCESS);
}