/**
   @file RAK12017_Detect_Interrupt.ino
   @author rakwireless.com
   @brief Detect the objects by interrupt
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/
#ifdef RAK4630
#include <Adafruit_TinyUSB.h>
#endif

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);   
  
  pinMode(WB_IO4, INPUT);
  attachInterrupt(digitalPinToInterrupt(WB_IO4), blink, CHANGE);  
}

void blink() {
  if(digitalRead(WB_IO4) == 0)
    digitalWrite(LED_BLUE, HIGH);
  else
    digitalWrite(LED_BLUE, LOW);
}


void loop()
{

}
