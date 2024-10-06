#ifndef HEADER_H
#define HEADER_H

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
const std::string LOCAL_DIR = "local/";
const std::string REPOS_DIR = "repos/";

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

// Fill an entire string with null characters.
void fill_string(std::string &st);

// Resizes a string to the length of the fisrt occurrence of '\0'.
void resize_till_null(std::string &st);

// Client class, representing a user.
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

// Command line class, representing a command inputed by the client.
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

// Returns error message if value means error.
void exit_if_error(const int value, const std::string err = "code");

// Sends the file's bytes via socket.
void send_file(SOCKET socket, const std::string& file_name);

// Receives the file's bytes via socket.
void receive_file(SOCKET socket, const std::string& file_name);

// Sends file's entrys via socket.
void send_entry(SOCKET socket, const std::string& entry_path, const std::string& base_path);

// Sends multiple files via socket.
void send_files(SOCKET socket, const std::string& dir_path, const comm_line& command);

// Receives multiple files via socket.
void receive_files(SOCKET socket, const std::string& dir_path);

#endif