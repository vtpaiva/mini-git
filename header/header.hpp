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
constexpr int PASSWORD_SIZE = 32;
constexpr int COMMAND_SIZE = 8;
constexpr int ARG_SIZE = 64;
constexpr int CLIENT_SIZE = sizeof(char) + NAME_SIZE + PASSWORD_SIZE;

const std::string LOCAL_DIR = "local/";
const std::string REPOS_DIR = "repos/";
constexpr const char RECEIVED = '1';
constexpr const char NOT_RECEIVED = '\0';
constexpr const char VALID = '1';
constexpr const char INVALID = '0';
constexpr const char FILL_CHAR = '$';
constexpr const char* SOCKET_CREATION_ERROR = "Socket";
constexpr const char* BIND_ERROR = "Bind";
constexpr const char* LISTEN_ERROR = "Listen";
constexpr const char* ACCEPT_ERROR = "Accept";
constexpr const char* CONNECT_ERROR = "Connect";
constexpr const char* ADRESS_ERROR = "Adress";
constexpr const char* RECV_ERROR = "Message receiving";
constexpr const char* CURR_REPO_ERROR = "This is your current repository, tell somewhere to go:\n";
constexpr const char* OK_MESSAGE = "Sent!";

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

std::string filled_string(std::string &st) {
    std::string tmp = st;

    std::fill(tmp.begin() + tmp.find('\0'), tmp.end(), FILL_CHAR);

    return tmp;
}

inline void resize_till_null(std::string &st) {
    auto pos = st.find('\0');

    if (pos != std::string::npos)
        st.resize(pos);
}

void receive_file(SOCKET socket, const std::string& file_name) {
    int bytesReceived, file_size;
    std::ofstream file(file_name, std::ios::binary);

    if (!file) 
        return perror("Could not open file");

    read(socket, &file_size, sizeof(file_size));

    file_size = ntohl(file_size);

    std::string buffer(BUFFER_SIZE, '\0');

    for (int total = 0; total < file_size; total += bytesReceived) {
        bytesReceived = recv(socket, buffer.data(), BUFFER_SIZE, 0);
        send(socket, &RECEIVED, sizeof(RECEIVED), 0);

        file.write(buffer.data(), bytesReceived);
    }

    if (bytesReceived == -1) {
        send(socket, &NOT_RECEIVED, sizeof(NOT_RECEIVED), 0);

        perror("Erro ao receber dados");
    }


    file.close();
}

void send_file(SOCKET socket, const std::string& file_name) {
    char confirm = '1';
    int bytes_sent, file_size;
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);

    if (!file)
        return perror("File not found");

    file_size = htonl(file.tellg());
    file.seekg(0, std::ios::beg);

    send(socket, &file_size, sizeof(file_size), 0);

    std::string buffer(BUFFER_SIZE, '\0'); 

    while(file.read(buffer.data(), buffer.size())) {
        if ((bytes_sent = send(socket, buffer.data(), BUFFER_SIZE, 0)) == -1 || !confirm) 
            return perror("Erro ao enviar últimos dados");

        recv(socket, &confirm, sizeof(confirm), 0);
    }

    if(file.gcount() > 0) {
        if ((bytes_sent = send(socket, buffer.data(), file.gcount(), 0)) == -1 || !confirm) 
            return perror("Erro ao enviar últimos dados");

        recv(socket, &confirm, sizeof(confirm), 0);
    }

    file.close();
}

class client {
    public:

        std::string name, password;
        std::string curr_dir;

        client(SOCKET &client_socket) : name(std::string(NAME_SIZE,0)), password(PASSWORD_SIZE, 0), curr_dir(std::string(ARG_SIZE,0)){
            int received_bytes;

            received_bytes = recv(client_socket, name.data(), NAME_SIZE, 0) - 1;
            this -> name.resize(received_bytes);

            received_bytes = recv(client_socket, password.data(), PASSWORD_SIZE, 0) - 1;
            this -> password.resize(received_bytes);

            this -> curr_dir = REPOS_DIR + this -> name;
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

        void to_line(std::string &buffer) {
            std::snprintf(buffer.data(), BUFFER_SIZE, "%s %s\0", this->comm.c_str(), this->arg.c_str());

            buffer.resize(buffer.find('\0'));
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

void send_files(SOCKET socket, std::string dir_path, comm_line command) {
    char flag;   
    int number_files = 0;
    std::string curr_file, origin_path;

    origin_path = (command.arg == "*") ? dir_path : dir_path + "/" + command.arg;

    if(fs::is_directory(origin_path)) {
        for (const auto& entry : fs::directory_iterator(origin_path))
            number_files++;

        number_files = htonl(number_files);

        send(socket, &number_files, sizeof(number_files), 0);

        for (const auto& entry : fs::directory_iterator(origin_path)) {
            curr_file = entry.path().filename().string();

            send(socket, curr_file.data(), sizeof(curr_file.data()), 0);
            recv(socket, &flag, sizeof(flag), 0);

            if(flag)
                send_file(socket, origin_path + "/" + entry.path().filename().string());
        }
    } else {
        number_files = htonl(1);
        send(socket, &number_files, sizeof(int), 0);

        send(socket, command.arg.data(), size(command.arg), 0);
        recv(socket, &flag, sizeof(flag), 0);

        if(flag)
            send_file(socket, origin_path);
    }
}

void receive_files(SOCKET socket, std::string dir_path) {
    int number_files;
    std::string buffer(BUFFER_SIZE, '\0');

    read(socket, &number_files, sizeof(number_files));

    number_files = ntohl(number_files);

    for_each(number_files) {
        recv(socket, buffer.data(), BUFFER_SIZE, 0);
        send(socket, &RECEIVED, sizeof(char), 0);

        resize_till_null(buffer);

        receive_file(socket, dir_path + buffer);

        buffer.resize(BUFFER_SIZE);
        fill_string(buffer);
    }
}

#endif