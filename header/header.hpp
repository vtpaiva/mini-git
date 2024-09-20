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

#ifndef CONTS_H
#define CONTS_H

constexpr int PORT = 8080;
constexpr int MAX_BACKLOG = 32;
constexpr int BUFFER_SIZE = 1024;
constexpr int NAME_SIZE = 16;
constexpr int PASSWORD_SIZE = 32;
constexpr int COMMAND_SIZE = 8;
constexpr int FILE_NAME_SIZE = 32;
constexpr int CLIENT_SIZE = sizeof(char) + NAME_SIZE + PASSWORD_SIZE;

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

#endif

typedef int SOCKET;

#define fill_string(st) (std::fill(st.begin(), st.end(), '\0'))
#define resize_erase(st, size) (st.resize(size), fill_string(st))
#define resize_till_null(st) (st.resize(st.find('\0')))
 
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

class client {
    public:

        char status;
        std::string name, password;

        client(SOCKET &client_socket) : name(std::string(NAME_SIZE,0)), password(PASSWORD_SIZE, 0), status(INVALID){
            int received_bytes;

            received_bytes = recv(client_socket, &name[0], NAME_SIZE, 0) - 1;
            this -> name.resize(received_bytes);

            received_bytes = recv(client_socket, &password[0], PASSWORD_SIZE, 0) - 1;
            this -> password.resize(received_bytes);

            status = VALID;
        }

        void copy_data_to_buffer(char buffer[]) {
            sprintf(buffer, "Name: %s\nPassword: %s\n", this -> name.c_str(), 
                                                        this -> password.c_str());
        }
    private:
};

class comm_line {
    public:

        std::string comm, file;

        comm_line() : comm(COMMAND_SIZE, '\0'), file(FILE_NAME_SIZE, '\0') {}

        comm_line(std::string command, std::string file_name) : comm(command), file(file_name) {}

        void resize_fields() {
            resize_till_null(this -> comm), resize_till_null(this -> file);
        }

        void clean_fields() {
            this -> comm.resize(COMMAND_SIZE);
            fill_string(this -> comm);

            this -> file.resize(FILE_NAME_SIZE);
            fill_string(this -> file);
        }

        std::string to_comm() {
            return std::string(this -> comm + " " + this -> file);
        }

    private:
};