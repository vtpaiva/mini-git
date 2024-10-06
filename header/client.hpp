#ifndef CLIENT_H
#define CLIENT_H

#include "header.hpp"

// Function to print the output from a command executed by the server.
void comm_output(SOCKET socket);

// Send a certain input to the server from the stdin.
void get_client_input(const char* input, const SOCKET &socket, char buffer[], const int input_size = BUFFER_SIZE);

// Function to execute the remove file/directory/repository command.
void remove_comm(SOCKET socket);

// Print the client's current repository.
void show_repo(SOCKET socket);

// Function to handle the client's command.
static inline void handle_command(SOCKET socket, command_line command);

// Function to handle the client's command sending to the server.
void handle_connection(const SOCKET &socket, std::string &buffer);

#endif