#include <Ethernet.h>
#include <EthernetUdp.h>

// MAC address for your W5500 (needs to be unique in your network)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP address of your Arduino (adjust as needed)
IPAddress ip(192, 168, 0, 2);

// Server IP and port to connect to
IPAddress server(192, 168, 0, 1); // IP of the server (192.168.0.1)
unsigned int port = 50000;        // Port to send data to (50000)

// Create a UDP object
EthernetUDP Udp;

String IncomingData = "";

void setup() {
  Serial.begin(9600);

  // Start Ethernet
  Ethernet.init(8);
  Ethernet.begin(mac, ip);

  // Allow the Ethernet shield some time to initialize
  delay(1000);
  Serial.println("Starting UDP connection...");

  // Start the UDP connection on a local port
  Udp.begin(port);
  Serial.println("UDP connection started.");
  sendDataToServer("Test connection!");
}

void loop() {
  if (Serial.available() > 0) {
    IncomingData = Serial.readString();

    Serial.println(IncomingData);
    if(IncomingData.length() > 0){
      sendDataToServer(IncomingData);
    }
  }
}

void sendDataToServer(const String &packetData) {
  Udp.beginPacket(server, port);
  Udp.write(packetData.c_str());
  Udp.endPacket();
}