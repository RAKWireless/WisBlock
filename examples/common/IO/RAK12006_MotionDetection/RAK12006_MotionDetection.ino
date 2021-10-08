/**
 * @file RAK12006_MotionDetection.ino
 * @author rakwireless.com
 * @brief motion detection example
 * @version 0.1
 * @date 2021-06-15
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#define SENSOR_PIN  WB_IO6   // Attach AM312 sensor to Arduino Digital Pin WB_IO6

int gCurrentStatus = 0;         // variable for reading the pin current status
int gLastStatus = 0;            // variable for reading the pin last status

void setup() 
{
   pinMode(SENSOR_PIN, INPUT);   // The Water Sensor is an Input
   pinMode(LED_GREEN, OUTPUT);  // The LED is an Output
   pinMode(LED_BLUE, OUTPUT);   // The LED is an Output   
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
   Serial.println("========================");
   Serial.println("    RAK12006 test");
   Serial.println("========================");
}

void loop() {  

  gCurrentStatus = digitalRead(SENSOR_PIN);
  if(gLastStatus != gCurrentStatus)
  {
    if(gCurrentStatus == 0)
    {//0: detected   1: not detected
      Serial.println("IR detected ...");
     digitalWrite(LED_GREEN,HIGH);   //turn on
     digitalWrite(LED_BLUE,HIGH);
    }
    else
    {
      digitalWrite(LED_GREEN,LOW);
      digitalWrite(LED_BLUE,LOW);   // turn LED OF
    }   
    gLastStatus = gCurrentStatus;
  }
  else
  {
    delay(100);
  }

}
