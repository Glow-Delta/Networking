import argparse
from pythonosc import dispatcher
from pythonosc import osc_server

max_volume_per_track = [0.2,0.2]

# Function to handle incoming OSC messages for specific track volume
def handle_track_volume(address, volume):
    # Check if the volume is a float type
    if isinstance(volume, float):
        track_number = address.split("/")[2]  # Extract the track number from the address
        if volume > 0:
            print(f"Track {track_number} volume: {volume:.2f}")
            max_volume_per_track[(int(track_number)-1)] = round(volume, 5)
            print (max_volume_per_track)

# Function to handle incoming OSC messages for the selected track volume
def handle_selected_track_volume(address, volume):
    # Check if the volume is a float type
    if isinstance(volume, float):
        print(f"Selected track volume: {volume:.2f}")

def start_server(ip, port):
    # Set up OSC server to listen for messages
    disp = dispatcher.Dispatcher()
    disp.map("/track/*/volume", handle_track_volume)  # For specific track volumes
    disp.map("/track/volume", handle_selected_track_volume)  # For the selected track volume

    server = osc_server.BlockingOSCUDPServer((ip, port), disp)
    print(f"Starting OSC server on {ip}:{port}...")
    server.serve_forever()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", default="127.0.0.1",
                        help="The IP of the OSC server (your Python script)")
    parser.add_argument("--port", type=int, default=9000,
                        help="The port the OSC server is listening on")
    args = parser.parse_args()

    # Start the OSC server to listen for messages
    start_server(args.ip, args.port)
