/**
   @file RAK14003_LED_BAR_MCP32.ino
   @author rakwireless.com
   @brief Use MCP23017 to control LED Bar.
		Colour：2 Red, 3 Yellow, 5 Green	
   @version 0.1
   @date 2021-6-18
   @copyright Copyright (c) 2021
**/
#include <Wire.h>
#include "Adafruit_MCP23017.h"  //http://librarymanager/All#Adafruit_MCP23017

#define IIC_ADDRESS 0X21 

Adafruit_MCP23017 mcp;
  
void setup() 
{  
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 1);
  
  // Reset device
  pinMode(WB_IO4, OUTPUT);
  digitalWrite(WB_IO4, 1);
  delay(10);
  digitalWrite(WB_IO4, 0);
  delay(10);
  digitalWrite(WB_IO4, 1);
  delay(10);
  
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
  
  mcp.begin(IIC_ADDRESS,&Wire); // use default address 0.
  
  for(int i=0 ;i < 16 ;i++)
  {
    mcp.pinMode(i, OUTPUT); // Set port as output. 
  }
  for(int j=0 ;j < 16 ;j++)
  {
	mcp.digitalWrite(j, HIGH); // Turn off all LEDs.
  }
}

void loop() 
{
	int i;
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, LOW);
		delay(500);
	}
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, HIGH);
		delay(500);
	}
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, LOW);
	}
	delay(1000);
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, HIGH);
	}
	delay(1000);
}
