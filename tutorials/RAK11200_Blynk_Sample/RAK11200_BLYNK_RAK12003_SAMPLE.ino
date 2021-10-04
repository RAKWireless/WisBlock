/*************************************************************
  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL320J4BUQ" // use your own template ID from the generated template.
#define BLYNK_DEVICE_NAME "Quickstart Template" // use your own device name from the generated template.
#define BLYNK_AUTH_TOKEN "HU-yVkXbPSKOB7VYdCFf3ESB0xJ03p3Z"; // use your own blynk auth token name from the generated template.
#include <Wire.h>
#include "SparkFun_MLX90632_Arduino_Library.h"   // Click here to get the library: http://librarymanager/AllSparkFun_MLX90632_Arduino_Library
#define MLX90632_ADDRESS 0x3A
MLX90632 RAK_TempSensor;

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>w
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN; //this is provided on your template

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "WiFi_SSID"; //your network credentials
char pass[] = "WiFi_Password"; //your network credentials

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
  Blynk.virtualWrite(V5, objectTemp()); // this is from RAK12003
}

void setup()
{
  // Debug console
  Serial.begin(115200);
  time_t timeout = millis();
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
  
  //Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
  tempSetup();
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  Serial.println(objectTemp());
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

void tempSetup() {
  TwoWire &wirePort = Wire;
  MLX90632::status returnError;
  Serial.println("MLX90632 Read Example");

  Wire.begin(); //I2C init

  if (RAK_TempSensor.begin(MLX90632_ADDRESS, wirePort, returnError) == true) //MLX90632 init
  {
    Serial.println("MLX90632 Init Succeed");
  }
  else
  {
    Serial.println("MLX90632 Init Failed");
  }
}

double objectTemp() {
  double object_temp = RAK_TempSensor.getObjectTempF(); //Get the temperature of the object we're looking at in Farenheit
  return object_temp;
}

double internalTemp() {
  double sensor_temp = RAK_TempSensor.getSensorTemp(); //Get the temperature of the sensor in Celcius
  return sensor_temp;
}
