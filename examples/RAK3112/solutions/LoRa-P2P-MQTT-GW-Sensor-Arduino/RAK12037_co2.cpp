/**
 * @file RAK12037_co2.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12037 CO2 gas sensor
 * @version 0.1
 * @date 2022-04-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <SparkFun_SCD30_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_SCD30

/** Sensor instance */
SCD30 scd30;

/**
 * @brief Initialize MQ2 gas sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12037(void)
{
	// Enable power
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH); // power on RAK12037

	Wire.begin();
	if (!scd30.begin(Wire))
	{
		MYLOG("SCD30", "SCD30 not found");
		digitalWrite(WB_IO2, LOW); // power down RAK12037
		return false;
	}

	//**************init SCD30 sensor *****************************************************
	// Change number of seconds between measurements: 2 to 1800 (30 minutes), stored in non-volatile memory of SCD30
	scd30.setMeasurementInterval(10);

	// Disable self calibration
	scd30.setAutoSelfCalibration(false);

	// Start the measurements
	scd30.beginMeasuring();

	return true;
}

/**
 * @brief Read CO2 sensor data
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_CO2_2, LPP_CHANNEL_CO2_Temp_2 and LPP_CHANNEL_CO2_HUMID_2
 *
 */
void read_rak12037(void)
{
	time_t start_time = millis();
	while (!scd30.dataAvailable())
	{
		MYLOG("SCD30", "Waiting for data");
		delay(500);
		if ((millis() - start_time) > 5000)
		{
			// timeout, no data available
			MYLOG("SCD30", "Timeout");
			return;
		}
	}

	uint16_t co2_reading = scd30.getCO2();
	float temp_reading = scd30.getTemperature();
	float humid_reading = scd30.getHumidity();

	MYLOG("SCD30", "CO2 level %dppm", co2_reading);
	MYLOG("SCD30", "Temperature %.2f", temp_reading);
	MYLOG("SCD30", "Humidity %.2f", humid_reading);

	g_solution_data.addConcentration(LPP_CHANNEL_CO2_2, co2_reading);
	g_solution_data.addTemperature(LPP_CHANNEL_CO2_Temp_2, temp_reading);
	g_solution_data.addRelativeHumidity(LPP_CHANNEL_CO2_HUMID_2, humid_reading);
}