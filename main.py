from threading import Thread
import osc_client
import server

if __name__ == "__main__":
    osc_client_connection = osc_client.start_client()
    server_fd = server.start_server(50000)

    server_thread = Thread(target=server.loop_handle_conenction, args=(server_fd, osc_client_connection))
    server_thread.start()
    while True:
        print ("Enter Pillar")
        pillar = int(input())
        print ("Enter amount of sensors (0-16)")
        sensors = int(input())

        osc_client.switch_tracks(pillar, sensors, osc_client_connection)
        print ("\n\n\n")