/**
   @file RAK13010_SDI_12_Basic_Data_Request.ino
   @author rakwireless.com
   @brief  Basic Data Request to a Single Sensor.
           This is a very basic (stripped down) example where the user initiates a measurement
           and receives the results to a terminal window without typing numerous commands into
           the terminal.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#include "RAK13010_SDI12.h"  // Click to install library: http://librarymanager/All#RAK12033-SDI12

#define DATA_PIN    WB_IO6   // The pin of the SDI-12 data bus.

#define SENSOR_ADDRESS 5

RAK_SDI12 mySDI12(DATA_PIN);

String sdiResponse = "";
String myCommand   = "";

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  // Power the sensors.

  // Initialize Serial for debug output.
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

  Serial.println("Opening SDI-12 bus.");
  mySDI12.begin();
  delay(500);

  mySDI12.forceListen();
  Serial.println("Waiting for input:");
}

void loop() 
{
  static uint8_t serialMsgRflag = 0;

  static String  sdiMsgStr;
  static boolean sdiMsgReady = false;
  
  if (Serial.available()) 
  {
    char inByte1 = Serial.read();
    if (inByte1 == '\r' || inByte1 == '\n') 
    {
      serialMsgRflag = 1;
    } 
  }

  int avail = mySDI12.available();
  if (avail < 0) 
  {
    mySDI12.clearBuffer();  // Buffer is full,clear.
  }  
  else if (avail > 0) 
  {
    for (int a = 0; a < avail; a++) 
    {
      char inByte2 = mySDI12.read();
      if (inByte2 == '\n') 
      {
        sdiMsgReady = true;
      } 
      else 
      {
        sdiMsgStr += String(inByte2);
      }
    }
  }

  if (sdiMsgReady)
  {
    Serial.println("R: "+ sdiMsgStr);
    sdiMsgReady = false;  // Reset String for next SDI-12 message.
    sdiMsgStr   = "";
    if(serialMsgRflag == 2)
    {
      serialMsgRflag = 3;
    }
  }

  if (serialMsgRflag)
  {
    if(serialMsgRflag == 1)
    {
      serialMsgRflag = 2;
      mySDI12.sendCommand(String(SENSOR_ADDRESS) + "M!");
      Serial.println("T: "+ String(SENSOR_ADDRESS) + "M!");
    }
    if(serialMsgRflag == 3)
    {
      serialMsgRflag = 0;
      delay(2000);
      mySDI12.sendCommand(String(SENSOR_ADDRESS) + "D0!");
      Serial.println("T: "+ String(SENSOR_ADDRESS) + "D0!");
      mySDI12.clearBuffer();
    }
  }
}
