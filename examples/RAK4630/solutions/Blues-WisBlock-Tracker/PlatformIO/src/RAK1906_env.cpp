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

#include "main.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

/** BME680 instance for Wire */
Adafruit_BME680 bme(&Wire);

/** Last temperature read */
float _last_temp_rak1906 = 0;
/** Last humidity read */
float _last_humid_rak1906 = 0;
/** Last pressure read */
float _last_pressure_rak1906 = 0;

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
	// bme.setGasHeater(320, 150); // 320*C for 150 ms
	// As we do not use the BSEC library here, the gas value is useless and just consumes battery. Better to switch it off
	bme.setGasHeater(0, 0); // switch off

	return true;
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
	MYLOG("BME", "Start BME reading");
	bme.beginReading();
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
		MYLOG("BME", "BME timeout");
		return false;
	}

	_last_temp_rak1906 = bme.temperature;
	_last_humid_rak1906 = bme.humidity;
	_last_pressure_rak1906 = (float)(bme.pressure) / 100.0;

	g_solution_data.addRelativeHumidity(LPP_CHANNEL_HUMID_2, _last_humid_rak1906);
	g_solution_data.addTemperature(LPP_CHANNEL_TEMP_2, _last_temp_rak1906);
	g_solution_data.addBarometricPressure(LPP_CHANNEL_PRESS_2, _last_pressure_rak1906);

#if MY_DEBUG > 0
	MYLOG("BME", "RH= %.2f T= %.2f P= %.3f", bme.humidity, bme.temperature, (float)(bme.pressure) / 100.0);
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
