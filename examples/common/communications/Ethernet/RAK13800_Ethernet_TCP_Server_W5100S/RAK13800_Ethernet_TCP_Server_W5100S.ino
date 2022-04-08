/**
   @file RAK13800_Ethernet_TCP_Server_W5100S.ino
   @author rakwireless.com
   @brief  A simple server can communicate with any connected TCP client.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

IPAddress ip(192, 168, 1, 177);

IPAddress myDns(192, 168, 1, 1); 

IPAddress gateway(192, 168, 1, 1);  // optional : Set the gateway IP,dependent on your local network.

IPAddress subnet(255, 255, 255, 0);   // optional : Set the subnet,dependent on your local network.

EthernetServer server(8080);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

boolean alreadyConnected = false; // whether or not the client was connected previously.

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.

  pinMode(WB_IO3, OUTPUT);
  digitalWrite(WB_IO3, LOW);  // Reset Time.
  delay(100);
  digitalWrite(WB_IO3, HIGH);  // Reset Time.
  
  time_t timeout = millis();
  // Initialize Serial for debug output.
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println("RAK13800 Ethernet TCP Server W5100S example.");

  Ethernet.init( SS );
  Ethernet.begin(mac, ip, myDns, gateway, subnet);
  
  if (Ethernet.hardwareStatus() == EthernetNoHardware)  // Check for Ethernet hardware present.
  {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) 
    {
      delay(1); // Do nothing, just love you.
    }
  }
  while (Ethernet.linkStatus() == LinkOFF) 
  {
    Serial.println("Ethernet cable is not connected.");
    delay(500);
  }

  server.begin(); // start listening for clients

  Serial.print("Server address:");
  Serial.println(Ethernet.localIP());
}

void loop() 
{
  EthernetClient client = server.available(); // Wait for a new client.

  if (client) 
  {
    if (!alreadyConnected) // When the client sends the first byte, say hello.
    {
      client.flush(); // Clear out the input buffer.
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }
    if (client.available() > 0) 
    {
      char thisChar = client.read();  // Read the bytes incoming from the client.
      server.write(thisChar); // Echo the bytes back to the client.
      Serial.write(thisChar); // Echo the bytes to the server as well.
    }
  }
}
