#include "../header/server.hpp"

int main(int argc, char **argv) {
    int n_bind, n_listen;
    SOCKET server_socket, client_socket;
    std::vector<std::thread> threads;

    // Initialize the server socket.
    exit_if_error(server_socket = socket(AF_INET, SOCK_STREAM, 0), 
                  SOCKET_CREATION_ERROR);

    struct sockaddr_in serv_addr = default_server_addr;

    exit_if_error(n_bind = bind(server_socket, (sockaddr*) &serv_addr, sizeof(sockaddr_in)), 
                  BIND_ERROR);

    exit_if_error(n_listen = listen(server_socket, MAX_BACKLOG), 
                  LISTEN_ERROR);

    std::cout << "Waiting for connections..." << std::endl;

    // Creating threads to handle input and connections from the clients.
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

// Return the client's current repository name.
std::string curr_repo(client client) {
    std::string tmp(ARG_SIZE, '\0');

    if(std::sscanf(client.curr_dir.data(), "%*[^/]/%s", tmp.data()))
        return tmp;

    return "Error\n";
}

// Create a new file in the client's repository.
void add_file(command_line command, client &client) {
    std::string filename(client.curr_dir + "/" + command.arg);
    std::ofstream client_file(filename);

    if (!client_file) {
        std::cerr << "File create error: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "File created: " << filename << std::endl;
    client_file.close();
}

// Function to change the client's current repository by the command.
inline void change_repo(command_line command, client &client) {
    if(!fs::is_directory(REPOS_DIR + client.name + "/" + command.arg))
        fs::create_directory(REPOS_DIR + client.name + "/" + command.arg);

    client.curr_dir = REPOS_DIR + client.name + "/" + command.arg;
}

// Function to change the client's current repository by the repos's name.
inline void change_repo(std::string repo_name, client &client) {
    if(!fs::is_directory(REPOS_DIR + client.name + "/" + repo_name))
        fs::create_directory(REPOS_DIR + client.name + "/" + repo_name);

    client.curr_dir = REPOS_DIR + client.name + "/" + repo_name;
}

// Function to change the client's current repository to another user's repository.
inline void change_to_external_repo(command_line command, client &client) {
    if(fs::is_directory(REPOS_DIR + command.arg))
        client.curr_dir = REPOS_DIR + command.arg;
}

// Function to execute the command to remove a file/directory/repository.
void remove_comm(SOCKET socket, command_line command, client &client) {
    // If the name matches, remove directory.
    if(fs::is_directory(client.curr_dir + "/" + command.arg))
        fs::remove_all(client.curr_dir + "/" + command.arg);
    // If the name matches, remove repository.
    else if(fs::is_directory(REPOS_DIR + client.name + "/" + command.arg)) {
        if(command.arg == "main") {
            send(socket, MAIN_EXCEPTION, sizeof(MAIN_EXCEPTION), 0);
            return;
        }

        std::string repo_to_delete(REPOS_DIR + client.name + "/" + command.arg);

        // If the client is trying to delete the current repository, tell somewhere to go.
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
    } 
    // If the name matches, remove file.
    else {
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

// Forks another user's repository.
void fork_repo(SOCKET socket, client &client, command_line command) {
    // Gets repository name.
    std::string repo_name(BUFFER_SIZE, '\0');

    std::sscanf(command.arg.data(), "%*[^/]/%s", repo_name.data());

    resize_till_null(repo_name);

    // Creates repository and for.
    change_repo("fork-" + repo_name, std::ref(client));

    fs::path source = REPOS_DIR + command.arg, destiny = client.curr_dir;

    fs::copy(source, destiny, fs::copy_options::recursive);
}

// Function to execute a command and send the output to the client.
void execute_comm(SOCKET socket, client curr_client, std::string terminal_comm) {
    FILE* pipe;
    char flag = RECEIVED;
    std::string buffer(BUFFER_SIZE, '\0');

    pipe = popen(terminal_comm.c_str(), "r");

    if (!pipe)
        return;

    // While there is data left, keep sending it to the client.
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

// Function to handle the commands from the client.
static inline void handle_command(command_line command, client &curr_client, SOCKET socket) {
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
    else if(command.comm == "fork")
        fork_repo(socket, std::ref(curr_client), command);
}

// Function to handle the connection from the client.
int accept_client(SOCKET socket, std::string buffer) {
    command_line command = command_line();
    client *new_client = new client(socket);

    send(socket, LOGIN_MESSAGE.data(), BUFFER_SIZE, 0);
    std::cout << "Login: \"" << new_client -> name << "\"!" << std::endl;

    std::filesystem::create_directory(REPOS_DIR + new_client -> name);
    std::filesystem::create_directory(new_client -> curr_dir);

    // While the client doesn't want to 'exit', keep receiving commands.
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

// Function to handle a client connection to the server, creating a thread.
void handle_connect(SOCKET server_socket, SOCKET client_socket, std::string buffer, std::vector<std::thread> &threads) {
    while(client_socket = accept(server_socket, nullptr, nullptr)) {
        threads.push_back(std::thread(accept_client, client_socket, buffer));
    }
}

// Function to handle exit command in the input.
void handle_input() {
    std::string input(COMMAND_SIZE, '\0');

    while(std::getline(std::cin, input)) {
        if (input == "exit")
            return;   


        std::cout << "Unknown command!" << std::endl;         
    }
}