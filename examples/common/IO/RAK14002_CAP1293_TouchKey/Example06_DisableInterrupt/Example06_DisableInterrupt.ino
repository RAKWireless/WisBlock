/**
   @file Example06_DisableInterrupt.ino
   @author taylor.lee (taylor.lee@rakwireless.com)
   @brief Disabled interrupt example for CAP1293 touch sensor IC
   @version 0.1
   @date 2021-02-18

   @copyright Copyright (c) 2021

*/
#include <Wire.h>
#include "CAP1293.h"

CAP1293 sensor; // Initialize sensor

static const int8_t gInterruptPin = WB_IO6;   // interrupt pin
static bool gIsInterrupt = false;             // interrupt flag
bool keyStatus[3] = {false, false, false};    // all touch key status, false is released, true is pressed.

void interruptHandle()
{
  gIsInterrupt = true;
}

void setup()
{
  Wire.begin();      // Join I2C bus
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

  // set interrupt pin
  pinMode(gInterruptPin, INPUT);
  // set interrupt handle function
  attachInterrupt(gInterruptPin, interruptHandle, FALLING);

  /*
     Interrupt is enabled at begin() function,
     if you want disable it, please call setInterruptDisabled()
       When interrupt disabled is set, MCU cannot detect touch and release event.
  */
  sensor.setInterruptDisabled();

  Serial.print("Interrupt: ");
  if (sensor.isInterruptEnabled() == true)
  {
    Serial.println("ENABLED");
  }
  else
  {
    Serial.println("DISABLED");
  }
}

void loop()
{
  uint8_t keyChanged = 0;
  if (gIsInterrupt == true)
  {
    gIsInterrupt = false;

    keyChanged = sensor.getTouchKeyStatus(keyStatus);
    // some key's status have changed
    if (keyChanged > 0)
    {
      // Left
      if (keyChanged & 0x01)
      {
        if (keyStatus[0] == true)
        {
          Serial.println("Left Pressed");
        }
      }

      // Middle
      if (keyChanged & 0x02)
      {
        if (keyStatus[1] == true)
        {
          Serial.println("Middle Pressed");
        }
      }

      // Right
      if (keyChanged & 0x04)
      {
        if (keyStatus[2] == true)
        {
          Serial.println("Right Pressed");
        }
      }
    }
  }
}
