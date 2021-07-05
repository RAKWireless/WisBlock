/**
   @file Example07_SetSensitivity.ino
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief Set Sensitivity example for CAP1293 touch sensor IC
   @version 0.1
   @date 2021-02-18

   @copyright Copyright (c) 2021

*/
#include <Wire.h>
#include "CAP1293.h" // Click here to get the library: http://librarymanager/All#SparkFun_CAP1293

CAP1293 sensor; // Initialize sensor

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

  /* SET SENSITIVITY
      Set the sensitivity multiplier for different applications,
      such as different sized touch pads. The default sensitivity
      multiplier is set to 2x.

      Different sensitivity settings:
      SENSITIVITY_128X  // Most sensitive
      SENSITIVITY_64X
      SENSITIVITY_32X
      SENSITIVITY_16X
      SENSITIVITY_8X
      SENSITIVITY_4X
      SENSITIVITY_2X
      SENSITIVITY_1X    // Least sensitive
  */
  sensor.setSensitivity(SENSITIVITY_4X);

  /* GET SENSITIVITY
      Returns the sensitivty multiplier for the
      current sensitivity settings.
  */
  Serial.print("Sensitivity Multiplier: ");
  Serial.print(sensor.getSensitivity());
  Serial.println("x");
}

void loop()
{
  if (sensor.isTouched() == true)
  {
    Serial.println("Touched!");
    while (sensor.isTouched() == true)
      delay(5);
  }
}
