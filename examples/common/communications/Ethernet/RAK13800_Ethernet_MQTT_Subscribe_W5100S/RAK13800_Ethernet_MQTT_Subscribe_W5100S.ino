/**
   @file RAK13800_Ethernet_MQTT_Subscribes_W5100S.ino
   @author rakwireless.com
   @brief  This example connects to a MQTT broker and subscribes to a single topic.
           When a message is received it prints the message to the serial monitor.
   @version 0.1
   @date 2021-11-02
   @copyright Copyright (c) 2021
**/

#include <SPI.h>
#include <RAK13800_W5100S.h>   // Click to install library: http://librarymanager/All#RAKwireless_W5100S
#include <ArduinoMqttClient.h> // Click to install library: http://librarymanager/All#ArduinoMqttClient

 // You can provide a username and password for authentication.
#define USER_NAME     "admin"
#define PASSWORD      "password"

IPAddress ip(192, 168, 1, 177); // Set IP address,dependent on your local network.

IPAddress server(192, 168, 1, 101); // Set the server IP address.

EthernetClient client;

MqttClient mqttClient(client);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // Set the MAC address, do not repeat in a network.

const char broker[] = "mybroker";
int        port     = 61612;
const char topic[]  = "/simple/test";

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
  Serial.println("RAK13800 Ethernet MQTT Subscribes example.");

  Ethernet.init( SS );      // Set CS pin.
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

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // You can provide a unique client ID, if not set the library uses Arduino-millis().
  // Each client must have a unique client ID.
  mqttClient.setId("RAK13800_W5100s");

  mqttClient.setUsernamePassword(USER_NAME, PASSWORD);

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(server, port)) 
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

  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();

  
  mqttClient.subscribe(topic);  // Subscribe to a topic.
  // mqttClient.unsubscribe(topic); // Topics can be unsubscribed.

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
}

void loop() 
{
  int messageSize = mqttClient.parseMessage();
  if (messageSize) 
  {
    // Received a message, print out the topic and contents.
    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");

    // Use the Stream interface to print the contents.
    while (mqttClient.available()) 
    {
      Serial.print((char)mqttClient.read());
    }
    Serial.println();
  }
}
