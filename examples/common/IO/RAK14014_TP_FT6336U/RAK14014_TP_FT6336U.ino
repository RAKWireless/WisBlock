/**
   @file RAK14014_TP_FT6336U.ino
   @author rakwireless.com
   @brief This example is for printing touch locations.
   @version 0.1
   @date 2022-6-28
   @copyright Copyright (c) 2022
**/

#include "RAK14014_FT6336U.h"  // Click here to get the library: http://librarymanager/All#RAK14014_FT6336U

#define INT_PIN   WB_IO6

FT6336U ft6336u; 

static uint8_t intSattus = false; // TP interrupt generation flag.

void setup(void) 
{
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
  while(ft6336u.begin() == false) 
  {
		Serial.println("ft6336u is not connected.");
		delay(1000);
  }
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT_PIN), tpIntHandle, FALLING);

  Serial.print("FT6336U Firmware Version: "); 
  Serial.println(ft6336u.read_firmware_id());  
  Serial.print("FT6336U Device Mode: "); 
  Serial.println(ft6336u.read_device_mode());  
}

void loop() 
{
	if(intSattus)
	{
		intSattus = false;
    Serial.print("FT6336U TD Status: "); 
    Serial.println(ft6336u.read_td_status());  
    Serial.print("FT6336U Touch Event/ID 1: ("); 
    Serial.print(ft6336u.read_touch1_event()); 
    Serial.print(" / "); 
    Serial.print(ft6336u.read_touch1_id()); 
    Serial.println(")"); 
    Serial.print("FT6336U Touch Position 1: ("); 
    Serial.print(ft6336u.read_touch1_x()); 
    Serial.print(" , "); 
    Serial.print(ft6336u.read_touch1_y()); 
    Serial.println(")"); 
    Serial.print("FT6336U Touch Weight/MISC 1: ("); 
    Serial.print(ft6336u.read_touch1_weight()); 
    Serial.print(" / "); 
    Serial.print(ft6336u.read_touch1_misc()); 
    Serial.println(")"); 
    Serial.print("FT6336U Touch Event/ID 2: ("); 
    Serial.print(ft6336u.read_touch2_event()); 
    Serial.print(" / "); 
    Serial.print(ft6336u.read_touch2_id()); 
    Serial.println(")"); 
    Serial.print("FT6336U Touch Position 2: ("); 
    Serial.print(ft6336u.read_touch2_x()); 
    Serial.print(" , "); 
    Serial.print(ft6336u.read_touch2_y()); 
    Serial.println(")"); 
    Serial.print("FT6336U Touch Weight/MISC 2: ("); 
    Serial.print(ft6336u.read_touch2_weight()); 
    Serial.print(" / "); 
    Serial.print(ft6336u.read_touch2_misc()); 
    Serial.println(")"); 
	}
	delay(200);
}

void tpIntHandle(void)
{
  intSattus = true;
}
