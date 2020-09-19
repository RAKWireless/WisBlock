/**
 * @file RAK1904_Accelerate_LIS3DH.ino
 * @author rakwireless.com
 * @brief Setup and read values from a LIS3DH acceleration sensor
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
#include "SparkFunLIS3DH.h" //http://librarymanager/All#SparkFun-LIS3DH
#include <Wire.h>

LIS3DH SensorTwo(I2C_MODE, 0x18);

void setup()
{
	// Setup usb
	Serial.begin(115200);
	while (!Serial);

	if (SensorTwo.begin() != 0)
	{
		Serial.println("Problem starting the sensor at 0x18.");
	}
	else
	{
		Serial.println("Sensor at 0x18 started.");
	}
	Serial.println("enter !");
}

void loop()
{
	lis3dh_get();
	delay(1000);
}

void lis3dh_get()
{
	// read the sensor value
	uint8_t cnt = 0;

	Serial.print(" X(g) = ");
	Serial.println(SensorTwo.readFloatAccelX(), 4);
	Serial.print(" Y(g) = ");
	Serial.println(SensorTwo.readFloatAccelY(), 4);
	Serial.print(" Z(g)= ");
	Serial.println(SensorTwo.readFloatAccelZ(), 4);
}
