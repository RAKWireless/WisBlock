/**
 * @file RAK12035_Change_Address.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Example how to change the I2C address of the sensor
 * @version 0.1
 * @date 2021-11-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>
#include <Wire.h>
#include "RAK12035_SoilMoisture.h" // Click here to get the library: http://librarymanager/All#RAK12035_SoilMoisture

/** Sensor */
RAK12035 sensor;

/** Set these two values after the sensor calibration was done */
uint16_t zero_val = 73;
uint16_t hundred_val = 250;

void setup()
{
	// Initialize the built in LED
	pinMode(LED_GREEN, OUTPUT);
	digitalWrite(LED_GREEN, LOW);

	// Initialize the connection status LED
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, HIGH);

	// Initialize Serial for debug output
	Serial.begin(115200);

	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	Wire.begin();

	// Initialize sensor
	sensor.begin();

	// Get sensor firmware version
	uint8_t data = 0;
	sensor.get_sensor_version(&data);
	Serial.print("Sensor Firmware version: ");
	Serial.println(data, HEX);
	Serial.println();

	// Set the calibration values
	// Reading the saved calibration values from the sensor.
	sensor.get_dry_cal(&zero_val);
	sensor.get_wet_cal(&hundred_val);
	Serial.printf("Dry calibration value is %d\n", zero_val);
	Serial.printf("Wet calibration value is %d\n", hundred_val);

	// Get the I2C address from the sensor
	uint8_t addr = sensor.get_sensor_addr();
	sensor.get_sensor_addr();
	Serial.print("I2C address is : ");
	Serial.println(addr, HEX);
	delay(1000);

	// Set the new I2C address of the sensor
	uint8_t addr2 = 0x21;
	if (!sensor.set_sensor_addr(addr2))
	{
		Serial.println("New sensor address not accepted");
	}
	else
	{
		// Tell the sensor class the new sensor address
		sensor.set_i2c_addr(addr2);

		// Check if the new sensor address works
		sensor.get_sensor_version(&data);
		Serial.println("Reading firmware version from the new sensor I2C address");
		Serial.print("Sensor Firmware version: ");
		Serial.println(data, HEX);
		Serial.println();
	}
}

void loop()
{
	// Read capacitance
	uint16_t capacitance = 0;
	sensor.get_sensor_capacitance(&capacitance);
	Serial.print("Soil Moisture Capacitance: ");
	Serial.println(capacitance);

	// Read moisture in %
	// after calibration, we get the Capacitance in air and in water, like zero_val and B. zero_val means 0% and B means 100%.
	// So the humidity is humidity =  (Capacitance-A) / ((B-A)/100.0)
	uint8_t moisture = 0;
	sensor.get_sensor_moisture(&moisture);
	Serial.print("Soil Moisture humidity(%): ");
	Serial.print(moisture);

	// Read temperature
	uint16_t temperature = 0;
	sensor.get_sensor_temperature(&temperature);
	Serial.print("Soil Moisture Temperatur: ");
	Serial.print(temperature / 10);
	Serial.println(" *C");
	delay(1000);
}
