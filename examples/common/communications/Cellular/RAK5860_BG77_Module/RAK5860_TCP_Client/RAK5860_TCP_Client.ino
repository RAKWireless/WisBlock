/**
   @file RAK5860_TCP_Server.ino
   @author rakwireless.com
   @brief Example of how to use TCP send date.
   @version 0.1
   @date 2023-3-14
   @copyright Copyright (c) 2023
**/

#include "Arduino.h"

#if defined NRF52_SERIES
  #include <Adafruit_TinyUSB.h>
#endif

#define BG77_POWER_KEY WB_IO1

String command; // String to store BG77 commnads.

// Read the return value of BG77.
void BG77_read(time_t timeout)
{
  time_t timeStamp = millis();
  while((millis() - timeStamp) < timeout)
  {
    if(Serial1.available()>0) 
    {
      Serial.print((char)Serial1.read());
      delay(1);
    }
  }
}

// Write commnads to BG77.
void BG77_write(const char *command) 
{
  while(*command)
  {
    Serial1.write(*command);
    command++;
  }
  Serial1.println();
}

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  
  time_t serial_timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  
  // Check if the modem is already awake
  time_t timeout = millis();
  bool moduleSleeps = true;
  Serial1.begin(115200);
  delay(1000);
  Serial1.println("ATI");
  //BG77 init
  while ((millis() - timeout) < 4000)
  {
    if (Serial1.available())
    {
      String result = Serial1.readString();
      Serial.println("Modem response after start:");
      Serial.println(result);
      moduleSleeps = false;
    }
  }
  if (moduleSleeps)
  {
    // Module slept, wake it up
    pinMode(BG77_POWER_KEY, OUTPUT);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
    digitalWrite(BG77_POWER_KEY, 1);
    delay(2000);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
  }
  Serial.println("BG77 power up!");
  delay(1000);

  command = "AT+CFUN=1,0\r";
  BG77_write(command.c_str());
  BG77_read(15000);

  command = "AT+CGATT=1\r";
  BG77_write(command.c_str());
  BG77_read(2000);

  command = "AT+CEREG?\r";
  BG77_write(command.c_str());
  BG77_read(2000);
  
  command = "AT+CPIN?\r";
  BG77_write(command.c_str());
  BG77_read(2000);
  
  command = "AT+QNWINFO\r";
  BG77_write(command.c_str());
  BG77_read(2000);
  
  command = "AT+QCSQ\r";
  BG77_write(command.c_str());
  BG77_read(2000);

  command = "AT+CSQ\r";
  BG77_write(command.c_str());
  BG77_read(2000);
  
  command = "AT+QIACT=1\r";
  BG77_write(command.c_str());
  BG77_read(2000);

  // Open a Socket Service
  command = "AT+QIOPEN=1,0,\"TCP\",\"112.125.89.8\",36399,0,0\r";
  BG77_write(command.c_str());
  BG77_read(2000);
}

void loop()
{
  // Send Hex String
  command = "AT+QISENDEX=0,\"313233343536\"\r";
  BG77_write(command.c_str());
  BG77_read(10000);
}
