#include <Arduino.h>
 
#include <Ethernet.h>      // Standard Ethernet library for W5500
#include <EthernetUdp.h>    // UDP support for Ethernet library
 
// Enter a MAC address and IP address for your controller below
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);           // Local IP address of the Teensy
IPAddress remoteIp(192, 168, 1, 100);     // Destination IP address for sending
unsigned int remotePort = 50000;          // Destination port to send to
unsigned int localPort = 8888;            // Local port for Teensy to use
 
// An EthernetUDP instance to let us send packets over UDP
EthernetUDP Udp;
 
void setup() {
  // Initialize Ethernet with the correct CS pin for the W5500
  Ethernet.init(10);  // Set CS pin as needed for W5500
  Ethernet.begin(mac, ip);
 
  // Open serial communications
  Serial.begin(9600);
  Serial2.begin(9600);

  Serial.println("Checking Ethernet");
 
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found. Cannot run without hardware.");
    while (true) {
      delay(1); // Do nothing if there's no Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  Serial.println("Done Checking");

  Serial.println("Starting UDP");
  // Start UDP
  Udp.begin(localPort);
  Serial.println("Setup complete");
}
 
void loop() {
  if (Serial2.available()) {  
    // Read the incoming message
    String message = Serial2.readStringUntil('\n');
    Serial.println("Received: " + message); // Print the received message for debugging
 
    const char* charArray = message.c_str();
 
    Udp.beginPacket(remoteIp, remotePort);
    Udp.write(charArray);
    Udp.endPacket();
 
    Serial.println("Message sent!");
  }
  else{
    Serial.println("Serial2 not avalible");
    delay(500);
  }
}
