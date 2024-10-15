from pythonosc import udp_client

def start_client():
    client = udp_client.SimpleUDPClient("127.0.0.1", 8000)
    return client
pillars_active = [1,0,0]
def switch_tracks(pillar, sensors, client):
    track_adjustment = 1
    if (pillar > 1):
        track_adjustment = ((pillar-1) * 3)+1
    client.send_message(f"/track/{track_adjustment}/volume", 0)
    client.send_message(f"/track/{track_adjustment+1}/volume", 0)
    client.send_message(f"/track/{track_adjustment+2}/volume", 0)
    pillars_active[pillar] = 0
    if (0 < sensors <= 5):
        client.send_message(f"/track/{track_adjustment}/volume", 0.5)
    elif (5 < sensors <= 10):
        client.send_message(f"/track/{track_adjustment+1}/volume", 0.5)
    elif (10 < sensors):
        client.send_message(f"/track/{track_adjustment+2}/volume", 0.5)
        pillars_active[pillar] = 1


    if all(i == pillars_active[0] for i in pillars_active):
        client.send_message(f"/track/8/volume", 0.5)
        client.send_message(f"/track/7/volume", 0.0)
    else:
        client.send_message(f"/track/8/volume", 0.0)
        client.send_message(f"/track/7/volume", 0.5)