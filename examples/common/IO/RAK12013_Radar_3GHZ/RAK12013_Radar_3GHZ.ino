/**
   @file RAK12013_Rader_3GHZ.ino
   @author rakwireless.com
   @brief Rader_3GHZ example.
		  The RCWL-0516 is a motion detection sensor. 
		  It can detect motion through doppler microwave technology through walls or other materials. 
		  It will get triggered not only by people but also by other moving objects.
   @version 0.1
   @date 2021-10-18
   @copyright Copyright (c) 2020
**/
#include <Wire.h>

#define SENSOR_OUT    WB_IO3
#define SENSOR_EN     WB_IO4
#define BLUE_LED      LED_BLUE
#define GREEN_LED     LED_GREEN

boolean g_motion_status = false;

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  
  pinMode (SENSOR_EN, OUTPUT);
  digitalWrite(SENSOR_EN, HIGH); // Sensor disable input (low = disable)
  
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
  Serial.println("Rader 3GHZ example");

  pinMode (SENSOR_OUT, INPUT); 
  pinMode (BLUE_LED, OUTPUT);  
  pinMode (GREEN_LED, OUTPUT);
  Serial.println("Waiting for motion");
}

void loop() 
{
  int val; // Read Pin as input
  
  val = digitalRead(SENSOR_OUT);
  if((val > 0) && (g_motion_status == false))
  {
    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    Serial.println("Motion Detected");
    g_motion_status = true;
  }
  if((val == 0) && (g_motion_status == true))
  {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
    Serial.println("NO Motion"); 
    g_motion_status = false;
  }
}
