/**
 * @file RAK12035_LowPower.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Low Power sensor usage
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
	Serial.printf("0x%02X", data);
	Serial.println();

	// Set the calibration values
	// Reading the saved calibration values from the sensor.
	sensor.get_dry_cal(&zero_val);
	sensor.get_wet_cal(&hundred_val);
	Serial.printf("Dry calibration value is %d\n", zero_val);
	Serial.printf("Wet calibration value is %d\n", hundred_val);
}

void loop()
{
	digitalWrite(LED_BLUE, HIGH);
	// Switch on the power supply of the sensor
	sensor.sensor_on();

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
	Serial.println(moisture);

	// Read temperature
	uint16_t temperature = 0;
	sensor.get_sensor_temperature(&temperature);
	Serial.print("Soil Moisture Temperatur: ");
	Serial.print(temperature / 10);
	Serial.println(" *C");

	Serial.println("Put the sensor into sleep mode for 2 minutes");
	// Shut down the sensor
	sensor.sensor_sleep();
	// Sleep for 2 minutes
	digitalWrite(LED_BLUE, LOW);
	delay(2 * 60 * 1000);
	Serial.println("Wake up and do another measurement");
}
