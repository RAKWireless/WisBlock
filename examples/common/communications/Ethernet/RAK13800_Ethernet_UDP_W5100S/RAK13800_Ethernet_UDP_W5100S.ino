/**
   @file RAK13800_Ethernet_UDP_W5100S.ino
   @author rakwireless.com
   @brief  Receives UDP message strings, prints them to the serial port and sends an "acknowledge" string back to the sender.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

#define LOCAL_PORT   8888    // Set the server port.

IPAddress ip(192, 168, 1, 177); // Set IP address,dependent on your local network.

EthernetUDP Udp;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // Buffer to hold incoming packet.
char ReplyBuffer[] = "Received";            // String to send back.

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
  Serial.println("RAK13800 Ethernet UDP W5100S example.");

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

  Udp.begin(LOCAL_PORT);  // Start UDP.
  Serial.println("Udp begin OK.");
}

void loop() 
{
  int packetSize = Udp.parsePacket(); // If there's data available, read a packet.
  if (packetSize) 
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i=0; i < 4; i++) 
    {
      Serial.print(remote[i], DEC);
      if (i < 3) 
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); // Read the packet into packetBufffer.
    Serial.println("Contents:");
    Serial.println(packetBuffer);

    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());  // Send a reply to the IP address and port that sent us the packet we received.
    Udp.write(ReplyBuffer);
    Udp.endPacket();

    memset(packetBuffer,0,UDP_TX_PACKET_MAX_SIZE);
  }
  delay(10);
}
