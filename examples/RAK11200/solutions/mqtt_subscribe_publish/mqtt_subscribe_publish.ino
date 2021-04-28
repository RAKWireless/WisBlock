/**
   @file mqtt_sub_pub.ino
   @author taylor.lee@rakwireless.com
   @brief conncet mqtt broker, subscribe RAK11200/led from mqtt broker server to control led on board,
          and publish temperature and humidity collected by RAK1906(BME680 sensor) to mqtt broker server.
   @version 0.1
   @date 2021-01-12
**/

#include <WiFi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/archive/master.zip
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h" //http://librarymanager/All#Adafruit_BME680

// Replace the next variables with your SSID/Password combination
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Replace it with your MQTT Broker IP address or domain
const char* mqtt_server = "test.mosquitto.org";

// Define an ID to indicate the device, If it is the same as other devices which connect the same mqtt server,
// it will lead to the failure to connect to the mqtt server
const char* mqttClientId = "REPLACE_WITH_YOUR_MQTT_ClIENT_ID";

// if need username and password to connect mqtt server, they cannot be NULL.
const char* mqttUsername = NULL;
const char* mqttPassword = NULL;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Adafruit_BME680 bme;

float temperature = 0;
float humidity = 0;

long lastMsg = 0;

// LED Pin
const int ledPin = WB_LED1;

void setup()
{
  Serial.begin(115200);

  if (!bme.begin(0x76))
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  // conncet WiFi AP
  setup_wifi();

  // connect mqtt broker
  mqttClient.setServer(mqtt_server, 1883);
  // set callback of receiving message
  mqttClient.setCallback(callback);

  // set LED pin mode
  pinMode(ledPin, OUTPUT);
}

/* conncet WiFi AP
   wait until wifi is connected.
*/
void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/* callback of receiving subscribed message
   it can only deal with topics that he has already subscribed.
*/
void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp = "";
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  // get and print message recieved
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // If a message is received on the topic RAK11200/led, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "RAK11200/led")
  {
    Serial.print("Changing led status to ");
    if (messageTemp == "on")
    {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off")
    {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }

  // Feel free to add more to handle more topics
}

/*
    reconnect to mqtt broker if disconnected
    when connected to mqtt broker, subscribe topic here.
*/
void reconnect() {
  // Loop until reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Connecting to  MQTT broker...");
    if (mqttClient.connect(mqttClientId, mqttUsername, mqttPassword))
    {
      Serial.println("connected");

      // Subscribe topics which you want to receive.
      mqttClient.subscribe("RAK11200/led");
      // you can add other Subscribe here
    }
    else
    {
      Serial.print("failed, code=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - lastMsg > 20000) // publish every 20s
  {
    lastMsg = now;

    // get temperature(Celsius) from BME680 sensor
    temperature = bme.readTemperature();

    // Convert the value to a char array
    char temperatureString[16] = {0};
    snprintf(temperatureString, sizeof(temperatureString), "%.1f Cel", temperature);
    Serial.print("Temperature: ");
    Serial.println(temperatureString);

    // publish temperature
    mqttClient.publish("RAK11200/temperature", temperatureString);

    // get humidity from BME680 sensor
    humidity = bme.readHumidity();

    // Convert the value to a char array
    char humidityString[16] = {0};
    snprintf(humidityString, sizeof(humidityString), "%.1f %%", humidity);
    Serial.print("Humidity: ");
    Serial.println(humidityString);

    // publish humidity
    mqttClient.publish("RAK11200/humidity", humidityString);
  }
}
