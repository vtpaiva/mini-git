#include "../header/header.hpp"

const std::string REPOS_DIR = "repos/";
constexpr const char* LOGIN_MESSAGE = "Login completed!";

int accept_client(SOCKET server_socket, SOCKET client_socket, std::string buffer) {
    client *new_client = new client(client_socket);

    send(client_socket, LOGIN_MESSAGE, BUFFER_SIZE, 0);
    std::cout << "Login: \"" << new_client -> name << "\"!" << std::endl;

    std::filesystem::create_directory(REPOS_DIR + new_client->name);

    int received_bytes = recv(client_socket, &buffer[0], BUFFER_SIZE, 0) - 1;
    buffer.resize(received_bytes);

    std::ofstream client_file(REPOS_DIR + new_client->name + "/" + buffer);
    client_file.close();

    std::cout << "Logout: \"" << new_client -> name << "\"!" << std::endl;

    return EXIT_SUCCESS;
}

void handle_connect(SOCKET server_socket, SOCKET client_socket, std::string buffer, std::vector<std::thread> &threads) {
    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, server_socket, client_socket, buffer));
    }
}

int handle_input() {
    std::string input(COMMAND_SIZE, '\0');

    while(std::getline(std::cin, input)) {
        if (input == "exit")
            return EXIT_SUCCESS;            
    }

    return EXIT_FAILURE;
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