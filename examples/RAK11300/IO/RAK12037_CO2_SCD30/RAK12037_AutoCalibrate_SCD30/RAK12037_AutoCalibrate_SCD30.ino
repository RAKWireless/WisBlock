/**
   @file RAK12037_AutoCalibrate_SCD30.ino
   @author rakwireless.com
   @brief SCD30 Automatic self-calibration(ASC) Example.
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

	Serial.println("SCD30 Automatic self-calibration Example.");
	
	Wire.begin();

	//Start sensor using the Wire port and enable the auto-calibration (ASC)
	if (airSensor.begin(Wire, true) == false)
	{
		Serial.println("Air sensor not detected. Please check wiring. Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	Serial.print("Automatic self-calibration set to：");
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
