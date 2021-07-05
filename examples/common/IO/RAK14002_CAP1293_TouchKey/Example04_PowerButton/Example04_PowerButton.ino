/**
   @file Example04_PowerButton.ino
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief Power Button example for CAP1293 touch sensor IC
   @version 0.1
   @date 2021-02-18

   @copyright Copyright (c) 2021

*/
#include <Wire.h>
#include "CAP1293.h" // Click here to get the library: http://librarymanager/All#RAK14002-CAP1293-Library

CAP1293 sensor; // Initialize sensor

static const int8_t gInterruptPin = WB_IO6;   // interrupt pin
static bool gIsInterrupt = false;             // interrupt flag
bool gKeyStatus[3] = {false, false, false};    // all touch key status, false is released, true is pressed.

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

  /* SET POWER BUTTON PAD
      Set which pad acts as the power button: PAD_LEFT,PAD_MIDDLE,PAD_RIGHT
  */
  //sensor.setPowerButtonPad(PAD_LEFT);
  sensor.setPowerButtonPad(PAD_MIDDLE);
  //sensor.setPowerButtonPad(PAD_RIGHT);

  /* GET POWER BUTTON PAD
    Get the current pad which acts as the power button.
    The function returns the pad as an interger which
    maps to a specific position.

    PAD   POSITION
    1     Left
    2     Middle
    3     Right
  */
  int pad = sensor.getPowerButtonPad();
  Serial.print("Power Button Pad: ");
  if (pad == 1)
  {
    Serial.println("Left");
  }
  else if (pad == 2)
  {
    Serial.println("Middle");
  }
  if (pad == 3)
  {
    Serial.println("Right");
  }

  /* SET POWER BUTTON TIME
      Set the length of time (in ms) which the designated
      power button must indicate a touch before being
      recongized as a power button touch.
  */
  //sensor.setPowerButtonTime(PWR_TIME_280_MS);    // 280 ms
  //sensor.setPowerButtonTime(PWR_TIME_560_MS);    // 560 ms
  //sensor.setPowerButtonTime(PWR_TIME_1120_MS);   // 1120 ms
  sensor.setPowerButtonTime(PWR_TIME_2240_MS); // 2240 ms

  /* GET POWER BUTTON TIME
      Get the length of time the designated power button
      must indicate a touch before being recongized as a
      power button touch.
  */
  Serial.print("Power Button Time: ");
  Serial.print(sensor.getPowerButtonTime());
  Serial.println(" ms");

  /* SET ENABLE / DISABLE POWER BUTTON
      Enable or disable the power button functionality.
      When the power button is ENABLED, the specified pad
      (from setPowerButtonPad() function) acts as a power
      button. When the power button is DISABLED, all pads
      act as regular capactitive touch sensors.
  */
  sensor.setPowerButtonEnabled(); // Enable power button
  //sensor.setPowerButtonDisabled();  // Disable power button

  /* IS POWER BUTTON TOUCHED
      Returns the state of the power button. Returns true
      if ENABLED, otherwise returns false.
  */
  Serial.print("Power Button: ");
  if (sensor.isPowerButtonEnabled() == true)
  {
    Serial.println("ENABLED");
  }
  else
  {
    Serial.println("DISABLED");
  }

  // set interrupt
  pinMode(gInterruptPin, INPUT_PULLUP);
  attachInterrupt(gInterruptPin, interruptHandle, FALLING);
}

void loop()
{
  if (gIsInterrupt == true)
  {
    gIsInterrupt = false;

    uint8_t keyChanged = sensor.getTouchKeyStatus(gKeyStatus);
    // some key's status have changed
    if (keyChanged > 0)
    {
      // Left
      if (keyChanged & 0x01)
      {
        if (gKeyStatus[0] == true)
        {
          Serial.println("Left Pressed");
        }
      }

      // Middle
      if (keyChanged & 0x02)
      {
        if (gKeyStatus[1] == true)
        {
          Serial.println("Middle Pressed");
        }
      }

      // Right
      if (keyChanged & 0x04)
      {
        if (gKeyStatus[2] == true)
        {
          Serial.println("Right Pressed");
        }
      }
    }

    // power button
    if (sensor.isPowerButtonTouched() == true)
    {
      Serial.println("Power Button trigged");
    }
  }
}
