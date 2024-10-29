#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
 
const int csPin = 10;
EthernetUDP Udp;
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress remoteIP(192, 168, 0, 1); // Replace with your target IP
const unsigned int remotePort = 50000; // Replace with your target port
 
void setup() {
  Serial.begin(9600); // Start Serial communication
  while (!Serial) {
    ; // Wait for Serial port to be available
  }
 
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH); // Ensure CS is high to start
 
  Serial.println("Initializing Ethernet...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(mac, IPAddress(192, 168, 0, 2)); // Static IP fallback
  } else {
    Serial.print("Ethernet IP: ");
    Serial.println(Ethernet.localIP());
  }
 
  Udp.begin(8888); // Start UDP on port 8888 (local port)
}
 
void loop() {
  // Check if data is available on Serial1
  if (Serial1.available()) {
    // Read the incoming message
    String message = Serial1.readStringUntil('\n');
    Serial.println("Received: " + message); // Print the received message for debugging
 
    // You can parse the message here if needed
    if (message.startsWith("id=")) {
      int idIndex = message.indexOf("id=") + 3;
      int activationIndex = message.indexOf(":activation_level=");
 
      if (activationIndex != -1) {
        // Extract the ID and activation level
        String id = message.substring(idIndex, activationIndex - 3);
        String activation = message.substring(activationIndex + 18); // Adjust index to get the activation level
        String RefactoredMessage = "";
 
        // Process the id and activation level as needed
        Serial.print("ID: ");
        Serial.println(id);
        Serial.print("Activation Level: ");
        Serial.println(activation);

        RefactoredMessage = "id=";
        RefactoredMessage += id;
        RefactoredMessage += ":activation_level=";
        RefactoredMessage += activation;

        // Send the message
        Udp.beginPacket(remoteIP, remotePort);
        Udp.print(RefactoredMessage);
        Udp.endPacket();
      
        Serial.print("Sent: ");
        Serial.println(RefactoredMessage);

        RefactoredMessage = "";
      }
    }
  }
}
