from pythonosc import udp_client

def start_osc_client(ip, port):
    client = udp_client.SimpleUDPClient(ip, port)
    return client