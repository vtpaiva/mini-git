# mini-git

## Description

In order to explore tools and develop programming skills in computer networks with socket communication, a miniature version of Git was developed, emulating its functionalities in a considerably more superficial way. The program allows a user, the client, to communicate with a server on another machine via a socket, sending commands to receive and transmit files and directories and manage repositories. The clients, in addition to the application's own commands, have access to console commands through the terminal.

The tool makes it possible to have multiple connected clients, supported by a threading system, allowing clients to access repositories and files created by other users, with the possibility of editing and sharing.


## Prerequisites for Running the Application

This application is developed using C++ and utilizes the sockets library for communication. Please ensure that you meet the following prerequisites before attempting to run the program.

### System Requirements

- **Operating System**: The application is designed to run on any system that supports `<sys/socket.h>`. This includes most **Linux**, **macOS**, and other Unix-like systems.

### Software Requirements

- **C++ Compiler**: You will need a C++ compiler that supports C++11 or later. We recommend using **GCC** (GNU Compiler Collection), which can be installed on most Linux distributions.
  
  ```bash
  sudo apt-get install g++
  ```

## How it works

First, compile the using the follow command:

  ```bash
  make
  ```

This command will also create `local` and `repos` directories.

Then, run ```./server``` and ```./client``` in separate terminals.
After, insert the user's name and, once logged in, execute the table commands or use the terminal default commands on the client side to interact with the **mini-git** environment.

When a client connects to the server using a specified name, they are redirected to their `main` repository. The client can then use various commands through the terminal to navigate and interact with the repository environment.

The user will have a `local` folder that will be used as a local directory to be referenced in push/pull operations and terminal commands. This folder will simulate the local machine and the reference directory on the server machine for the client will be the current repository folder.

The server will have a `repos` folder that will be used as your machine. All repositories will be stored in this directory.

## Command Table

| Command             | Description |
|-------------------|-------------|
| `create 'file_name'`  | Creates a file in the repository on the server machine. |
| `delete 'name'`       | Deletes a file, directory, or repository on the server machine. |
| `push 'name'`         | Simulates Git's `push` functionality by sending files or directories to the server.<br>If the argument is `*`, the entire local directory is sent by the client. |
| `pull 'name'`         | Simulates Git's `pull` functionality by requesting files or directories from the server.<br>If the argument is `*`, the entire current repository directory is sent by the server. |
| `files 'folder'`      | Displays all files within a specified folder on the server machine.<br>If the argument is empty, all contents of the current repository are shown. |
| `show 'file_name'`    | Displays the contents of a file from the server machine. |
| `repo 'repo_name'`    | Switches to another repository or creates a new one if `repo_name` does not exist.<br>If the argument is empty, displays the name of the current repository. |
| `repos`               | Lists all of the client's repositories. |
| `xrepo 'user_name/repo_name'`   | Switches the current repository to that of another client. |
| `fork 'user_name/repo_name`      | Forks another user's repository, creating a repository on the user's folder named "fork-repo_name". |
| `exit`      | Disconnects client from the server (on client's terminal) or shut the server down (on server's terminal). |

## License

This project is licensed under the **GNU General Public License v3.0**. You are free to redistribute it and/or modify it under the terms of the GPL, ensuring that the same rights apply to any derivative works. See [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) for more information.

