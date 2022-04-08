/**
   @file RAK13800_Ethernet_TCP_Client_W5100S.ino
   @author rakwireless.com
   @brief  Establish connection with TCP server and send data.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

#define SERVER_PORT   8080    //  Define the server port.

IPAddress ip(192, 168, 1, 177); // Set IP address,dependent on your local network.

IPAddress server(192, 168, 1, 101); // Set the server IP address,dependent on your local network.

EthernetClient client;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

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
  Serial.println("RAK13800 Ethernet TCP Client example.");

  Ethernet.init( SS );
  Ethernet.begin(mac, ip);

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

  delay(1000); // Give the Ethernet shield a second to initialize.
  Serial.println("connecting...");

  
  if (client.connect(server, SERVER_PORT)) 
  {
    Serial.println("connected");  // If you get a connection, report back via serial.
  } 
  else 
  {
    Serial.println("connection failed");  // If you didn't get a connection to the server.
  }
}

void loop() 
{
  if (client.available()) 
  {
    char c = client.read();
    Serial.print(c);  //  If there are incoming bytes available from the server, read them and print them.
  }

  while (Serial.available() > 0) 
  {
    char inChar = Serial.read();
    if (client.connected()) 
    {
      client.print(inChar);   // As long as there are bytes in the serial queue, read them and send them out the socket if it's open.
    }
  }

  if (!client.connected()) 
  {
    Serial.println();
    Serial.println("disconnecting."); // If the server's disconnected, stop the client.
    client.stop();
    while (true) 
    {
      delay(1); // You can add the reconnection code here.
    }
  }
}
