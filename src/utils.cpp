#include "../header/header.hpp"

// Sends the file's bytes via socket.
void send_file(SOCKET socket, const std::string& file_name) {
    char flag;
    std::string buffer(BUFFER_SIZE, '\0');
    std::ifstream file(file_name, std::ios::binary | std::ios::ate);

    if (!file)
        return perror("File not found");

    // Gets the file size.
    int32_t file_size_network = htonl(file.tellg());
    file.seekg(0, std::ios::beg);

    send(socket, &file_size_network, sizeof(file_size_network), 0);

    // While the file is not over, keep sending the data.
    while(file) {
        file.read(buffer.data(), buffer.size());
        std::streamsize bytes_read = file.gcount();

        if (bytes_read > 0) {
            int bytes_sent = send(socket, buffer.data(), static_cast<int>(bytes_read), 0);

            if (bytes_sent == -1)
                return perror("Sending data error");

            recv(socket, &flag, sizeof(flag), 0);

            if (flag != RECEIVED)
                return perror("Confirmation not received");
        }
    }

    file.close();
}

// Receives the file's bytes via socket.
void receive_file(SOCKET socket, const std::string& file_name) {
    int ret, total_received = 0, bytes_to_receive;
    int32_t file_size_network;
    std::string buffer(BUFFER_SIZE, '\0');

    if((ret = recv(socket, reinterpret_cast<char*>(&file_size_network), sizeof(file_size_network), 0)) <= 0) 
        return perror("Failed to receive file size");

    int32_t file_size = ntohl(file_size_network);

    std::ofstream file(file_name, std::ios::binary);

    if(!file) 
        return perror("Could not open file");

    while(total_received < file_size) {
        bytes_to_receive = std::min(BUFFER_SIZE, file_size - total_received);

        if((ret = recv(socket, buffer.data(), bytes_to_receive, 0)) < 1) {
            file.close();
            return perror("Failed to receive file data");
        }

        file.write(buffer.data(), ret);
        total_received += ret;

        // Confirms the receiving.
        send(socket, &RECEIVED, sizeof(RECEIVED), 0);
    }

    file.close();
}

// Sends file's entrys via socket.
void send_entry(SOCKET socket, const std::string& entry_path, const std::string& base_path) {
    std::string relative_path = fs::relative(entry_path, base_path).string();
    uint32_t path_length_network = htonl(relative_path.size());

    // If it's a directory send the files inside recursively.
    if(fs::is_directory(entry_path)) {
        send(socket, &FOLDER_FLAG, sizeof(FOLDER_FLAG), 0);
        send(socket, &path_length_network, sizeof(path_length_network), 0);
        send(socket, relative_path.data(), relative_path.size(), 0);

        for (const auto& entry : fs::directory_iterator(entry_path))
            send_entry(socket, entry.path().string(), base_path);
    } 
    // If it's a file, send it's data.
    else if(fs::is_regular_file(entry_path)) {
        send(socket, &FILE_FLAG, sizeof(FILE_FLAG), 0);
        send(socket, &path_length_network, sizeof(path_length_network), 0);
        send(socket, relative_path.data(), relative_path.size(), 0);

        send_file(socket, entry_path);
    }
}

// Sends multiple files via socket.
void send_files(SOCKET socket, const std::string& dir_path, const comm_line& command) {
    std::string origin_path = (command.arg == "*") ? dir_path : dir_path + "/" + command.arg;

    if(fs::exists(origin_path)) 
        send_entry(socket, origin_path, dir_path);
    else
        perror("Path does not exist");


    // Ends the data tramission with flag.
    send(socket, &END, sizeof(END), 0);
}

// Receives multiple files via socket.
void receive_files(SOCKET socket, const std::string& dir_path) {
    int received_bytes;
    char type;

    // While theres files left, keeps receiving.
    while(received_bytes = recv(socket, &type, sizeof(type), 0)) {
        if (type == END)
            break;

        uint32_t path_length_network;
    
        if((received_bytes = recv(socket, reinterpret_cast<char*>(&path_length_network), sizeof(path_length_network), 0)) < 1) 
            return perror("Failed to receive path length");

        uint32_t path_length = ntohl(path_length_network);
        std::string relative_path(path_length, '\0');

        if((received_bytes = recv(socket, &relative_path[0], path_length, 0)) <= 0) 
            return perror("Failed to receive path");

        std::string full_path = dir_path + "/" + relative_path;

        if(type == FOLDER_FLAG)
            fs::create_directories(full_path);
        else if (type == FILE_FLAG)
            receive_file(socket, full_path);
        else 
            perror("Unknown type received");
    }
}

// Fill an entire string with null characters.
void fill_string(std::string &st) {
    std::fill(st.begin(), st.end(), '\0');
}

// Returns error message if value means error.
void exit_if_error(const int value, const std::string err) {
    if(value < 0) {
        std::cout << "Fail on " << err << "! (" << strerror(errno) << ")" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Resizes a string to the length of the fisrt occurrence of '\0'.
void resize_till_null(std::string &st) {
    auto pos = st.find('\0');

    if (pos != std::string::npos)
        st.resize(pos);
}