/**
   @file RAK13800_Ethernet_MQTT_Publish_W5100S.ino
   @author rakwireless.com
   @brief  This example connects to a MQTT broker and publishes a message to a topic once a second.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h>   // Click to install library: http://librarymanager/All#RAKwireless_W5100S
#include <ArduinoMqttClient.h> // Click to install library: http://librarymanager/All#ArduinoMqttClient

#define USER_NAME     "admin"       // Provide a username and password for authentication.
#define PASSWORD      "password"
#define SERVER_PORT   61612         //  Define the server port.

IPAddress ip(192, 168, 1, 177);     // Set IP address,dependent on your local network.

IPAddress server(192, 168, 1, 101); // Set the server IP address.

EthernetClient client;

MqttClient mqttClient(client);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

int        port     = 61612;
const char topic[]  = "/simple/test";

const long interval = 1000;   // Upload data interval.
unsigned long previousMillis = 0;

int count = 0;

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.

  pinMode(WB_IO3, OUTPUT);
  digitalWrite(WB_IO3, LOW);  // Reset Time.
  delay(100);
  digitalWrite(WB_IO3, HIGH);  // Reset Time.
  
  // Open serial communications and wait for port to open:
  time_t timeout = millis();
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
  Serial.println("RAK13800 Ethernet MQTT Publish example.");

  Ethernet.init( SS );  // Set CS pin.
  Ethernet.begin(mac, ip);  // Start the Ethernet connection:

  if (Ethernet.hardwareStatus() == EthernetNoHardware)  // Check for Ethernet hardware present
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

  delay(1000);  // Give the Ethernet shield a second to initialize:

  // You can provide a unique client ID, if not set the library uses Arduino-millis().
  // Each client must have a unique client ID.
  mqttClient.setId("RAK13800_W5100s");

  mqttClient.setUsernamePassword(USER_NAME, PASSWORD);

  Serial.print("Attempting to connect to the MQTT broker...");

  if (!mqttClient.connect(server, SERVER_PORT)) 
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    delay(100);
    while (true) 
    {
      delay(1); // Do nothing, just love you.
    }
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() 
{
  // Call poll() regularly to allow the library to send MQTT keep alives which.
  // Avoids being disconnected by the broker.
  mqttClient.poll();

  unsigned long currentMillis = millis();   // Avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay.
  
  if (currentMillis - previousMillis >= interval) 
  {
    // Save the last time a message was sent.
    previousMillis = currentMillis;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("hello ");
    Serial.println(count);

    // Send message, the Print interface can be used to set the message contents.
    mqttClient.beginMessage(topic);
    mqttClient.print("hello ");
    mqttClient.print(count);  //  Can be changed to the sensor data if you want.
    mqttClient.endMessage();

    Serial.println();
    count++;
  }
}
