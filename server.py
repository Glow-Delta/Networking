import socket
import select
import logging
import re
import osc_client

def start_server(port):
    #Define socket type
    server_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_fd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    #Bind socket to ip/port
    server_fd.bind(('192.168.0.1', port))
    server_fd.listen(5) #set to listen and max amount of clients
    return server_fd

close_connection = "Error=-1"
connections = []

def loop_handle_conenction(server_fd, osc_connection):
    while True:
        handle_connection(server_fd,osc_connection)

def handle_connection(server_fd, osc_connection):
    #read incoming data
    read_socket, _, _ = select.select([server_fd] + connections, [], [])

    for sock in read_socket:
        if sock == server_fd:
            #accept connection
            client_sock, address = server_fd.accept()
            connections.append(client_sock)
            print (f"New Connection from {address}")
        else:
            #handles existing clients
            try:
                data = sock.recv(1024) #recieve buffer size
                if data:
                    message = data.decode('utf-8')
                    print (f"Recieved Message: {message}")
                    
                    if "404" in message:
                        response = "ID="+str(connections.index(sock)+1)
                        print (response)
                        sock.sendall(response.encode('utf-8'))
                    else:
                        numbers = list(map(int, re.findall(r'\d+', message)))
                        print (f"For osc:\nID={numbers[0]}\nActivation_level={numbers[1]}\n")
                        osc_client.switch_tracks(numbers[0], numbers[1], osc_connection)
                else:
                    #client disconnected
                    print ("Disconnected")
                    sock.sendall(close_connection.encode('utf-8'))
                    connections.remove(sock)
                    sock.close()
            except Exception as e:
                logging.error(f"Error while handling client: {e}")
                connections.remove(sock)
                sock.close()
                print (f"Connections: {len(connections)}")