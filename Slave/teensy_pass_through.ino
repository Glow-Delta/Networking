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

// Function to check and send data over UDP from a specified Serial port
void checkAndSendSerialData(HardwareSerial& serialPort) {
  if (serialPort.available()) {
    // Read the incoming message from the specified serial port
    String receivedMessage = serialPort.readStringUntil('\n');
    Serial.print("Received from Serial: ");
    Serial.println(receivedMessage);

    // Convert the String to a char array for UDP sending
    const char* charArray = receivedMessage.c_str();

    // Send the message over UDP
    Udp.beginPacket(remoteIp, remotePort);
    Udp.write(charArray);
    Udp.endPacket();

    Serial.println("Message sent!");
  }
}

void setup() {
  // Initialize Ethernet with the correct CS pin for the W5500
  Ethernet.init(10);  // Set CS pin as needed for W5500
  Ethernet.begin(mac, ip);

  // Open serial communications
  Serial.begin(9600);    // For USB Serial Monitor
  Serial1.begin(9600);   // Start Serial1
  Serial2.begin(9600);   // Start Serial2
  Serial3.begin(9600);   // Start Serial3
  Serial4.begin(9600);   // Start Serial4
  Serial5.begin(9600);   // Start Serial5

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
  // Check and send data from each serial port
  checkAndSendSerialData(Serial1);
  checkAndSendSerialData(Serial2);
  checkAndSendSerialData(Serial3);
  checkAndSendSerialData(Serial4);
  checkAndSendSerialData(Serial5);
}
