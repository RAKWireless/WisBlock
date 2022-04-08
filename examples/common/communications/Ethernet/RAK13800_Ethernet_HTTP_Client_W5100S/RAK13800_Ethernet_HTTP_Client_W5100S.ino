/**
   @file RAK13800_Ethernet_HTTP_Client_W5100S.ino
   @author rakwireless.com
   @brief  This example connects to a website (http://www.google.com).
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

#define SERVER_PORT   80    //  Define the server port.

// If you don't want to use DNS (and reduce your sketch size)
// Use the numeric IP instead of the name for the server.
//IPAddress server(74,125,232,128);  // Numeric IP for Google (no DNS)
char server[] = "www.google.com";    // Name address for Google (using DNS)

IPAddress ip(192, 168, 0, 177); // Set the static IP address to use if the DHCP fails to assign

IPAddress myDns(192, 168, 0, 1);

EthernetClient client;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // Set to false for better speed measurement

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
  Serial.println("RAK13800 Ethernet HTTP Client example.");

  Ethernet.init( SS );
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) // Start the Ethernet connection.
  {
    Serial.println("Failed to configure Ethernet using DHCP");
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
    
    Ethernet.begin(mac, ip, myDns); // Try to congifure using IP address instead of DHCP.
  } 
  else 
  {
    Serial.print("DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
 
  delay(1000);   // Give the Ethernet shield a second to initialize.
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  if (client.connect(server, SERVER_PORT)) // If you get a connection, report back via serial.
  {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
    // Make a HTTP request.
    client.println("GET /search?q=arduino HTTP/1.1");
    client.println("Host: www.google.com");
    client.println("Connection: close");
    client.println();
  } 
  else 
  {
    Serial.println("connection failed");  // If you didn't get a connection to the server.
  }
  beginMicros = micros();
}

void loop() 
{
  int len = client.available(); //  If there are incoming bytes available from the server, read them and print them.
  if (len > 0) 
  {
    byte buffer[80];
    if (len > 80) 
	{
      len = 80;
	}
    client.read(buffer, len);
    if (printWebData) 
    {
      Serial.write(buffer, len); // Show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  if (!client.connected()) // If the server's disconnected, stop the client.
  {
    endMicros = micros();
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    Serial.print("Received ");
    Serial.print(byteCount);
    Serial.print(" bytes in ");
    float seconds = (float)(endMicros - beginMicros) / 1000000.0;
    Serial.print(seconds, 4);
    float rate = (float)byteCount / seconds / 1000.0;
    Serial.print(", rate = ");
    Serial.print(rate);
    Serial.print(" kbytes/second");
    Serial.println();

    while (true) 
    {
      delay(1); // Do nothing forevermore.
    }
  }
}
