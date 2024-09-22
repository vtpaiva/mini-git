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

constexpr int PORT = 8084;
constexpr int MAX_BACKLOG = 32;
constexpr int BUFFER_SIZE = 1024;
constexpr int NAME_SIZE = 16;
constexpr int PASSWORD_SIZE = 32;
constexpr int COMMAND_SIZE = 8;
constexpr int ARG_SIZE = 32;
constexpr int CLIENT_SIZE = sizeof(char) + NAME_SIZE + PASSWORD_SIZE;

const std::string END_TRANS = "cavalo";
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


typedef int SOCKET;

#define fill_string(st) (std::fill(st.begin(), st.end(), '\0'))
#define resize_erase(st, size) (st.resize(size), fill_string(st))
 
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

class client {
    public:

        char status;
        std::string name, password;

        client(SOCKET &client_socket) : name(std::string(NAME_SIZE,0)), password(PASSWORD_SIZE, 0), status(INVALID){
            int received_bytes;

            received_bytes = recv(client_socket, name.data(), NAME_SIZE, 0) - 1;
            this -> name.resize(received_bytes);

            received_bytes = recv(client_socket, password.data(), PASSWORD_SIZE, 0) - 1;
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

        void from_line(std::string &buffer, int argc = 2) {
            if(sscanf(buffer.c_str(), "%s %s", this -> comm.data(), this -> arg.data()) != argc) {
                std::cout << "Wrong number of arguments" << std::endl;
            }
        }

    private:
};
#endif