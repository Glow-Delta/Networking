#include "socket_utils.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>

// Create a socket for the server
int create_server_socket(int port) {
    int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    return server_fd;
}
