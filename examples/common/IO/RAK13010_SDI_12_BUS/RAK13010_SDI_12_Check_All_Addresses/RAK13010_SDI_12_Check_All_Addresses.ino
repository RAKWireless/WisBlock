/**
   @file RAK13010_SDI_12_Check_All_Addresses.ino
   @author rakwireless.com
   @brief  Check all Addresses for Active Sensors and Print Status.
           It discovers the address of all sensors active and attached to the board.
           THIS CAN BE *REALLY* SLOW TO RUN!!!
           Each sensor should have a unique address already - if not, multiple sensors may
           respond simultaenously to the same request and the output will not be readable
           by the Arduino.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
#include "RAK13010_SDI12.h" // Click to install library: http://librarymanager/All#RAK12033-SDI12

#define DATA_PIN    WB_IO6   // The pin of the SDI-12 data bus.

RAK_SDI12 mySDI12(DATA_PIN);

/**
 * @brief Gets identification information from a sensor, and prints it to the serial port expects.
 * @param i A character between '0'-'9', 'a'-'z', or 'A'-'Z'
 */
void printInfo(char i) 
{
  String command = "";
  command += (char)i;
  command += "I!";
  mySDI12.sendCommand(command);
  mySDI12.clearBuffer();
  delay(30);

  Serial.print("  --");
  Serial.print(i);
  Serial.print("--  ");

  while (mySDI12.available()) 
  {
    Serial.write(mySDI12.read());
    delay(10);  // 1 character ~ 7.5ms.
  }
}

/**
 * @brief this checks for activity at a particular address.
 * @param i A character between '0'-'9', 'a'-'z', or 'A'-'Z'
 */
boolean checkActive(char i) 
{
  String myCommand = "";
  myCommand        = "";
  myCommand += (char)i;  // Sends basic 'acknowledge' command [address][!].
  myCommand += "!";

  for (int j = 0; j < 3; j++) 
  {
//    Serial.printf("sdi.sendCommand(myCommand);\r\n");
//    delay(100);
    mySDI12.sendCommand(myCommand);
    mySDI12.clearBuffer();
    delay(30);
    if (mySDI12.available()) 
    {
      return true;
    }
  }
  mySDI12.clearBuffer();
  return false;
}

void scanAddressSpace(void) 
{
  for (char i = '0'; i <= '9'; i++) // Scan address space 0-9.
  {
    Serial.printf("Scan Address Space = %c\r\n",i);
    if (checkActive(i)) 
    { 
      printInfo(i); 
    }
  }
  
  for (char i = 'a'; i <= 'z'; i++) // Scan address space a-z.
  {
    Serial.printf("Scan Address Space = %c\r\n",i);
    if (checkActive(i))
    { 
      printInfo(i); 
    }
  }

  for (char i = 'A'; i <= 'Z'; i++) // Scan address space A-Z.
  {
    Serial.printf("Scan Address Space = %c\r\n",i);
    if (checkActive(i)) 
    { 
      printInfo(i); 
    }
  }
}

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
  Serial.println("Start Search for SDI-12 Devices.");

  mySDI12.begin();
  delay(500);
  scanAddressSpace();
  mySDI12.end();

  Serial.println("End Search for SDI-12 Devices.");

  digitalWrite(WB_IO2, LOW); // Cut power.
}

void loop() 
{
  delay(100);
}
