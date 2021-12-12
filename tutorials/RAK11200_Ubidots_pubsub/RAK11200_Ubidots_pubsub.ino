#include "UbidotsEsp32Mqtt.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>  // Click to install library: http://librarymanager/All#Adafruit_BME680

Adafruit_BME680 bme;
#define SEALEVELPRESSURE_HPA (1010.0)

#define OC_PIN    WB_IO3
#define RELAY_PIN WB_IO4

const char *UBIDOTS_TOKEN = "******";         // Put your Ubidots Token here
const char *WIFI_SSID = "******";             // Put your WiFi SSID
const char *WIFI_PASS = "******";             // Put your WiFi Password

const char *DEVICE_LABEL = "rak11200";        // Ubidots device name
const char *VARIABLE_LABEL = "temp";          // Ubidots temperature variable name
const char *VARIABLE_LABEL_2 = "hum";         // Ubidots humidity variable name
const char *VARIABLE_LABEL_3 = "press";       // Ubidots pressure variable name
const char *VARIABLE_LABEL_4 = "gas-res";     // Ubidots gas resistance variable name
const char *SUBSCRIBE_VARIABLE_LABEL = "relay";  // Ubidots relay variable name


const int PUBLISH_FREQUENCY = 5000; 

unsigned long timer;

Ubidots ubidots(UBIDOTS_TOKEN);


void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }

  if ((char)payload[0]=='1')
  {
    digitalWrite(RELAY_PIN, HIGH);
  }
  else
  {
    digitalWrite(RELAY_PIN, LOW);
  }

  Serial.println();
}

void bme680_init()
{
  Wire.begin();

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    return;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}


void setup()
{
  // put your setup code here, to run once:
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  Serial.begin(115200);
  bme680_init();
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  pinMode(RELAY_PIN,OUTPUT);
  pinMode(OC_PIN,INPUT);
  ubidots.setup();
  ubidots.reconnect();
  ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL);
  timer = millis();
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!ubidots.connected())
  {
    ubidots.reconnect();
    ubidots.subscribeLastValue(DEVICE_LABEL, SUBSCRIBE_VARIABLE_LABEL);
  }

  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  
  if (abs(millis() - timer) > PUBLISH_FREQUENCY) 
  {
    ubidots.add(VARIABLE_LABEL, bme.temperature);
    ubidots.add(VARIABLE_LABEL_2, bme.humidity);
    ubidots.add(VARIABLE_LABEL_3, (bme.pressure / 100.0));
    ubidots.add(VARIABLE_LABEL_4, (bme.gas_resistance / 1000.0));
    
    ubidots.publish(DEVICE_LABEL);
    timer = millis();
  }
  ubidots.loop();
}
