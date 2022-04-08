/**
   @file RAK16002_LTC2941_Coulomb.ino
   @author rakwireless.com
   @brief  Output current battery level.
   @version 0.1
   @date 2021-11-08
   @copyright Copyright (c) 2020
**/

#include "LTC2941.h"	//Click here to get the library: http://librarymanager/All#GROVE-Coulomb_Counter

#define BATTERY_CAPACITY	2200   // unit:mAh.	

#define CURRENT_CAPACITY  42352    // Set the current battery level to 1800 mAh.

void setup(void)
{
  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);	
  
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
  
  Serial.println("=====================================");
  Serial.println("RAK16002 Coulomb LTC2941 example");
  Serial.println("=====================================");
  
  Wire.begin();
    
  ltc2941.initialize();
  ltc2941.setBatteryFullMAh(BATTERY_CAPACITY , false);
  ltc2941.setAccumulatedCharge(CURRENT_CAPACITY); // Set the current battery level to 1800 mAh.
}

void loop(void)
{
  float coulomb = 0, mAh = 0, percent = 0;
  coulomb = ltc2941.getCoulombs();
  mAh = ltc2941.getmAh();
  percent = ltc2941.getPercent();
  
  Serial.print(coulomb);
  Serial.print("C   ");
  Serial.print(mAh);
  Serial.print("mAh   ");
  Serial.print(percent);
  Serial.println("%");
  delay(1000);
}
