/**
   @file RAK13800_Ethernet_DHCP_W5100S.ino
   @author rakwireless.com
   @brief  Get an IP address via DHCP and print the address obtained.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h> // Click to install library: http://librarymanager/All#RAKwireless_W5100S

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
  Serial.println("RAK13800 Ethernet DHCP example.");
  Ethernet.init( SS );

  Serial.println("Initialize Ethernet with DHCP.");   // start the Ethernet connection.
  if (Ethernet.begin(mac) == 0) 
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
  }
  
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP()); // Print your local IP address.
}

void loop() 
{
  //  Add your code here.
}
