/**
   @file RAK12008_CO2_FRC_ASC_STC31.ino
   @author rakwireless.com
   @brief Example Forced recalibration and Automatic self-calibration function to calibrate STC31.
   @version 0.1
   @date 2022-02-21
   @copyright Copyright (c) 2022
**/
#include <Wire.h>
#include "SparkFun_STC3x_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_STC3x

#define STC31_ADDRESS     0x2C

STC3x mySensor;

void setup()
{
  pinMode(WB_IO2,OUTPUT);
  digitalWrite(WB_IO2,HIGH);  //power on RAK12008.
  
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
  Serial.println(F("STC31 FRC ASC Example."));
  Wire.begin();

if (mySensor.begin(STC31_ADDRESS,Wire) == false)
  {
    while (1)
    {
      Serial.println("Sensor not detected. Please check wiring. Freezing...");
      delay(5000);
    }
  }

	/*
	 * We need to tell the sensor what binary gas and full range we are using
	 * Possible values are:
	 * STC3X_BINARY_GAS_CO2_N2_100   : Set binary gas to CO2 in N2.  Range: 0 to 100 vol%
	 * STC3X_BINARY_GAS_CO2_AIR_100  : Set binary gas to CO2 in Air. Range: 0 to 100 vol%
	 * STC3X_BINARY_GAS_CO2_N2_25    : Set binary gas to CO2 in N2.  Range: 0 to 25 vol%
	 * STC3X_BINARY_GAS_CO2_AIR_25   : Set binary gas to CO2 in Air. Range: 0 to 25 vol%
	 */
  if (mySensor.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_25) == false)
  {
    Serial.println(F("Could not set the binary gas! Freezing..."));
    while (1)
      ;
  }

  /*
   * Forced recalibration is used to improve the sensor output with a known reference value.
   * See the Field Calibration Guide for more details.
   * This command will trigger a concentration measurement and therefore it will take the same measurement time.
   */
  if (mySensor.forcedRecalibration(0.16) == true) // Force a recalibration with a concentration of 0.1%
  {
    Serial.println(F("Forced Recalibration was successful!"));
  }
  else
  {
    Serial.println(F("Forced Recalibration FAILED! Please check the debug messages for more details"));
  }

  /*
   * The sensor can run in automatic self-calibration mode.
   * This mode will enhance the accuracy for applications where the target gas is not present for the majority of the time.
   * See the Field Calibration Guide for more details.
   * This feature can be enabled or disabled by using the commands as shown below. The default state is disabled.
   * The automatic self-calibration is optimized for a gas concentration measurement interval of 1s.
   * Substantially different measurement intervals may decrease the self-calibration performance.
   */
  if (mySensor.enableAutomaticSelfCalibration() == true)
  {
    Serial.println(F("Automatic self calibration is enabled!"));
  }
  else
  {
    Serial.println(F("enableAutomaticSelfCalibration FAILED! Please check the debug messages for more details"));
  }
}

void loop()
{
  if (mySensor.measureGasConcentration()) // measureGasConcentration will return true when fresh data is available
  {
    Serial.print(F("CO2(%):"));
    Serial.print(mySensor.getCO2(), 2);

    Serial.print(F("\tTemperature(C):"));
    Serial.print(mySensor.getTemperature(), 2);

    Serial.println();
  }
  else
    Serial.print(F("."));

  delay(1000);
}
