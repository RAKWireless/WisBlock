/**
   @file RAK12037_GetSettings_SCD30.ino
   @author rakwireless.com
   @brief  SCD30 get setting information example.
   @version 0.1
   @date 2022-1-18
   @copyright Copyright (c) 2022
**/

#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SCD30

SCD30 airSensor;

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  
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
	Serial.println("SCD30 Get Settings Example.");
	
	Wire.begin();

	// Start sensor using the Wire port, but disable the auto-calibration
	if (airSensor.begin(Wire, false) == false)
	{
		Serial.println("Air sensor not detected. Please check wiring. Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	uint16_t settingVal; // The settings will be returned in settingVal

	if (airSensor.getForcedRecalibration(&settingVal) == true) // Get the setting
	{
		Serial.print("Forced recalibration factor (ppm) is ");
		Serial.println(settingVal);
	}
	else
	{
		Serial.print("getForcedRecalibration failed! Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	if (airSensor.getMeasurementInterval(&settingVal) == true) // Get the setting
	{
		Serial.print("Measurement interval (s) is ");
		Serial.println(settingVal);
	}
	else
	{
		Serial.print("getMeasurementInterval failed! Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	if (airSensor.getTemperatureOffset(&settingVal) == true) // Get the setting
	{
		Serial.print("Temperature offset (C) is ");
		Serial.println(((float)settingVal) / 100.0, 2);
	}
	else
	{
		Serial.print("getTemperatureOffset failed! Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	if (airSensor.getAltitudeCompensation(&settingVal) == true) // Get the setting
	{
		Serial.print("Altitude offset (m) is ");
		Serial.println(settingVal);
	}
	else
	{
		Serial.print("getAltitudeCompensation failed! Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	if (airSensor.getFirmwareVersion(&settingVal) == true) // Get the setting
	{
		Serial.print("Firmware version is 0x");
		Serial.println(settingVal, HEX);
	}
	else
	{
		Serial.print("getFirmwareVersion! Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	Serial.print("Auto calibration set to ");
	if (airSensor.getAutoSelfCalibration() == true)
		Serial.println("true");
	else
		Serial.println("false");
}

void loop()
{
	if (airSensor.dataAvailable())
	{
		Serial.print("co2(ppm):");
		Serial.print(airSensor.getCO2());

		Serial.print(" temp(C):");
		Serial.print(airSensor.getTemperature(), 1);

		Serial.print(" humidity(%):");
		Serial.print(airSensor.getHumidity(), 1);

		Serial.println();
	}
	else
		Serial.println("Waiting for new data");

	delay(3000);
}
