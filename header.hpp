#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef int SOCKET;

#ifndef CONTS_H
#define CONTS_H

constexpr int PORT = 8080;
constexpr int MAX_BACKLOG = 32;
constexpr int BUFFER_SIZE = 1024;

constexpr const char* SOCKET_CREATION_ERROR = "Socket";
constexpr const char* BIND_ERROR = "Bind";
constexpr const char* LISTEN_ERROR = "Listen";
constexpr const char* ACCEPT_ERROR = "Accept";
constexpr const char* CONNECT_ERROR = "Connect";
constexpr const char* ADRESS_ERROR = "Adress";
constexpr const char* DEFAULT_MESSAGE = "Default message!";

#endif

// Returns error message if value means error.
void exit_if_error(const int value, const char *err = "code") {
    if(value < 0) {
        std::cout << "Fail on " << err << "! (" << strerror(errno) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}