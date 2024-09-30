# Redes

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
| `show 'file_name'`    | Displays the contents of a file on the server machine. |
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
        <img src="data:image/jpeg;base64,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" width="100px;"  style="border-radius: 50%;"><br>
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

## License

This project is licensed under the **GNU General Public License v3.0**. You are free to redistribute it and/or modify it under the terms of the GPL, ensuring that the same rights apply to any derivative works. See [GNU General Public License v3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) for more information.

