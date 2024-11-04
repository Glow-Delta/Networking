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
        track_id, activation_level = message.split(":")
        track_id = int(track_id.split("=")[1])
        activation_level = int(activation_level.split("=")[1])
        pillar_activation[int(track_id)-1] = int(activation_level)
        osc_volume_control.background_music_check(pillar_activation,volumes, client)
        osc_volume_control.pillar_activation(track_id,activation_level,volumes,client)
        
        
if __name__ == "__main__":
    udpserver = start_Server("192.168.1.100", 50000)
    reaper_client = osc_client.start_osc_client("127.0.0.1", 9000)
    listen(udpserver, volumes, reaper_client)
    