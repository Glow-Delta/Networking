#include "socket_utils.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

// Function to handle multiple connections and receive data
void handle_connections(int server_fd, int new_socket[], struct sockaddr_in &address, int max_connections) {
    fd_set readfds;
    int max_sd, sd, activity, addrlen = sizeof(address), valread;
    char buffer[BUFFER_SIZE];

    // Clear the socket set
    FD_ZERO(&readfds);

    // Add the server socket to the set
    FD_SET(server_fd, &readfds);
    max_sd = server_fd;

    // Add child sockets to the set
    for (int i = 0; i < max_connections; i++) {
        sd = new_socket[i];
        if (sd > 0) {
            FD_SET(sd, &readfds);
        }
        if (sd > max_sd) {
            max_sd = sd;
        }
    }

    // Wait for an activity on one of the sockets
    activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

    if ((activity < 0) && (errno != EINTR)) {
        std::cerr << "Select error" << std::endl;
    }

    // If there's an incoming connection
    if (FD_ISSET(server_fd, &readfds)) {
        int new_socket_desc;
        if ((new_socket_desc = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "New connection, socket fd is " << new_socket_desc << ", IP is : "
                  << inet_ntoa(address.sin_addr) << ", Port: " << ntohs(address.sin_port) << std::endl;

        // Add new socket to the array of sockets
        for (int i = 0; i < max_connections; i++) {
            if (new_socket[i] == 0) {
                new_socket[i] = new_socket_desc;
                std::cout << "Adding to list of sockets as " << i << std::endl;
                break;
            }
        }
    }

    // Handle incoming data from clients
    for (int i = 0; i < max_connections; i++) {
        sd = new_socket[i];
        if (FD_ISSET(sd, &readfds)) {
            // Check if it was for closing
            if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                // Get details of the disconnected client
                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                std::cout << "Host disconnected, IP " << inet_ntoa(address.sin_addr)
                          << ", Port " << ntohs(address.sin_port) << std::endl;

                // Close the socket and mark as 0 in the array
                close(sd);
                new_socket[i] = 0;
            } else {
                buffer[valread] = '\0';
                std::cout << "Message from Arduino [" << i << "]: " << buffer << std::endl;

                // Check if buffer contains "404."
                if (strstr(buffer, "404.") != nullptr) {
                    std::cout << "404.* detected, sending back value of i: " << i << std::endl;
                    
                    // Convert integer `i` to string for sending
                    std::string response = std::to_string(i);

                    // Send the value of `i` back over the client socket `sd`
                    send(sd, response.c_str(), response.length(), 0);
                }
            }
        }
    }
}
