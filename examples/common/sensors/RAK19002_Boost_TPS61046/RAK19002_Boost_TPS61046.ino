/**
   @file RAK19002_Boost_TPS61046.ino
   @author rakwireless.com
   @brief Boost module can output 12V/80mA or 24V/40mA on Slot A.
   @version 0.1
   @date 2021-1-28
   @copyright Copyright (c) 2020
**/

#include <Arduino.h>

//Attention: The PIN is changed with the Slot as below:
//Slot A: WB_IO1
//Slot B: WB_IO2
//Slot C: WB_IO3
//Slot D: WB_IO5

void setup()
{
  pinMode(WB_IO1, OUTPUT);

}
void loop()
{
  //enable the boost and output 12V
  digitalWrite(WB_IO1,HIGH);
  delay(20000);
}
