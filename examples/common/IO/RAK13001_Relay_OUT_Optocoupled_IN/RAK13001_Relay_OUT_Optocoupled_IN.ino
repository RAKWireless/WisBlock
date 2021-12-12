/**
   @file RAK13001_Relay_OUT_Optocoupled_IN.ino
   @author rakwireless.com
   @brief Relay output optocoupler input.
   @version 0.1
   @date 2021-3-18
   @copyright Copyright (c) 2021
**/
#include <Wire.h>

#define OC_PIN    WB_IO3
#define RELAY_PIN WB_IO4

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  
  // Initialize Serial for debug output
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
  
  pinMode(RELAY_PIN,OUTPUT);
  pinMode(OC_PIN,INPUT_PULLUP); 
}

void loop() 
{
  static uint8_t count=0;
  count++;
  if(count == 5)
  {
    digitalWrite( RELAY_PIN , LOW);
  }
  if(count == 10)
  {
    count=0;
    digitalWrite( RELAY_PIN , HIGH);
  }

  if(digitalRead(OC_PIN) == LOW)
  {
    Serial.println("Optocoupled PIN IN Low");
  }
  
  delay(1000);
}
