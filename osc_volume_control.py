from pythonosc import udp_client

def set_reaper_track_volume(client: udp_client.SimpleUDPClient, track_number: int, volume: float):
    """
    Sets the volume of a specified track in REAPER via OSC.
    
    Parameters:
    client (udp_client.SimpleUDPClient): The OSC client connected to REAPER.
    track_number (int): The track number in REAPER to control (1-based index).
    volume (float): The volume level, typically between 0.0 (mute) and 1.0 (max volume).
    """
    # Ensure volume stays within typical bounds for REAPER
    volume = max(0.0, min(volume, 1.0))
    
    # REAPER OSC address pattern for track volume
    osc_address = f"/track/{track_number}/volume"
    
    # Send the OSC message
    client.send_message(osc_address, volume)

def fade_in(track_id, original_volume, duration, client):
    set_reaper_track_volume(client, track_id, 0)
    mute_track(client, track_id, False)
    muted[track_id-1] = 0
    steps = 100
    for i in range(steps):
        volume = (original_volume / steps) * i
        set_reaper_track_volume(client, track_id, volume)
        time.sleep(duration / steps)

import time

def mute_track(client: udp_client.SimpleUDPClient, track_number: int, mute: bool):
    """
    Mutes or unmutes a specified track in REAPER via OSC.
    
    Parameters:
    - client (udp_client.SimpleUDPClient): The OSC client connected to REAPER.
    - track_number (int): The track number in REAPER to control (1-based index).
    - mute (bool): True to mute the track, False to unmute it.
    """
    osc_address = f"/track/{track_number}/mute"
    client.send_message(osc_address, int(mute))
    muted[track_number - 1] = int(mute)
    if track_number in active_tracks:
        if mute:
            active_tracks.remove(track_number)
    else:
        active_tracks.append(track_number)

# Initialize mute tracking for, say, 10 tracks
num_tracks = 17
muted = [1] * num_tracks  # 0 means unmuted, 1 means muted
active_tracks = []

import time

def cross_fade(tracks_in, tracks_out, duration, client, track_in_vols, track_out_vols):
    # Ensure inputs are lists of the same length
    assert len(tracks_in) == len(track_in_vols), "tracks_in and track_in_vols must be of the same length"
    assert len(tracks_out) == len(track_out_vols), "tracks_out and track_out_vols must be of the same length"
    
    # Initialize volumes: set incoming tracks to volume 0 and unmute them, set outgoing to original volume
    for track, max_vol in zip(tracks_in, track_in_vols):
        set_reaper_track_volume(client, track, 0)
        mute_track(client, track, False)
        muted[track - 1] = 0  # Track is unmuted

    for track, max_vol in zip(tracks_out, track_out_vols):
        set_reaper_track_volume(client, track, max_vol)
        mute_track(client, track, False)  # Ensure outgoing tracks are unmuted initially

    steps = 100
    for i in range(steps):
        # Fade in each track in tracks_in
        for track, max_vol in zip(tracks_in, track_in_vols):
            volume_in = (max_vol / steps) * i
            set_reaper_track_volume(client, track, volume_in)
        
        # Fade out each track in tracks_out
        for track, max_vol in zip(tracks_out, track_out_vols):
            volume_out = max_vol * (1 - (i / steps))
            set_reaper_track_volume(client, track, volume_out)
        
        time.sleep(duration / steps)

    # Finalize: mute and reset outgoing tracks, set incoming tracks to their target volume
    for track, max_vol in zip(tracks_out, track_out_vols):
        set_reaper_track_volume(client, track, 0)
        mute_track(client, track, True)
        muted[track - 1] = 1  # Track is muted

    for track, max_vol in zip(tracks_in, track_in_vols):
        set_reaper_track_volume(client, track, max_vol)  # Ensure tracks_in reach target volume

def should_fade(track_number: int, fade_type: str):
    """
    Determines if a fade-in or fade-out is needed for the specified track.
    
    Parameters:
    - track_number (int): The track number to check.
    - fade_type (str): Type of fade, either "in" for fade-in or "out" for fade-out.
    
    Returns:
    - bool: True if the specified fade is needed, False otherwise.
    """
    if fade_type == "in":
        # Fade-in is needed if the track is not in active_tracks
        return track_number not in active_tracks
    elif fade_type == "out":
        # Fade-out is needed if the track is in active_tracks
        return track_number in active_tracks
    else:
        raise ValueError("Invalid fade_type. Use 'in' for fade-in or 'out' for fade-out.")

def pillar_activation(track, activation, volumes, client):
    unmuted = None
    section = 1
    if track > 1:
        section = ((track - 1) * 3) + 1

    for i in range(3):
        if muted[(section - 1) + i] == 0:
            unmuted = (section - 1) + i
            print (f"unmuted track: {unmuted}")

    # Determine the appropriate section based on activation value
    if 0 <= activation <= 5:
        section = section
    elif 6 <= activation <= 10:
        section = section + 1
    elif 11 <= activation <= 16:
        section = section + 2

    print(f"Section: {section}")

    # Perform cross-fade if the conditions are met
    if should_fade(section, "in"):
        if unmuted is not None and should_fade((unmuted+1), "out"):
            # Wrap section and unmuted in lists and pass corresponding volume lists
            print ("doing cross fade")
            cross_fade(
                tracks_in=[section],
                tracks_out=[(unmuted+1)],
                duration=2,
                client=client,
                track_in_vols=[volumes[section - 1]],
                track_out_vols=[volumes[unmuted]]
            )
        else:
            print ("fade in")
            # Fade in section if cross-fade is not needed
            fade_in(section, volumes[section - 1], 2, client)

prev = None
def background_music_check(levels, volumes, client):
    global prev
    total = sum(levels)
    background = 0 if total < 40 else 1

    # Check if a change in background music state is needed
    if background != prev:
        if background == 1:
            # Check if we need to fade in track 17
            if should_fade(17, "in"):
                cross_fade([17], [16], 2, client, [volumes[16]], [volumes[15]])
        else:
            # Check if we need to fade out track 16
            if should_fade(16, "out"):
                cross_fade([16], [17], 2, client, [volumes[15]], [volumes[16]])

        # Update the previous state
        prev = background
