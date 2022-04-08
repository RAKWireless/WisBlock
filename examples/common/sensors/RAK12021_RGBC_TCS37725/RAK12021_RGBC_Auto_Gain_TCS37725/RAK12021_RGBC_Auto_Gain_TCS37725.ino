/**
   @file RAK12021_RGBC_Auto_Gain_TCS37725.ino
   @author rakwireless.com
   @brief  Use automatic gain to read RGBC data.
   @version 0.1
   @date 2021-11-08
   @copyright Copyright (c) 2020
**/
#include <Wire.h>
#include "TCS3772.h"  // Click here to get the library: http://librarymanager/All#TCS37725

// It use WB_IO2 to power up and is conflicting with INT1, so better use in SlotA/SlotC/SlotD.

TCS3772 tcs3772;

void setup() 
{
  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

#if !defined (_VARIANT_RAK11300_) 
  Wire.begin();
#endif
  
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
  Serial.println("RAK12021 RGBC auto gain example.");
  
  if(tcs3772.begin() == true)
  {
    Serial.println("Found sensor.");
  }
  else
  {
    Serial.println("TCS37725 not found ... check your connections.");
    while(1)
    {
      delay(10);  
    }
  }
  delay(1000);
}

void loop() 
{  
  TCS3772_DataScaled tcs3772_data = {0};
  
  uint16_t scale_factor;
  
  tcs3772_data = tcs3772.getMeasurement();

  scale_factor = tcs3772.autoGain(tcs3772_data.clear);
  
  Serial.print("  R: ");
  Serial.print(tcs3772_data.red);
  Serial.print("  G: ");
  Serial.print(tcs3772_data.green);
  Serial.print("  B: ");
  Serial.print(tcs3772_data.blue);
  Serial.print("  C: ");
  Serial.print(tcs3772_data.clear);
  Serial.print("  P: ");
  Serial.print(tcs3772_data.blue);
  Serial.print("  Gain: ");
  Serial.println(scale_factor);

  delay(1000);
}
