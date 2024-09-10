#include "../header/header.hpp"

std::string file_path(client &client, char buffer[]) {
    std::stringstream full_path;
    full_path << client.name << "/" << std::string(buffer);

    std::string path_to_file = full_path.str();

    remove_char(path_to_file, '\0');

    return path_to_file;
}

void accept_client(SOCKET server_socket, SOCKET client_socket, char buffer[]) {
    client *new_client = new client(client_socket);

    new_client -> copy_data_to_buffer(buffer);

    send(client_socket, buffer, BUFFER_SIZE, 0);
    std::cout << "Login sent to client.\n" << std::endl;

    std::filesystem::create_directory(new_client->name);

    recv(client_socket, buffer, BUFFER_SIZE, 0);
    std::ofstream client_file(file_path(*new_client, buffer));

    client_file.close();
}

int main(int argc, char **argv) {
    SOCKET server_socket, client_socket;
    int n_bind, n_listen;
    char server_buffer[BUFFER_SIZE] = {0};

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

    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, server_socket, client_socket, server_buffer));
    }

    for (std::thread &t : threads) {
        t.join();
    }

    close(client_socket);
    close(server_socket);

    return EXIT_SUCCESS;
}