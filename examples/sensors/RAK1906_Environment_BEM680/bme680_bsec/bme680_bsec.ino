/**
 * @file bme680_bsec.ino
 * @author rakwireless.com
 * @brief Setup and read values from a BME680 environment sensor using the BOSCH BSEC library
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */
#include "bsec.h" //http://librarymanager/All#BSEC_BME680

// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;

String output;

// Entry point for the example
void setup(void)
{
	Serial.begin(115200);
	while (!Serial)
		delay(10);
	Wire.begin();

	iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);
	output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
	Serial.println(output);
	checkIaqSensorStatus();

	bsec_virtual_sensor_t sensorList[10] = {
		BSEC_OUTPUT_RAW_TEMPERATURE,
		BSEC_OUTPUT_RAW_PRESSURE,
		BSEC_OUTPUT_RAW_HUMIDITY,
		BSEC_OUTPUT_RAW_GAS,
		BSEC_OUTPUT_IAQ,
		BSEC_OUTPUT_STATIC_IAQ,
		BSEC_OUTPUT_CO2_EQUIVALENT,
		BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
		BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
	};

	iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
	checkIaqSensorStatus();

	// Print the header
	output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
	Serial.println(output);
}

// Function that is looped forever
void loop(void)
{
	unsigned long time_trigger = millis();
	if (iaqSensor.run())
	{ // If new data is available
		output = String(time_trigger);
		output += ", " + String(iaqSensor.rawTemperature);
		output += ", " + String(iaqSensor.pressure);
		output += ", " + String(iaqSensor.rawHumidity);
		output += ", " + String(iaqSensor.gasResistance);
		output += ", " + String(iaqSensor.iaq);
		output += ", " + String(iaqSensor.iaqAccuracy);
		output += ", " + String(iaqSensor.temperature);
		output += ", " + String(iaqSensor.humidity);
		output += ", " + String(iaqSensor.staticIaq);
		output += ", " + String(iaqSensor.co2Equivalent);
		output += ", " + String(iaqSensor.breathVocEquivalent);
		Serial.println(output);
	}
	else
	{
		checkIaqSensorStatus();
	}
}

// Helper function definitions
void checkIaqSensorStatus(void)
{
	if (iaqSensor.status != BSEC_OK)
	{
		if (iaqSensor.status < BSEC_OK)
		{
			output = "BSEC error code : " + String(iaqSensor.status);
			Serial.println(output);
			for (;;)
				errLeds(); /* Halt in case of failure */
		}
		else
		{
			output = "BSEC warning code : " + String(iaqSensor.status);
			Serial.println(output);
		}
	}

	if (iaqSensor.bme680Status != BME680_OK)
	{
		if (iaqSensor.bme680Status < BME680_OK)
		{
			output = "BME680 error code : " + String(iaqSensor.bme680Status);
			Serial.println(output);
			for (;;)
				errLeds(); /* Halt in case of failure */
		}
		else
		{
			output = "BME680 warning code : " + String(iaqSensor.bme680Status);
			Serial.println(output);
		}
	}
}

void errLeds(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);
	delay(100);
	digitalWrite(LED_BUILTIN, LOW);
	delay(100);
}
