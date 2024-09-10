#include <iostream>
#include <fstream>  
#include <filesystem>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <regex>

#ifndef CONTS_H
#define CONTS_H

#define each(n) for(int i = 0; i < n; i++)

constexpr int PORT = 8080;
constexpr int MAX_BACKLOG = 32;
constexpr int BUFFER_SIZE = 1024;
constexpr int NAME_SIZE = 32;
constexpr int EMAIL_SIZE = 64;
constexpr int PASSWORD_SIZE = 16;

constexpr const char* SOCKET_CREATION_ERROR = "Socket";
constexpr const char* BIND_ERROR = "Bind";
constexpr const char* LISTEN_ERROR = "Listen";
constexpr const char* ACCEPT_ERROR = "Accept";
constexpr const char* CONNECT_ERROR = "Connect";
constexpr const char* ADRESS_ERROR = "Adress";
constexpr const char* RECV_ERROR = "Message receiving";

#endif

typedef int SOCKET;

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

class client {
    public:

        std::string name, email, password;

        client(SOCKET &client_socket) : name(std::string(NAME_SIZE,0)), email(std::string(EMAIL_SIZE, 0)), password(PASSWORD_SIZE, 0){
            exit_if_error(recv(client_socket, &name[0], NAME_SIZE, 0), 
                          RECV_ERROR);
            remove_char(this -> name);

            exit_if_error(recv(client_socket, &email[0], EMAIL_SIZE, 0), 
                          RECV_ERROR);
            remove_char(this -> email);
                        
            exit_if_error(recv(client_socket, &password[0], PASSWORD_SIZE, 0), 
                          RECV_ERROR);
            remove_char(this -> password);
        }

        void copy_data_to_buffer(char buffer[]) {
            sprintf(buffer, "Name: %s\nEmail: %s\nPassword: %s\n", this -> name.c_str(), 
                                                                   this -> email.c_str(), 
                                                                   this -> password.c_str());
        }

    private:
};