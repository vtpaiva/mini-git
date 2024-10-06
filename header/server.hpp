#ifndef SERVER_H
#define SERVER_H

#include "header.hpp"

const std::string LOGIN_MESSAGE = "Login completed!";

// Return the client's current repository name.
std::string curr_repo(client client);

// Create a new file in the client's repository.
void add_file(comm_line command, client &client);

// Function to change the client's current repository by the command.
void change_repo(comm_line command, client &client);

// Function to change the client's current repository by the repos's name.
void change_repo(std::string repo_name, client &client);

// Function to change the client's current repository to another user's repository.
void change_to_external_repo(comm_line command, client &client);

// Function to execute the command to remove a file/directory/repository.
void remove_comm(SOCKET socket, comm_line command, client &client);

// Forks another user's repository.
void fork_repo(SOCKET socket, client &client, comm_line command);

// Function to execute a command and send the output to the client.
void execute_comm(SOCKET socket, client curr_client, std::string terminal_comm);

// Function to handle the commands from the client.
static void handle_command(comm_line command, client &curr_client, SOCKET socket);

// Function to handle the connection from the client.
int accept_client(SOCKET socket, std::string buffer);

// Function to handle a client connection to the server, creating a thread.
void handle_connect(SOCKET server_socket, SOCKET client_socket, std::string buffer, std::vector<std::thread> &threads);

// Function to handle exit command in the input.
void handle_input();

#endif