/**
   @file RAK13003_GPIO_Expander_IO_MCP32.ino
   @author rakwireless.com
   @brief Use IIC to expand 16 GPIO. 
          Configure PA input PB output, or PA output PB input.Serial port print GPIO status.
   @version 0.1
   @date 2021-2-24
   @copyright Copyright (c) 2021
**/
#include <Wire.h>
#include "Adafruit_MCP23017.h"  //http://librarymanager/All#Adafruit_MCP23017

//#define PAIN_PBOUT  
#define PAOUT_PBIN 

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

  Serial.println("MCP23017 GPIO Input Output Test.");
  
  mcp.begin(); // use default address 0.
  
#ifdef PAIN_PBOUT 
  for(int i=0 ;i < 8 ;i++)
  {
    mcp.pinMode(i, INPUT);  // PA input. 
  }
  for(int j=8 ;j < 16 ;j++)
  {
    mcp.pinMode(j, OUTPUT); // PB output.
  }
  mcp.digitalWrite(8, LOW); // The output state of the PB port can be changed to high or low level.
  mcp.digitalWrite(9, HIGH);
  mcp.digitalWrite(10, LOW);
  mcp.digitalWrite(11, HIGH);

  mcp.digitalWrite(12, LOW);
  mcp.digitalWrite(13, HIGH);
  mcp.digitalWrite(14, LOW);
  mcp.digitalWrite(15, HIGH);

  Serial.println();
  for(int i=0; i < 8; i++ )
  {
    if(mcp.digitalRead(i) == 1)
      Serial.printf("GPIO A %d Read High\r\n",i);
    else
      Serial.printf("GPIO A %d Read Low\r\n",i);
  }
#endif

#ifdef PAOUT_PBIN 
  for(int i=0 ;i < 8 ;i++)
  {
    mcp.pinMode(i, OUTPUT); // PA output. 
  }
  for(int j=8 ;j < 16 ;j++)
  {
    mcp.pinMode(j, INPUT);  // PB input.
  }
  mcp.digitalWrite(0, LOW); // The output state of the PA port can be changed to high or low level.
  mcp.digitalWrite(1, HIGH);
  mcp.digitalWrite(2, LOW);
  mcp.digitalWrite(3, HIGH);

  mcp.digitalWrite(4, LOW);
  mcp.digitalWrite(5, HIGH);
  mcp.digitalWrite(6, LOW);
  mcp.digitalWrite(7, HIGH);
  Serial.println();
  for(int i=8; i < 16; i++ )
  {
    if(mcp.digitalRead(i) == 1)
      Serial.printf("GPIO B %d Read High\r\n",i-8);
    else
      Serial.printf("GPIO B %d Read Low\r\n",i-8);
  }
#endif
}
void loop() 
{
  // We do nothing here.
}
