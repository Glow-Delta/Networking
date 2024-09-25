#include <SPI.h>
#include <Ethernet.h>

// Ethernet settings
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
IPAddress ip(192, 168, 0, 2);  // Arduino IP address
IPAddress server(192, 168, 0, 1);  // Raspberry Pi IP address (server)
EthernetClient client;

float TubeNumber = 404;
float ActivationFloat = 0;
float[] valueToSend = [TubeNumber, ActivationFloat];

void setup() {
  // Initialize Ethernet and Serial communication
  Ethernet.begin(mac, ip);
  Serial.begin(9600);

  // Allow the Ethernet shield to initialize
  delay(1000);

  // Connect to Raspberry Pi server
  if (client.connect(server, 80)) {
    Serial.println("Connected to Raspberry Pi");

    // Make an HTTP POST request with a value
    sendValueToServer(valueToSend);
  } else {
    Serial.println("Connection failed");
  }
}

void loop() {
  // If there are incoming bytes from the server, print them
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // If the server disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnected");
    client.stop();

    // Wait before reconnecting
    delay(5000);

    // Try to reconnect and send another value
    if (client.connect(server, 80)) {
      Serial.println("Reconnecting...");

      valueToSend++;  // Increment the value as an example
      sendValueToServer(valueToSend);
    }
  }
}

// Function to send a value to the server using HTTP POST
void sendValueToServer(int value) {
  String postData = "value=" + String(value);

  client.println("POST /data HTTP/1.1");
  client.println("Host: 192.168.0.1");  // Raspberry Pi IP address
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(postData.length());
  client.println();  // End of headers
  client.println(postData);  // Body with the data to send

  Serial.print("Sent value: ");
  Serial.println(value);
}