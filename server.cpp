#include "server.h"
#include "socket_utils.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CONNECTIONS 5

int main() {
    int server_fd;
    struct sockaddr_in address;
    int new_socket[MAX_CONNECTIONS] = {0};

    // Create the server socket
    server_fd = create_server_socket(PORT);

    // Set the server to listen for incoming connections
    setup_server(server_fd, address, PORT);

    // Main loop to handle connections
    while (true) {
        handle_connections(server_fd, new_socket, address, MAX_CONNECTIONS);
    }

    // Close the server socket
    close(server_fd);
    return 0;
}
