import queue
import socket
import threading
import osc_volume_control
import osc_client

def bind_server(ip, port):
    server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server.bind((ip, port))
    return server
    
def start_Server(ip, port):
    server = bind_server(ip, port)
    return server
    
pillar_activation = [0]*5
volumes = [0.5] * 17
def listen(server, volumes, client):
    while True:
        data, addr = server.recvfrom(1024)
        message = data.decode('utf-8')
        # Extract `track_id` and `activation_level` from the message
        parts = message.split(":")
        track_id = int(parts[0].split("=")[1])  # Extracts "0" and converts to int
        activation_level = int(parts[1].split("=")[1])  # Extracts "12" and converts to int
        # Update the pillar_activation array and call functions
        pillar_activation[track_id] = activation_level
        osc_volume_control.background_music_check(pillar_activation, volumes, client)
        osc_volume_control.pillar_activation(track_id, activation_level, volumes, client)

        
        
if __name__ == "__main__":
    udpserver = start_Server("192.168.0.1", 50000)
    reaper_client = osc_client.start_osc_client("127.0.0.1", 9000)
    listen(udpserver, volumes, reaper_client)
    