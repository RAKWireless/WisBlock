/**
 * @file RAK1920_QWIIC_AirQuality_SGP30.ino
 * @author rakwireless.com
 * @brief Read temperature from a Sensirion SGP30 Indoor Air Quality Sensor on a QWIIC breakout board.
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
 * IO1 <-> P0.17 (Arduino GPIO number 17)
 * IO2 <-> P1.02 (Arduino GPIO number 34)
 * IO3 <-> P0.21 (Arduino GPIO number 21)
 * IO4 <-> P0.04 (Arduino GPIO number 4)
 * IO5 <-> P0.09 (Arduino GPIO number 9)
 * IO6 <-> P0.10 (Arduino GPIO number 10)
 * SW1 <-> P0.01 (Arduino GPIO number 1)
 */
/*
  Library for the Sensirion SGP30 Indoor Air Quality Sensor
  By: Ciara Jekel
  SparkFun Electronics
  Date: June 28th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
  SGP30 Datasheet: https://cdn.sparkfun.com/assets/c/0/a/2/e/Sensirion_Gas_Sensors_SGP30_Datasheet.pdf
  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14813
  This example reads the sensors calculated CO2 and TVOC values
*/

#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>

SGP30 mySensor; //create an object of the SGP30 class

void setup()
{
	Serial.begin(115200);
	while (!Serial)
		;
	Wire.begin();
	//Initialize sensor
	if (mySensor.begin() == false)
	{
		Serial.println("No SGP30 Detected. Check connections.");
	}
	//Initializes sensor for air quality readings
	//measureAirQuality should be called in one second increments after a call to initAirQuality
	mySensor.initAirQuality();
}

void loop()
{
	//First fifteen readings will be
	//CO2: 400 ppm  TVOC: 0 ppb
	delay(1000); //Wait 1 second
	//measure CO2 and TVOC levels
	mySensor.measureAirQuality();
	Serial.print("CO2: ");
	Serial.print(mySensor.CO2);
	Serial.print(" ppm\tTVOC: ");
	Serial.print(mySensor.TVOC);
	Serial.println(" ppb");
}
