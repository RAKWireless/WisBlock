/**
   @file RAK13003_GPIO_Expander_Polls_MCP32.ino
   @author rakwireless.com
   @brief Use IIC to expand 16 GPIO. 
          Polling the expander status to detect whether there is a low level on PA or PB.
   @version 0.2
   @date 2022-5-11
   @copyright Copyright (c) 2022
**/

#include <Wire.h>
#include "Adafruit_MCP23X17.h"  //http://librarymanager/All#Adafruit_MCP23017

Adafruit_MCP23X17 mcp;

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
  
  Serial.println("MCP23017 Polls GPIO Test.");
  
  mcp.begin_I2C();  // Use default address 0.
  
  for(int i=0 ;i < 16 ;i++)
  {
    mcp.pinMode(i, INPUT_PULLUP);  // Set GPIO to input mode with internal pullup resistor
  }
}
void loop()
{
  uint16_t status = 0;
  if(mcp.readGPIOAB() != 0xFFFF) // Polls whether any pin is pulled low.
  {
    status = mcp.readGPIOAB(); // Read GPIO status.
    
    for(int i=0 ;i < 16 ;i++)
    {
      if(!(status & 0x0001))
      {
        Serial.printf("MCP23017 Pin %d is pulled Low.\r\n",i);    
      }
      status>>=1;
    }
    delay(1000);
  }
}
