#ifndef CONTS_H
#define CONTS_H

#include <iostream>
#include <fstream>  
#include <filesystem>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>    
#include <cstring>
#include <thread>
#include <vector>
#include <regex>

namespace fs = std::filesystem;

constexpr int PORT = 8080;
constexpr int MAX_BACKLOG = 32;
constexpr int BUFFER_SIZE = 1024;
constexpr int NAME_SIZE = 16;
constexpr int COMMAND_SIZE = 8;
constexpr int ARG_SIZE = 64;

const std::string LOCAL_DIR = "local/";
const std::string REPOS_DIR = "repos/";
constexpr const char RECEIVED = '1';
constexpr const char NOT_RECEIVED = '\0';
constexpr const char FILE_FLAG = '0';
constexpr const char FOLDER_FLAG = '1';
constexpr const char END = 'E';
constexpr const char* SOCKET_CREATION_ERROR = "Socket";
constexpr const char* BIND_ERROR = "Bind";
constexpr const char* LISTEN_ERROR = "Listen";
constexpr const char* ACCEPT_ERROR = "Accept";
constexpr const char* CONNECT_ERROR = "Connect";
constexpr const char* ADRESS_ERROR = "Adress";
constexpr const char* RECV_ERROR = "Message receiving";
constexpr const char* CURR_REPO_ERROR = "This is your current repository, tell somewhere to go:\n";
constexpr const char* OK_MESSAGE = "Sent!";
constexpr const char* MAIN_EXCEPTION = "main_exc";

const struct sockaddr_in default_server_addr = {.sin_family = AF_INET, 
                                                .sin_port = htons(PORT),
                                                .sin_addr = {s_addr: INADDR_ANY}},
                         default_client_addr = {.sin_family = AF_INET, 
                                                .sin_port = htons(PORT)};

#define PRINT_GREEN system("printf \"\\033[0;31m\"")
#define PRINT_RED system("printf \"\\033[0;32m\"")
#define PRINT_DEFAULT system("printf \"\\033[0m\"")
#define for_each(n) for(int It = 0; It < n; It++)

typedef int SOCKET;

void fill_string(std::string &st) {
    std::fill(st.begin(), st.end(), '\0');
}

// Returns error message if value means error.
void exit_if_error(const int value, const std::string err = "code") {
    if(value < 0) {
        std::cout << "Fail on " << err << "! (" << strerror(errno) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void remove_char(std::string &str, char rem = '\n') {
    str.erase(std::remove(str.begin(), str.end(), rem), str.end());
}

inline void resize_till_null(std::string &st) {
    auto pos = st.find('\0');

    if (pos != std::string::npos)
        st.resize(pos);
}

class client {
    public:

        std::string name, curr_dir;

        client(SOCKET &client_socket) : name(std::string(NAME_SIZE,0)), curr_dir(std::string(ARG_SIZE,0)){
            int received_bytes;

            received_bytes = recv(client_socket, name.data(), NAME_SIZE, 0) - 1;
            this -> name.resize(received_bytes);

            this -> curr_dir = REPOS_DIR + this -> name + "/main";
        }

    private:
};

class comm_line {
    public:

        std::string comm, arg;

        comm_line() : comm(COMMAND_SIZE, '\0') {}

        comm_line(std::string command, std::string arg) : comm(command), arg(arg) {}

        void resize_fields() {
            resize_till_null(this -> comm);
            resize_till_null(this -> arg);
        }

        void clean_fields() {
            this -> comm.resize(COMMAND_SIZE);
            fill_string(this -> comm);

            this -> arg.resize(ARG_SIZE);
            fill_string(this -> arg);
        }

        void from_line(std::string &buffer) {
            const char *model = (buffer.find('\"') == std::string::npos) ? "%s %s" : "%s \"%[^\"]\"";

            std::sscanf(buffer.c_str(), model, this -> comm.data(), this -> arg.data());
        }

        void format_from_buffer(std::string buffer) {
            this -> clean_fields();
            this -> from_line(buffer);
            this -> resize_fields();
        }

    private:
};

void send_file(SOCKET socket, const std::string& file_name) {
    char flag;
    std::string buffer(BUFFER_SIZE, '\0');
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);

    if (!file)
        return perror("File not found");

    int32_t file_size_network = htonl(file.tellg());
    file.seekg(0, std::ios::beg);

    send(socket, &file_size_network, sizeof(file_size_network), 0);

    while(file) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytes_read = file.gcount();

        if (bytes_read > 0) {
            int bytes_sent = send(socket, buffer.data(), static_cast<int>(bytes_read), 0);

            if (bytes_sent == -1)
                return perror("Sending data error");

            recv(socket, &flag, sizeof(flag), 0);

            if (flag != RECEIVED)
                return perror("Confirmation not received");
        }
    }

    file.close();
}

void receive_file(SOCKET socket, const std::string& file_name) {
    int ret, total_received = 0, bytes_to_receive;
    int32_t file_size_network;
    std::string buffer(BUFFER_SIZE, '\0');

    if((ret = recv(socket, reinterpret_cast<char*>(&file_size_network), sizeof(file_size_network), 0)) <= 0) 
        return perror("Failed to receive file size");

    int32_t file_size = ntohl(file_size_network);

    std::ofstream file(file_name, std::ios::binary);

    if(!file) 
        return perror("Could not open file");

    while(total_received < file_size) {
        bytes_to_receive = std::min(BUFFER_SIZE, file_size - total_received);

        if((ret = recv(socket, buffer.data(), bytes_to_receive, 0)) <= 0) {
            file.close();
            return perror("Failed to receive file data");
        }

        file.write(buffer.data(), ret);
        total_received += ret;

        send(socket, &RECEIVED, sizeof(RECEIVED), 0);
    }

    file.close();
}

void send_entry(SOCKET socket, const std::string& entry_path, const std::string& base_path) {
    std::string relative_path = fs::relative(entry_path, base_path).string();
    uint32_t path_length_network = htonl(relative_path.size());

    if(fs::is_directory(entry_path)) {
        send(socket, &FOLDER_FLAG, sizeof(FOLDER_FLAG), 0);
        send(socket, &path_length_network, sizeof(path_length_network), 0);
        send(socket, relative_path.data(), relative_path.size(), 0);

        for (const auto& entry : fs::directory_iterator(entry_path))
            send_entry(socket, entry.path().string(), base_path);

    } else if(fs::is_regular_file(entry_path)) {
        send(socket, &FILE_FLAG, sizeof(FILE_FLAG), 0);
        send(socket, &path_length_network, sizeof(path_length_network), 0);
        send(socket, relative_path.data(), relative_path.size(), 0);

        send_file(socket, entry_path);
    }
}

void send_files(SOCKET socket, const std::string& dir_path, const comm_line& command) {
    std::string origin_path = (command.arg == "*") ? dir_path : dir_path + "/" + command.arg;

    if(fs::exists(origin_path)) 
        send_entry(socket, origin_path, dir_path);
    else
        perror("Path does not exist");


    send(socket, &END, sizeof(END), 0);
}

void receive_files(SOCKET socket, const std::string& dir_path) {
    int ret;
    char type;

    while(ret = recv(socket, &type, sizeof(type), 0)) {
        if (type == END)
            break;

        uint32_t path_length_network;
    
        if((ret = recv(socket, reinterpret_cast<char*>(&path_length_network), sizeof(path_length_network), 0)) < 1) 
            return perror("Failed to receive path length");

        uint32_t path_length = ntohl(path_length_network);
        std::string relative_path(path_length, '\0');

        if((ret = recv(socket, &relative_path[0], path_length, 0)) <= 0) 
            return perror("Failed to receive path");

        std::string full_path = dir_path + "/" + relative_path;

        if(type == FOLDER_FLAG)
            fs::create_directories(full_path);
        else if (type == FILE_FLAG)
            receive_file(socket, full_path);
        else 
            perror("Unknown type received");
    }
}

#endif