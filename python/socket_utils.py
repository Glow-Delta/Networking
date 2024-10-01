import socket
import select
import logging

BUFFER_SIZE = 1024
MAX_CONNECTIONS = 5

def create_server_socket(port):
    """Create and return a server socket."""
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_fd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    return server_fd

def setup_server(server_fd, port):
    """Bind and listen on the server socket."""
    server_fd.bind(('0.0.0.0', port))
    server_fd.listen(MAX_CONNECTIONS)
    logging.info(f"Server listening on port {port}")

def handle_connections(server_fd, connections):
    """Handle incoming connections and data."""
    read_sockets, _, _ = select.select([server_fd] + connections, [], [])
    
    for sock in read_sockets:
        if sock == server_fd:
            # Accept new connection
            client_socket, address = server_fd.accept()
            connections.append(client_socket)
            logging.info(f"New connection from {address}")
        else:
            # Handle existing connection
            try:
                data = sock.recv(BUFFER_SIZE)
                if data:
                    message = data.decode('utf-8')
                    logging.info(f"Message from client: {message}")

                    if "404." in message:
                        response = "ID=" + str(connections.index(sock))
                        sock.sendall(response.encode('utf-8'))
                else:
                    # Client disconnected
                    logging.info("Client disconnected")
                    connections.remove(sock)
                    sock.close()
            except Exception as e:
                logging.error(f"Error while handling client: {e}")
                connections.remove(sock)
                sock.close()
