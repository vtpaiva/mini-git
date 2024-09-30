#include "../header/header.hpp"

const std::string LOGIN_MESSAGE = "Login completed!";

std::string curr_repo(client client) {
    std::string tmp(ARG_SIZE, '\0');

    if(std::sscanf(client.curr_dir.data(), "%*[^/]/%s", tmp.data()))
        return tmp;

    return "Error\n";
}

void add_file(comm_line command, client &client) {
    std::string filename(client.curr_dir + "/" + command.arg);
    std::ofstream client_file(filename);

    if (!client_file) {
        std::cerr << "File create error: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "File created: " << filename << std::endl;
    client_file.close();
}

inline void change_repo(comm_line command, client &client) {
    if(!fs::is_directory(REPOS_DIR + client.name + "/" + command.arg))
        fs::create_directory(REPOS_DIR + client.name + "/" + command.arg);

    client.curr_dir = REPOS_DIR + client.name + "/" + command.arg;
}

inline void change_repo(std::string repo_name, client &client) {
    if(!fs::is_directory(REPOS_DIR + client.name + "/" + repo_name))
        fs::create_directory(REPOS_DIR + client.name + "/" + repo_name);

    client.curr_dir = REPOS_DIR + client.name + "/" + repo_name;
}

inline void change_to_external_repo(comm_line command, client &client) {
    if(fs::is_directory(REPOS_DIR + command.arg))
        client.curr_dir = REPOS_DIR + command.arg;
}

void remove_comm(SOCKET socket, comm_line command, client &client) {
    if(fs::is_directory(client.curr_dir + "/" + command.arg))
        fs::remove_all(client.curr_dir + "/" + command.arg);
    else if(fs::is_directory(REPOS_DIR + client.name + "/" + command.arg)) {
        if(command.arg == "main") {
            send(socket, MAIN_EXCEPTION, sizeof(MAIN_EXCEPTION), 0);
            return;
        }

        std::string repo_to_delete(REPOS_DIR + client.name + "/" + command.arg);

        if(client.curr_dir == repo_to_delete) {
            std::string buffer(BUFFER_SIZE, '\0');

            send(socket, CURR_REPO_ERROR, BUFFER_SIZE, 0);

            int bytes_received = recv(socket, buffer.data(), BUFFER_SIZE, 0);

            resize_till_null(buffer);

            change_repo(buffer, std::ref(client));

            fs::remove_all(repo_to_delete);

            return;
        }

        fs::remove_all(repo_to_delete);
    } else {
        if (fs::exists(client.curr_dir + "/" + command.arg)) {
            if (!fs::remove(client.curr_dir + "/" + command.arg)) {
                std::cerr << "Removing file error: " << strerror(errno) << std::endl;
            } else {
                std::cout << "File removed: " << command.arg << std::endl;
            }
        } else {
            std::cerr << "File not found: " << command.arg << std::endl;
        }
    }

    send(socket, OK_MESSAGE, std::strlen(OK_MESSAGE), 0);
}

void execute_comm(SOCKET socket, client curr_client, std::string terminal_comm) {
    FILE* pipe;
    char flag = '1';
    std::string buffer(BUFFER_SIZE, '\0');

    pipe = popen(terminal_comm.c_str(), "r");
        
    if (!pipe)
        return perror("Command error");

    while (fgets(buffer.data(), BUFFER_SIZE, pipe)) {
        resize_till_null(buffer);

        send(socket, buffer.data(), BUFFER_SIZE, 0);
        recv(socket, &flag, sizeof(flag), 0);

        if(!flag)
            return;
    }

    pclose(pipe);

    send(socket, &END, sizeof(END), 0);
}

static inline void handle_command(comm_line command, client &curr_client, SOCKET socket) {
    std::string filename = curr_client.curr_dir + "/" + command.arg, tmp;

    if (command.comm == "create")
        add_file(command, std::ref(curr_client));
    else if (command.comm == "delete") 
        remove_comm(socket, command, std::ref(curr_client));
    else if(command.comm == "push") 
        receive_files(socket, curr_client.curr_dir + "/");
    else if(command.comm == "pull")
        send_files(socket, curr_client.curr_dir, command);
    else if(command.comm == "files")
        execute_comm(socket, curr_client, "ls -C " + curr_client.curr_dir + "/" + command.arg);
    else if(command.comm == "show")
        execute_comm(socket, curr_client, "cat " + curr_client.curr_dir + "/" + command.arg);
    else if(command.comm == "repo") {
        if(command.arg == "")
            send(socket, curr_repo(curr_client).data(), ARG_SIZE, 0);
        else
            change_repo(command, std::ref(curr_client));
    }
    else if(command.comm == "repos")
        execute_comm(socket, curr_client, "ls -C " + REPOS_DIR + curr_client.name);
    else if(command.comm == "xrepo") 
        change_to_external_repo(command, std::ref(curr_client));
    else if(command.comm == "comm")
        execute_comm(socket, curr_client, command.arg);
}

void get_initial_repo(SOCKET socket, client &client, std::string& buffer) {
    int received_bytes;

    if(fs::exists(REPOS_DIR + client.name)) {
        send(socket, &RECEIVED, sizeof(RECEIVED), 0);
        execute_comm(socket, client, "ls -C " + REPOS_DIR + client.name);
    } else
        send(socket, &NOT_RECEIVED, sizeof(NOT_RECEIVED), 0);

    received_bytes = recv(socket, buffer.data(),BUFFER_SIZE, 0) - 1;
    buffer.resize(received_bytes);

    change_repo(buffer, std::ref(client));

    buffer.resize(BUFFER_SIZE);
    fill_string(buffer);
}

int accept_client(SOCKET socket, std::string buffer) {
    comm_line command = comm_line();
    client *new_client = new client(socket);

    send(socket, LOGIN_MESSAGE.data(), BUFFER_SIZE, 0);
    std::cout << "Login: \"" << new_client -> name << "\"!" << std::endl;

    std::filesystem::create_directory(REPOS_DIR + new_client -> name);
    std::filesystem::create_directory(new_client -> curr_dir);

    for(int received_bytes; (received_bytes = recv(socket, buffer.data(), BUFFER_SIZE, 0)) > -1; buffer.resize(BUFFER_SIZE)) {
        resize_till_null(buffer);

        if(buffer == "exit") 
            break;

        command.format_from_buffer(buffer);

        handle_command(command, std::ref(*new_client), socket);
    }

    std::cout << "Logout: \"" << new_client -> name << "\"!" << std::endl;

    return EXIT_SUCCESS;
}

void handle_connect(SOCKET server_socket, SOCKET client_socket, std::string buffer, std::vector<std::thread> &threads) {
    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, client_socket, buffer));
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
    int n_bind, n_listen;
    SOCKET server_socket, client_socket;

    std::vector<std::thread> threads;

    exit_if_error(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = default_server_addr;
    memset(serv_addr.sin_zero, '0', 8);

    exit_if_error(n_bind = bind(server_socket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                  BIND_ERROR);

    exit_if_error(n_listen = listen(server_socket, MAX_BACKLOG), 
                  LISTEN_ERROR);

    std::cout << "Waiting for connections..." << std::endl;

    std::thread input_thread(handle_input);
    std::thread connect_thread(handle_connect, server_socket, client_socket, std::string(1024, '\0'), std::ref(threads));

    input_thread.join();

    for(auto& th: threads) 
        th.join();

    std::cout << "Shutting down the server..." << std::endl;

    close(client_socket);
    close(server_socket);

    exit(EXIT_SUCCESS);
}