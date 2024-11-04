#import socket

#DPserver = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

#UDPserver.bind(("192.168.0.1", 50000))

#print ("Started udp server")

#hile (True):
    #Addresspair = UDPserver.recvfrom(1024)
    #message, addres = UDPserver.recvfrom(1024)

    #decoded_message = message.decode('utf-8')

    #print(f"bericht van {addres}: {decoded_message}")
    #message = Addresspair[0]

    #address = Addresspair[1]

    #print (f"Message from {address}: {message}")


import socket

HOST = '192.168.1.100'
PORT =  50000

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
    s.bind((HOST, PORT))
    print(f'listening on {HOST}:{PORT}')
    while True:
        data, addr= s.recvfrom(1024)
        try:
            print(f'recieved from {addr[0]}: {data.decode()}')
        except UnicodeDecodeError:
            print (f"recieved {addr[0]}: {data} (could not decode)")
    