/**
   @file RAK12008_CO2_PHTCompensation_STC31.ino
   @author rakwireless.com
   @brief Example Forced recalibration and Automatic self-calibration function to calibrate STC31.
   @version 0.1
   @date 2022-02-21
   @copyright Copyright (c) 2022
**/
#include <Wire.h>
#include "SparkFun_STC3x_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_STC3x

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
  Serial.println(F("STC31 P H T Compensation Example."));
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
		while (1)
		{
			Serial.println(F("Could not set the binary gas! Freezing..."));
			delay(5000);
		}
  }
  
  float temperature = 25; // 25℃
	Serial.print(F("Set Temperature "));
  if (mySensor.setTemperature(temperature) == false)
    Serial.print(F("not "));
  Serial.println(F("successful."));

  float RH = 25; // 25%RH
	Serial.print(F("Set Relative Humidity "));
  if (mySensor.setRelativeHumidity(20) == false)
    Serial.print(F("not "));
  Serial.println(F("successful."));

  uint16_t pressure = 840;  // 840 mbar
	Serial.print(F("Set Pressure "));
  if (mySensor.setPressure(840) == false)
    Serial.print(F("not "));
  Serial.println(F("successful."));
}

void loop()
{
  if (mySensor.measureGasConcentration())
  {
    Serial.print(F("CO2(%):"));
    Serial.print(mySensor.getCO2(), 2);

    Serial.println();
  }
  else
    Serial.print(F("."));

  delay(1000);
}
