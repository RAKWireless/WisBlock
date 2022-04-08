/**
   @file RAK12025_I3G4250D_Gyroscope.ino
   @author rakwireless.com
   @brief Gyroscope example
   @version 0.1
   @date 2021-9-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include "I3G4250D.h"	// Click here to get the library: http://librarymanager/All#I3G4250D
// It use WB_IO2 to power up and is conflicting with INT1, so better use in Slot C and Slot D.
I3G4250D i3g4240d;
I3G4250D_DataScaled i3g4240d_data = {0};
void setup()
{
  uint8_t id = 0;
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  uint8_t error = -1;
	Serial.begin(115200);
	Serial.println("Gyroscope example");
  error = i3g4240d.I3G4250D_Init(0x0F,0x00,0x00,0x00,0x00,I3G4250D_SCALE_500);
  if(error != 0)
  {
    Serial.println("init fail");
  }
   i3g4240d.readRegister(0x0F,&id,1);
   Serial.print("Gyroscope Device ID = ");
   Serial.println(id,HEX);
}

void loop()
{ 
  i3g4240d_data = i3g4240d.I3G4250D_GetScaledData();
  Serial.println();
  Serial.print("Gyroscope X(dps) = ");
  Serial.println(i3g4240d_data.x);  
  Serial.print("Gyroscope Y(dps) = ");
  Serial.println(i3g4240d_data.y);  
  Serial.print("Gyroscope Z(dps) = ");
  Serial.println(i3g4240d_data.z); 
  delay(1000);
}
