/**
 * @file RAK1906_env.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief BME680 sensor functions
 * @version 0.1
 * @date 2021-05-29
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "app.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

// Forward declarations
void GetGasReference(void);
String CalculateIAQ(int score);
int GetHumidityScore(float current_humidity);
int GetGasScore(float current_gas);

/** BME680 instance for Wire */
Adafruit_BME680 bme(&Wire);

/** Last temperature read */
float _last_temp_rak1906 = 0;
/** Last humidity read */
float _last_humid_rak1906 = 0;
/** Last pressure read */
float _last_pressure_rak1906 = 0;

// Integrating Air Quality Index Calculation example
// Ref: https://github.com/robmarkcole/bme680-mqtt/blob/master/bme680-air-quality-RC.py
int humidity_score, gas_score;
float gas_reference = 2500;
float hum_reference = 40;
int getgasreference_count = 0;
int gas_lower_limit = 10000;  // Bad air quality limit
int gas_upper_limit = 300000; // Good air quality limit
float hum_weighting = 0.25;	  // so hum effect is 25% of the total air quality score
float gas_weighting = 0.75;	  // so gas effect is 75% of the total air quality score

/**
 * @brief Initialize the BME680 sensor
 *
 * @return true if sensor was found
 * @return false if sensor was not found
 */
bool init_rak1906(void)
{
	Wire.begin();

	if (!bme.begin(0x76))
	{
		MYLOG("BME", "Could not find a valid BME680 sensor, check wiring!");
		return false;
	}

	// Set up oversampling and filter initialization
	bme.setTemperatureOversampling(BME680_OS_8X);
	bme.setHumidityOversampling(BME680_OS_2X);
	bme.setPressureOversampling(BME680_OS_4X);
	bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
	bme.setGasHeater(320, 150); // 320*C for 150 ms
	// As we do not use the BSEC library here, the gas value is useless and just consumes battery. Better to switch it off
	// bme.setGasHeater(0, 0); // switch off

	// Burn-in BME680
	GetGasReference();

	return true;
}

/**
 * @brief Start sensing on the BME6860
 *
 */
void start_rak1906(void)
{
	MYLOG("BME", "Start BME reading");
	bme.beginReading();
}

/**
 * @brief Read environment data from BME680
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_HUMID_2, LPP_CHANNEL_TEMP_2,
 *     LPP_CHANNEL_PRESS_2 and LPP_CHANNEL_GAS_2
 *
 *
 * @return true if reading was successful
 * @return false if reading failed
 */
bool read_rak1906()
{
	time_t wait_start = millis();
	bool read_success = false;
	while ((millis() - wait_start) < 5000)
	{
		if (bme.endReading())
		{
			read_success = true;
			break;
		}
	}

	if (!read_success)
	{
		return false;
	}

	_last_temp_rak1906 = bme.temperature;
	_last_humid_rak1906 = bme.humidity;
	_last_pressure_rak1906 = (float)(bme.pressure) / 100.0;
	float _current_gas_rak1906 = bme.gas_resistance;

	humidity_score = GetHumidityScore(_last_humid_rak1906);
	gas_score = GetGasScore(_current_gas_rak1906);

	// Combine results for the final IAQ index value (0-100% where 100% is good quality air)
	float air_quality_score = humidity_score + gas_score;
	CalculateIAQ(air_quality_score);

	// Report the IAQ index in score representation instead of %
	air_quality_score = (100 - air_quality_score) * 5;

	g_solution_data.addRelativeHumidity(LPP_CHANNEL_HUMID_2, _last_humid_rak1906);
	g_solution_data.addTemperature(LPP_CHANNEL_TEMP_2, _last_temp_rak1906);
	g_solution_data.addBarometricPressure(LPP_CHANNEL_PRESS_2, _last_pressure_rak1906);
	g_solution_data.addAnalogInput(LPP_CHANNEL_GAS_2, air_quality_score);

#if MY_DEBUG > 0
	MYLOG("BME", "RH= %.2f T= %.2f", bme.humidity, bme.temperature);
	MYLOG("BME", "P= %.3f R= %.2f", (float)(bme.pressure) / 100.0, (float)(bme.gas_resistance) / 1000.0);
#endif

	return true;
}

/**
 * @brief Returns the latest values from the sensor
 *        or starts a new reading
 *
 * @param values array for temperature [0], humidity [1] and pressure [2]
 */
void get_rak1906_values(float *values)
{
	values[0] = _last_temp_rak1906;
	values[1] = _last_humid_rak1906;
	values[2] = _last_pressure_rak1906;
}

/**
 * @brief Calculate and return the altitude
 *        based on the barometric pressure
 *        Requires to have MSL set
 *
 * @return uint16_t altitude in cm
 */
uint16_t get_alt_rak1906(void)
{
	// Get latest values
	start_rak1906();
	delay(250);
	if (!read_rak1906())
	{
		return 0xFFFF;
	}

	MYLOG("BME", "Compute altitude\n");
	// pressure in HPa
	float pressure = bme.pressure / 100.0;
	MYLOG("BME", "P: %.2f MSL: %.2f", bme.pressure / 100.0, at_MSL);

	float A = pressure / at_MSL; // (1013.25) by default;
	float B = 1 / 5.25588;
	float C = pow(A, B);
	C = 1.0 - C;
	C = C / 0.0000225577;
	uint16_t new_val = C * 100;
	MYLOG("BME", "Altitude: %.2f m / %d cm", C, new_val);
	return new_val;
}

void GetGasReference()
{
	// Now run the sensor for a burn-in period, then use combination of relative humidity and gas resistance to estimate indoor air quality as a percentage.
	// Serial.println("Getting a new gas reference value");
	int readings = 10;
	for (int i = 1; i <= readings; i++)
	{
		bme.performReading();

		// read gas for 10 x 0.150mS = 1.5secs
		gas_reference += bme.readGas();
	}
	gas_reference = gas_reference / readings;
	// Serial.println("Gas Reference = "+String(gas_reference,3));
}

String CalculateIAQ(int score)
{
	String IAQ_text = "Unknown";
	score = (100 - score) * 5;
	if (score >= 301)
		IAQ_text = "Hazardous";
	else if (score >= 201 && score <= 300)
		IAQ_text = "Very Unhealthy";
	else if (score >= 176 && score <= 200)
		IAQ_text = "Unhealthy";
	else if (score >= 151 && score <= 175)
		IAQ_text = "Unhealthy for Sensitive Groups";
	else if (score >= 51 && score <= 150)
		IAQ_text = "Moderate";
	else if (score >= 00 && score <= 50)
		IAQ_text = "Good";
	MYLOG("BME", "IAQ Score [%d] = %s", score, IAQ_text.c_str());
	return IAQ_text;
}

int GetHumidityScore(float current_humidity)
{ // Calculate humidity contribution to IAQ index
	// float current_humidity = bme.readHumidity();
	float hum_offset = current_humidity - hum_reference;
	if (hum_offset > 0)
	{
		humidity_score = (100 - hum_reference - hum_offset) / (100 - hum_reference) * (hum_weighting * 100);
	}
	else
	{
		humidity_score = (hum_reference + hum_offset) / hum_reference * (hum_weighting * 100);
	}

	MYLOG("BME", "Humidity score: %d", humidity_score);
	return humidity_score;
}

int GetGasScore(float current_gas)
{
	// GetGasReference();

	// Calculate gas contribution to IAQ index
	// float current_gas = bme.gas_resistance;
	float gas_offset = gas_reference - current_gas;
	if (gas_offset > 0)
	{
		gas_score = (current_gas / gas_reference) * (100 - (hum_weighting * 100));
	}
	else
	{
		gas_score = 100 - (hum_weighting * 100);
	}

	MYLOG("BME", "Gas score: %d", gas_score);
	return gas_score;
}