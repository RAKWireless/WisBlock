/**
   @file RAK12015_Shock_801S.ino
   @author rakwireless.com
   @brief Setup and read values from a 801S sensor
   @version 0.1
   @date 2021-07-23
   @copyright Copyright (c) 2021
**/

#include <Arduino.h>
#include "Wire.h"
#define SensorOutput   WB_A1
uint8_t interrupt1Flag = 0;

void setup() {
  // put your setup code here, to run once:
  time_t timeout = millis();
  Serial.begin(115200);
  /* WisBLOCK 12015 Power On*/
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  /* WisBLOCK 12015 Power On*/
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
  //falling edge fires , triggers interrupt a1, and calls the interrupt flag function
  pinMode(SensorOutput, INPUT);
  attachInterrupt(SensorOutput, interruptHandle1, RISING);
  Serial.println("RAK12015 test Example");
}


/*
 * brief:The threshold of the sensor is determined by the resistance of the pull-up resistor. 
 * Now the pull-up resistance is 10K
 */
void loop() {
  // put your main code here, to run repeatedly:
  if (interrupt1Flag == 1)
  {
    Serial.println("Trigger vibration sensor");
    interrupt1Flag=0;
  }
  delay(500);
}

void interruptHandle1(void)
{
  interrupt1Flag = 1;
}
