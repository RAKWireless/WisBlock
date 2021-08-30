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

#define IIC_ADDRESS 0X04 

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
  
  mcp.begin(IIC_ADDRESS,&Wire); // use default address 0.
  
  for(int i=0 ;i < 16 ;i++)
  {
    mcp.digitalWrite(i, HIGH);  // Turn off all LEDs. 
    mcp.pinMode(i, OUTPUT);     // Set pins as output.
  }
}

void loop() 
{
	int i;
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, LOW);
		delay(200);
	}
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(9-i, HIGH);
		delay(200);
	}
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, LOW);
	}
	delay(300);
	for(i=0 ;i < 10 ;i++)
	{
		mcp.digitalWrite(i, HIGH);
	}
	delay(300);
}
