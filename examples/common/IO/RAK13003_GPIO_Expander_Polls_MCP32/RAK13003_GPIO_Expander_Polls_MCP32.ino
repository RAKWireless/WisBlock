/**
   @file RAK13003_GPIO_Expander_Polls_MCP32.ino
   @author rakwireless.com
   @brief Use IIC to expand 16 GPIO. 
          Polling the expander status to detect whether there is a low level on PA or PB.
   @version 0.1
   @date 2021-2-24
   @copyright Copyright (c) 2021
**/

#include <Wire.h>
#include "Adafruit_MCP23017.h"  //http://librarymanager/All#Adafruit_MCP23017

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
  
  Serial.println("MCP23017 Polls GPIO Test.");
  
  mcp.begin();  // Use default address 0.
  
  for(int i=0 ;i < 16 ;i++)
  {
    mcp.pinMode(i, INPUT);  // Set GPIO to input mode.
    mcp.pullUp(i, HIGH);  // Turn on a 100K pullup internally. 
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
