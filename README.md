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

## Getting started

When a client connects to the server using a specified name, they are redirected to their `main` repository. The client can then use various commands through the terminal to navigate and interact with the repository environment.

## Command Table

| Command           | Description |
|-------------------|-------------|
| `create 'file_name'`  | Creates a file in the repository on the server machine. |
| `delete 'name'`       | Deletes a file, directory, or repository on the server machine. |
| `push 'name'`         | Simulates Git's `push` functionality by sending files or directories to the server.<br>If the argument is `*`, the entire local directory is sent by the client. |
| `pull 'name'`         | Simulates Git's `pull` functionality by requesting files or directories from the server.<br>If the argument is `*`, the entire current repository directory is sent by the server. |
| `files 'folder'`      | Displays all files within a specified folder on the server machine.<br>If the argument is empty, all contents of the current repository are shown. |
| `show 'file_name'`    | Displays the contents of a file from the server machine. |
| `repo 'repo_name'`    | Switches to another repository or creates a new one if `repo_name` does not exist.<br>If the argument is empty, displays the name of the current repository. |
| `repos`               | Lists all of the client's repositories. |
| `xrepo 'repo_name'`   | Switches the current repository to that of another client. |
| `comm "command"`      | Executes a specified command on the server machine. (Use with caution!) |

## Authors

<table>
  <tr>
    <td align="center" style="border: none;">
      <a href="" title="">
        <img src="https://avatars.githubusercontent.com/u/144858995?v=4" width="100px;"  style="border-radius: 50%;"><br>
        <sub>
          <b>Felipe Aparecido<br></b>
		  <span style="color:grey;">11954502</span>
        </sub>
      </a>
    </td>
    <td align="center" style="border: none;">
      <a href="https://github.com/guaraujoc" title="github">
        <img src="https://avatars.githubusercontent.com/u/130992375?s=400&u=168448c320a3ad61a9737a30880fa942249baedc&v=4" width="100px;"  style="border-radius: 50%;"><br>
        <sub>
          <b>Gustavo Araújo<br></b>
		  <span style="color:grey;">13735630</span>
        </sub>
      </a>
    </td>

 <td align="center" style="border: none;">
      <a href="" title="">
        <img src="https://avatars.githubusercontent.com/u/113041643?s=96&v=4" width="100px;"  style="border-radius: 50%;"><br>
        <sub>
          <b>Rafael Learth<br></b>
		  <span style="color:grey;">13676367</span>
        </sub>
      </a>
    </td>

 <td align="center" style="border: none;">
      <a href="" title="">
        <img src="https://avatars.githubusercontent.com/u/141160595?v=4" width="100px;"  style="border-radius: 50%;"><br>
        <sub>
          <b>Viktor Sergio<br></b>
		  <span style="color:grey;">11800570</span>
        </sub>
      </a>
    </td>

 <td align="center" style="border: none;">
      <a href="" title="">
        <img src="https://avatars.githubusercontent.com/u/105892477?s=96&v=4" width="100px;"  style="border-radius: 50%;"><br>
        <sub>
          <b>Vitor Augusto<br></b>
		  <span style="color:grey;">13732303</span>
        </sub>
      </a>
    </td>
  </tr>
</table>

## License

This project is licensed under the **GNU General Public License v3.0**. You are free to redistribute it and/or modify it under the terms of the GPL, ensuring that the same rights apply to any derivative works. See [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) for more information.

