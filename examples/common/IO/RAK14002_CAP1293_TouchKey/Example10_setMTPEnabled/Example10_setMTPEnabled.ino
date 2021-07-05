/**
   @file Example10_setMTPEnabled.ino
   @author Taylor Lee (taylor.lee@rakwireless.com)
   @brief Set MTP example for CAP1293 touch sensor IC
   @version 0.1
   @date 2021-03-09

   @copyright Copyright (c) 2021

*/
#include <Wire.h>
#include "CAP1293.h" // Click here to get the library: http://librarymanager/All#SparkFun_CAP1293

CAP1293 sensor; // Initialize sensor

static const int8_t gInterruptPin = WB_IO6;   // interrupt pin
static bool gIsInterrupt = false;             // interrupt flag

void interruptHandle()
{
  gIsInterrupt = true;
}

void setup()
{
  Wire.begin();     // Join I2C bus
  Serial.begin(115200); // Start serial for output
  time_t timeout = millis();
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

  // Setup sensor
  if (sensor.begin() == false)
  {
    Serial.println("Not connected. Please check connections and read the hookup guide.");
    while (1)
      ;
  }
  else
  {
    Serial.println("Connected!");
  }

  pinMode(gInterruptPin, INPUT_PULLUP);
  attachInterrupt(gInterruptPin, interruptHandle, FALLING);

  // MULTIPLE TOUCH PATTERN enable function
  // paramter is use left middle right as pattern or not.
  sensor.setMTPEnabled(true, false, true);
}

void loop()
{
  if (gIsInterrupt == true)
  {
    gIsInterrupt = 0;
    if (sensor.isMTPStatus())
    {
      Serial.println("MTP Detected!");
      sensor.clearStatus();
    }
    else
    {
      sensor.clearInterrupt();
    }
  }
}