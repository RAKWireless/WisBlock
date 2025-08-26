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
#ifdef RAK1906_BSEC_H
#include "bsec.h"

bool check_rak1906_status(void);

/* Configure the BSEC library with information about the sensor
	18v/33v = Voltage at Vdd. 1.8V or 3.3V
	3s/300s = BSEC operating mode, BSEC_SAMPLE_RATE_LP or BSEC_SAMPLE_RATE_ULP
	4d/28d = Operating age of the sensor in days
	generic_18v_3s_4d
	generic_18v_3s_28d
	generic_18v_300s_4d
	generic_18v_300s_28d
	generic_33v_3s_4d
	generic_33v_3s_28d
	generic_33v_300s_4d
	generic_33v_300s_28d
*/
const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_300s_4d/bsec_iaq.txt"
};

/** BSEC instance for BME680 */
Bsec iaqSensor;

/** Timer for VOC measurement */
Ticker bsec_read_timer;

/** Last temperature read */
volatile float _last_temp_rak1906_bsec = 0;
/** Last humidity read */
volatile float _last_humid_rak1906_bsec = 0;
/** Last pressure read */
volatile float _last_pressure_rak1906_bsec = 0;
/** Last IAQ index read */
volatile float _last_iaq_rak1906_bsec = 0;

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

/**
 * @brief Timer callback to wakeup the loop with the BSEQ_REQ event
 *
 * @param unused
 */
void bsec_read_wakeup(void)
{
	api_wake_loop(BSEC_REQ);
}

/**
 * @brief Initialize the BME680 sensor
 *
 * @return true if sensor was found
 * @return false if sensor was not found
 */
bool init_rak1906_bsec(void)
{
	Wire.begin();

	iaqSensor.begin(BME680_I2C_ADDR_PRIMARY, Wire);

	MYLOG("BSEC", "BSEC library version %d.%d.%d.%d",
		  iaqSensor.version.major, iaqSensor.version.minor, iaqSensor.version.major_bugfix, iaqSensor.version.minor_bugfix);
	if (!check_rak1906_status())
	{
		MYLOG("BSEC", "Status error");
		return false;
	}

	iaqSensor.setConfig(bsec_config_iaq);

	iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_ULP);

	if (!check_rak1906_status())
	{
		MYLOG("BSEC", "Status error");
		return false;
	}

	// Set BSEC reading interval to 150 seconds
	bsec_read_timer.attach_ms(150000, bsec_read_wakeup);
	do_read_rak1906_bsec();

	return check_rak1906_status();
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
bool read_rak1906_bsec()
{
	// Reading and calculating the IAQ is done in a background task, just get the values and add them to the payload
	g_solution_data.addRelativeHumidity(LPP_CHANNEL_HUMID_2, _last_humid_rak1906_bsec);
	g_solution_data.addTemperature(LPP_CHANNEL_TEMP_2, _last_temp_rak1906_bsec);
	g_solution_data.addBarometricPressure(LPP_CHANNEL_PRESS_2, _last_pressure_rak1906_bsec);
	g_solution_data.addAnalogInput(LPP_CHANNEL_GAS_2, _last_iaq_rak1906_bsec);

#if MY_DEBUG > 0
	MYLOG("BSEC", "RH= %.2f T= %.2f", _last_humid_rak1906_bsec, _last_temp_rak1906_bsec);
	MYLOG("BSEC", "P= %.3f IAQ= %.2f", _last_pressure_rak1906_bsec, _last_iaq_rak1906_bsec);
#endif

	return true;
}

/**
 * @brief Returns the latest values from the sensor
 *        or starts a new reading
 *
 * @param values array for temperature [0], humidity [1] and pressure [2]
 */
void get_rak1906_bsec_values(float *values)
{
	values[0] = _last_temp_rak1906_bsec;
	values[1] = _last_humid_rak1906_bsec;
	values[2] = _last_pressure_rak1906_bsec;
}

/**
 * @brief Check BSEC and sensor status
 *
 * @return true if BSEC and sensor status returns no problem
 * @return false if BSEC or sensor status returns a warning or an error
 */
bool check_rak1906_status(void)
{
	if (iaqSensor.status != BSEC_OK)
	{
		if (iaqSensor.status < BSEC_OK)
		{
			MYLOG("BSEC", "BSEC error code : %d", iaqSensor.status);
		}
		else
		{
			MYLOG("BSEC", "BSEC warning code : %d", iaqSensor.status);
		}
		return false;
	}

	if (iaqSensor.bme680Status != BME680_OK)
	{
		if (iaqSensor.bme680Status < BME680_OK)
		{
			MYLOG("BSEC", "BME680 error code : %d", iaqSensor.bme680Status);
		}
		else
		{
			MYLOG("BSEC", "BME680 warning code : %d", iaqSensor.bme680Status);
		}
		return false;
	}
	return true;
}

/**
 * @brief Call BSEC algorithm to read data and calculate IAQ
 *
 * @return true if sensor status is ok
 * @return false if sensor status returns warning or error
 */
bool do_read_rak1906_bsec(void)
{
	MYLOG("BSEC", "Start reading BME680");
	if (iaqSensor.run())
	{
		_last_temp_rak1906_bsec = iaqSensor.temperature;
		_last_humid_rak1906_bsec = iaqSensor.humidity;
		_last_pressure_rak1906_bsec = iaqSensor.pressure / 100;
		_last_iaq_rak1906_bsec = iaqSensor.iaq;

#if MY_DEBUG > 0
		MYLOG("BSEC", "RH= %.2f T= %.2f", _last_humid_rak1906_bsec, _last_temp_rak1906_bsec);
		MYLOG("BSEC", "P= %.3f IAQ= %.2f", _last_pressure_rak1906_bsec, _last_iaq_rak1906_bsec);
#endif
	}
	else
	{
		MYLOG("BSEC", "No data available");
	}
	return check_rak1906_status();
}

/**
 * @brief Calculate and return the altitude
 *        based on the barometric pressure
 *        Requires to have MSL set
 *
 * @return uint16_t altitude in cm
 */
uint16_t get_alt_rak1906_bsec(void)
{
	MYLOG("BSEC", "Compute altitude\n");

	MYLOG("BSEC", "P: %.2f MSL: %.2f", _last_pressure_rak1906_bsec, at_MSL);

	float A = _last_pressure_rak1906_bsec / at_MSL; // (1013.25) by default;
	float B = 1 / 5.25588;
	float C = pow(A, B);
	C = 1.0 - C;
	C = C / 0.0000225577;
	uint16_t new_val = C * 100;
	MYLOG("BME", "Altitude: %.2f m / %d cm", C, new_val);
	return new_val;
}
#endif // RAK1906_BSEC_H