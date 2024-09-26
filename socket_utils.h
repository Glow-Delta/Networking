#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>

// Function declarations
int create_server_socket(int port);
void setup_server(int server_fd, struct sockaddr_in &address, int port);
void handle_connections(int server_fd, int new_socket[], struct sockaddr_in &address, int max_connections);

#endif
