/**
 * @file RAK12035_Calibration.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Example for sensor calibration
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

	// Because each sensor has differences, it is required to calibrate the sensors
	// First get the values of a sensor in dry air
	Serial.println("Sensor Calibration");
	Serial.println("Keep the sensor in a dry environment (air) and press ENTER to start the first calibration");
	Serial.println("The calibration steps takes some time as an average is calculated out of 100 sensor readings.");

	Serial.println("\nPress ENTER when ready.");

	while (!Serial.available())
	{
		delay(200);
	}
	Serial.println("Start dry calibration.");
	while (Serial.available())
	{
		Serial.read();
	}

	uint16_t new_reading = 0;
	uint16_t new_value = 0;
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, HIGH);

	sensor.get_sensor_capacitance(&new_value);
	for (int readings = 0; readings < 100; readings++)
	{
		sensor.get_sensor_capacitance(&new_reading);
		new_value += new_reading;
		new_value = new_value / 2;
		delay(250);
		digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
	}
	// Save the result
	zero_val = new_value;
	Serial.print("Calibration in dry air finished, average capacitance is ");
	Serial.println(new_value);
	Serial.println("=======================================");

	Serial.println("Now submerge the sensor into water up to the white line on the sensor and press ENTER to start the second calibration");
	Serial.println("The calibration steps takes some time as an average is calculated out of 100 sensor readings.");

	Serial.println("\nPress ENTER when ready.");

	while (!Serial.available())
	{
		delay(200);
	}
	Serial.println("Start wet calibration.");
	while (Serial.available())
	{
		Serial.read();
	}

	new_value = 0;
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, HIGH);

	sensor.get_sensor_capacitance(&new_value);
	for (int readings = 0; readings < 100; readings++)
	{
		sensor.get_sensor_capacitance(&new_reading);
		new_value += new_reading;
		new_value = new_value / 2;
		delay(250);
		digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
	}
	// Save the result
	hundred_val = new_value;
	Serial.print("Calibration in water finished, average capacitance is ");
	Serial.println(new_value);
	Serial.println("=======================================");
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);

	/** Here we are saving the calibration values in the sensor itself */
	sensor.set_dry_cal(zero_val);
	sensor.set_wet_cal(hundred_val);
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
	Serial.println(moisture);

	// Read temperature
	uint16_t temperature = 0;
	sensor.get_sensor_temperature(&temperature);
	Serial.print("Soil Moisture Temperatur: ");
	Serial.print(temperature / 10);
	Serial.println(" *C");
	Serial.println("===========================");
	delay(1000);
}
