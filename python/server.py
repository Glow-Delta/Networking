import logging
from socket_utils import create_server_socket, setup_server, handle_connections

PORT = 8080

def main():
    logging.basicConfig(level=logging.INFO)
    
    # Create and setup server
    server_fd = create_server_socket(PORT)
    setup_server(server_fd, PORT)

    connections = []

    logging.info("Starting the server...")

    # Main loop to handle connections
    try:
        while True:
            handle_connections(server_fd, connections)
    except KeyboardInterrupt:
        logging.info("Shutting down the server.")
    finally:
        server_fd.close()

if __name__ == "__main__":
    main()
    