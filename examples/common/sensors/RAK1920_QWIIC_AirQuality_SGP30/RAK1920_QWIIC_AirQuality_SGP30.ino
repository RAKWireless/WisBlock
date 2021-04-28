/**
   @file RAK1920_QWIIC_AirQuality_SGP30.ino
   @author rakwireless.com
   @brief Setup and read values from a AirQuality_SGP30 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>

SGP30 g_mySensor; //create an object of the SGP30 class

void setup()
{
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
	Wire.begin();
	//Initialize sensor
	if (g_mySensor.begin() == false)
	{
		Serial.println("No SGP30 Detected. Check connections.");
	}
	//Initializes sensor for air quality readings
	//measureAirQuality should be called in one second increments after a call to initAirQuality
	g_mySensor.initAirQuality();
}

void loop()
{
	//First fifteen readings will be
	//CO2: 400 ppm  TVOC: 0 ppb
	delay(1000); //Wait 1 second
	//measure CO2 and TVOC levels
	g_mySensor.measureAirQuality();
	Serial.print("CO2: ");
	Serial.print(g_mySensor.CO2);
	Serial.print(" ppm\tTVOC: ");
	Serial.print(g_mySensor.TVOC);
	Serial.println(" ppb");
}
