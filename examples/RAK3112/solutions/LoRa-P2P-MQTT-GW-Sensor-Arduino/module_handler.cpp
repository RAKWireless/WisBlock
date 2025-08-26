/**
 * @file module-handler.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Find and handle WisBlock sensor modules
 * @version 0.1
 * @date 2022-02-02
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include "module_handler.h"

/**
 * @brief List of all supported WisBlock modules
 *
 */
sensors_t found_sensors[] = {
	// I2C address , I2C bus, found?
	{0x18, false}, //  0 ✔ RAK1904 accelerometer
	{0x44, false}, //  1 ✔ RAK1903 light sensor
	{0x42, false}, //  2 ✔ RAK12500 GNSS sensor
	{0x5c, false}, //  3 ✔ RAK1902 barometric pressure sensor
	{0x70, false}, //  4 ✔ RAK1901 temperature & humidity sensor
	{0x76, false}, //  5 ✔ RAK1906 environment sensor
	{0x20, false}, //  6 ✔ RAK12035 soil moisture sensor !! address conflict with RAK13003
	{0x10, false}, //  7 ✔ RAK12010 light sensor
	{0x51, false}, //  8 ✔ RAK12004 MQ2 CO2 gas sensor !! conflict with RAK15000
	{0x50, false}, //  9 ✔ RAK15000 EEPROM !! conflict with RAK12008
	{0x2C, false}, // 10 ✔ RAK12008 SCT31 CO2 gas sensor
	{0x55, false}, // 11 ✔ RAK12009 MQ3 Alcohol gas sensor
	{0x29, false}, // 12 ✔ RAK12014 Laser ToF sensor
	{0x52, false}, // 13 ✔ RAK12002 RTC module !! conflict with RAK15000
	{0x04, false}, // 14 ✔ RAK14003 LED bargraph module
	{0x59, false}, // 15 ✔ RAK12047 VOC sensor !! conflict with RAK13600, RAK13003, RAK5814
	{0x68, false}, // 16 ✔ RAK12025 Gyroscope address !! conflict with RAK1905
	{0x73, false}, // 17 ✔ RAK14008 Gesture sensor
	{0x3C, false}, // 18 ✔ RAK1921 OLED display
	{0x53, false}, // 19 ✔ RAK12019 LTR390 light sensor !! conflict with RAK15000
	{0x28, false}, // 20 ✔ RAK14002 Touch Button module
	{0x41, false}, // 21 ✔ RAK16000 DC current sensor
	{0x68, false}, // 22 ✔ RAK1905 MPU9250 9DOF sensor !! conflict with RAK12025
	{0x61, false}, // 23 ✔ RAK12037 CO2 sensor !! conflict with RAK16001
	{0x3A, false}, // 24 ✔ RAK12003 IR temperature sensor
	{0x68, false}, // 25 ✔ RAK12040 AMG8833 temperature array sensor
	{0x69, false}, // 26 ✔ RAK12034 BMX160 9DOF sensor
	{0x1D, false}, // 27 ✔ RAK12032 ADXL313 accelerometer
	{0x12, false}, // 28 ✔ RAK12039 PMSA003I particle matter sensor
	{0x55, false}, // 29 ✔ RAK12027 D7S seismic sensor
	{0x4A, false}, // 30 ✔ RAK12059 Water Level sensor
	{0x57, false}, // 31 RAK12012 MAX30102 heart rate sensor
	{0x54, false}, // 32 RAK12016 Flex sensor
	{0x47, false}, // 33 RAK13004 PWM expander module
	{0x38, false}, // 34 RAK14001 RGB LED module
	{0x5F, false}, // 35 RAK14004 Keypad interface
	{0x61, false}, // 36 RAK16001 ADC sensor !! conflict with RAK12037
	{0x59, false}, // 37 RAK13600 NFC !! conflict with RAK12047, RAK13600, RAK5814
	{0x59, false}, // 38 RAK16002 Coulomb sensor !! conflict with RAK13600, RAK12047, RAK5814
	{0x20, false}, // 39 RAK13003 IO expander module !! conflict with RAK12035
	{0x59, false}, // 40 ✔ RAK5814 ACC608 encryption module (limited I2C speed 100000) !! conflict with RAK12047, RAK13600, RAK13003
	{0x33, false}, // 41 ✔ RAK12052 MLX90640 IR array sensor
};

/**
 * @brief Scan both I2C bus for devices
 *
 */
void find_modules(void)
{
	// Scan the I2C interfaces for devices
	byte error;
	uint8_t num_dev = 0;

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);

	Wire.begin();
	// Some modules support only 100kHz
	// Wire.setClock(100000);
	for (byte address = 1; address < 127; address++)
	{
		if (address == 0x12)
		{
			// RAK12039 has extra GPIO for power control
			// On/Off control pin
			pinMode(WB_IO6, OUTPUT);
			// Sensor on
			digitalWrite(WB_IO6, HIGH);
			delay(500);
			time_t wait_sensor = millis();
			MYLOG("SCAN", "RAK12039 scan start %ld ms", millis());
			while (1)
			{
				delay(500);
				Wire.beginTransmission(address);
				error = Wire.endTransmission();
				if (error == 0)
				{
					MYLOG("SCAN", "RAK12039 answered at %ld ms", millis());
					break;
				}
				if ((millis() - wait_sensor) > 10000)
				{
					MYLOG("SCAN", "RAK12039 timeout after 10000 ms");
					pinMode(WB_IO6, INPUT);
					break;
				}
			}
		}
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0)
		{
			MYLOG("SCAN", "Found sensor at I2C1 0x%02X", address);
			for (uint8_t i = 0; i < sizeof(found_sensors) / sizeof(sensors_t); i++)
			{
				if (address == found_sensors[i].i2c_addr)
				{
					found_sensors[i].found_sensor = true;
					break;
				}
			}
			num_dev++;
		}
	}

	// Wire.setClock(100000); /// \todo Wire.setClock(400000);

	MYLOG("SCAN", "Found %d sensors", num_dev);
	for (uint8_t i = 0; i < sizeof(found_sensors) / sizeof(sensors_t); i++)
	{
		if (found_sensors[i].found_sensor)
		{
			MYLOG("SCAN", "ID %d addr %02X", i, found_sensors[i].i2c_addr);
		}
	}

	// Initialize the modules found
	if (found_sensors[TEMP_ID].found_sensor)
	{
		if (!init_rak1901())
		{
			found_sensors[TEMP_ID].found_sensor = false;
		}
	}

	if (found_sensors[PRESS_ID].found_sensor)
	{
		if (!init_rak1902())
		{
			found_sensors[PRESS_ID].found_sensor = false;
		}
	}

	if (found_sensors[LIGHT_ID].found_sensor)
	{
		if (init_rak1903())
		{
			snprintf(g_ble_dev_name, 9, "RAK_WEA");
		}
		else
		{
			found_sensors[LIGHT_ID].found_sensor = false;
		}
	}

	if (found_sensors[ENV_ID].found_sensor)
	{
		/*********************************************/
		/** Select between Bosch BSEC algorithm for  */
		/** IAQ index or simple T/H/P readings       */
		/*********************************************/
#if USE_BSEC == 1
		if (init_rak1906_bsec()) // !!! USING Bosch BSEC
#else
		if (init_rak1906()) // !!! USING SIMPLE READINGS
#endif
		{
			snprintf(g_ble_dev_name, 9, "RAK_ENV");
		}
		else
		{
			found_sensors[ENV_ID].found_sensor = false;
		}
	}

	if (found_sensors[RTC_ID].found_sensor)
	{
		if (!init_rak12002())
		{
			found_sensors[RTC_ID].found_sensor = false;
		}
	}

	if (found_sensors[MQ2_ID].found_sensor)
	{
		if (init_rak12004())
		{
			snprintf(g_ble_dev_name, 9, "RAK_GAS");
		}
		else
		{
			found_sensors[MQ2_ID].found_sensor = false;
		}
	}

	if (found_sensors[SCT31_ID].found_sensor)
	{
		if (init_rak12008())
		{
			snprintf(g_ble_dev_name, 9, "RAK_GAS");
		}
		else
		{
			found_sensors[SCT31_ID].found_sensor = false;
		}
	}

	if (found_sensors[LIGHT2_ID].found_sensor)
	{
		if (init_rak12010())
		{
			snprintf(g_ble_dev_name, 9, "RAK_WEA");
		}
		else
		{
			found_sensors[LIGHT2_ID].found_sensor = false;
		}
	}

	if (found_sensors[UVL_ID].found_sensor)
	{
		if (!init_rak12019())
		{
			found_sensors[UVL_ID].found_sensor = false;
		}
	}

	if (found_sensors[CO2_ID].found_sensor)
	{
		if (init_rak12037())
		{
			snprintf(g_ble_dev_name, 9, "RAK_GAS");
		}
		else
		{
			found_sensors[CO2_ID].found_sensor = false;
		}
	}

	if (found_sensors[PM_ID].found_sensor)
	{
		if (init_rak12039())
		{
			snprintf(g_ble_dev_name, 9, "RAK_ENV");
		}
		else
		{
			found_sensors[PM_ID].found_sensor = false;
		}
	}

	if (found_sensors[VOC_ID].found_sensor)
	{
		MYLOG("APP", "Initialize RAK12047");
		if (init_rak12047())
		{
			MYLOG("APP", "RAK12047 init success");
			snprintf(g_ble_dev_name, 9, "RAK_VOC");
		}
		else
		{
			found_sensors[VOC_ID].found_sensor = false;
		}
	}

	if (num_dev == 0)
	{
		// api_deinit_gpio(WB_IO2);
		Wire.end();
		// api_deinit_gpio(PIN_WIRE_SDA);
		// api_deinit_gpio(PIN_WIRE_SCL);
	}
}

/**
 * @brief AT command feedback about found modules
 *
 */
void announce_modules(bool just_list)
{
	if (!found_sensors[TEMP_ID].found_sensor)
	{
		// MYLOG("APP", "SHTC3 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1901 OK\n");
		if (!just_list)
		{
			read_rak1901();
		}
	}

	if (!found_sensors[PRESS_ID].found_sensor)
	{
		// MYLOG("APP", "LPS22HB error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1902 OK\n");
		if (!just_list)
		{
			read_rak1902();
		}
	}

	if (!found_sensors[LIGHT_ID].found_sensor)
	{
		// MYLOG("APP", "OPT3001 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1903 OK\n");
		if (!just_list)
		{
			read_rak1903();
		}
	}

	if (!found_sensors[ACC_ID].found_sensor)
	{
		// MYLOG("APP", "ACC error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1904 OK\n");
	}

	if (!found_sensors[MPU_ID].found_sensor)
	{
		// MYLOG("APP", "MPU error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1905 OK\n");
	}

	if (!found_sensors[ENV_ID].found_sensor)
	{
		// MYLOG("APP", "BME680 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK1906 OK\n");
	}

	if (!found_sensors[RTC_ID].found_sensor)
	{
		// MYLOG("APP", "RTC error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12002 OK\n");
		if (!just_list)
		{
			read_rak12002();
		}
	}

	if (!found_sensors[MQ2_ID].found_sensor)
	{
		// MYLOG("APP", "MQ2 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12004 OK\n");
		if (!just_list)
		{
			read_rak12004();
		}
	}

	if (!found_sensors[SCT31_ID].found_sensor)
	{
		// MYLOG("APP", "SCT31 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12008 OK\n");
		// read_rak12008();
	}

	if (!found_sensors[MQ3_ID].found_sensor)
	{
		// MYLOG("APP", "MQ3 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12009 OK\n");
		if (!just_list)
		{
			read_rak12009();
		}
	}

	if (!found_sensors[LIGHT2_ID].found_sensor)
	{
		// MYLOG("APP", "VEML7700 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12010 OK\n");
		if (!just_list)
		{
			read_rak12010();
		}
	}

	if (!found_sensors[UVL_ID].found_sensor)
	{
		// MYLOG("APP", "LTR390 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12019 OK\n");
		if (!just_list)
		{
			read_rak12019();
		}
	}

	if (!found_sensors[CO2_ID].found_sensor)
	{
		// MYLOG("APP", "SCD30 error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12037 OK\n");
		if (!just_list)
		{
			read_rak12037();
		}
	}

	if (!found_sensors[PM_ID].found_sensor)
	{
		// MYLOG("APP", "PMSA003I error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12039 OK\n");
		// read_rak12039();
	}

	if (!found_sensors[VOC_ID].found_sensor)
	{
		// MYLOG("APP", "SGP40 Sensor error");
		init_result = false;
	}
	else
	{
		AT_PRINTF("+EVT:RAK12047 OK\n");
	}
}

/**
 * @brief Read values from the found modules
 *
 */
void get_sensor_values(void)
{
	if (found_sensors[TEMP_ID].found_sensor)
	{
		// Read environment data
		read_rak1901();
	}
	// RAK1902 needs time to get correct value. Reading was already started and results will be gotten in app.cpp
	// if (found_sensors[PRESS_ID].found_sensor)
	// {
	// 	// Read environment data
	// 	read_rak1902();
	// }
	if (found_sensors[LIGHT_ID].found_sensor)
	{
		// Read environment data
		read_rak1903();
	}
	/*********************************************/
	/** Select between Bosch BSEC algorithm for  */
	/** IAQ index or simple T/H/P readings       */
	/*********************************************/
#if USE_BSEC == 1
	if (found_sensors[ENV_ID].found_sensor) // USING BOSCH BSEC
	{
		// Get last IAQ
		read_rak1906_bsec();
	}
#else
	// RAK1906 needs time to get correct value. Reading was already started and results will be gotten in app.cpp
	// if (found_sensors[ENV_ID].found_sensor)
	// {
	// 	// Start reading environment data
	// 	start_rak1906();
	// }
#endif
	if (found_sensors[MQ2_ID].found_sensor)
	{
		// Get the MQ2 sensor values
		read_rak12004();
	}
	// RAK12008 needs readings from temperature/humidity/pressure sensors. Readings will be gotten in app.cpp
	// if (found_sensors[SCT31_ID].found_sensor)
	// {
	// 	// Get the SCT31 sensor values
	// 	read_rak12008();
	// }
	if (found_sensors[MQ3_ID].found_sensor)
	{
		// Get the MQ3 sensor values
		read_rak12009();
	}
	if (found_sensors[CO2_ID].found_sensor)
	{
		// Get the CO2 sensor values
		read_rak12037();
	}
	if (found_sensors[LIGHT2_ID].found_sensor)
	{
		// Read environment data
		read_rak12010();
	}
	if (found_sensors[UVL_ID].found_sensor)
	{
		// Get the LTR390 sensor values
		read_rak12019();
	}
	if (found_sensors[PM_ID].found_sensor)
	{
		// Get the particle matter sensor values
		read_rak12039();
	}
	if (found_sensors[VOC_ID].found_sensor)
	{
		// Get the voc sensor values
		read_rak12047();
	}
}