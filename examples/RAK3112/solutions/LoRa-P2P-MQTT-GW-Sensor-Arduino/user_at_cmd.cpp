/**
 * @file user_at_cmd.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Handle user defined AT commands
 * @version 0.3
 * @date 2022-01-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "app.h"
#include <Preferences.h>
/** ESP32 preferences */
Preferences esp32_prefs;

/** Flag for sleep activated */
bool g_device_sleep = false;

/**
 * @brief Returns the current WiFi and MQTT settings
 *
 * @return int
 */
static int at_query_wifi()
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%s:%s:%s:%s:%s:%s:%s", g_ssid_prim.c_str(), g_pw_prim.c_str(), g_ssid_sec.c_str(), g_pw_sec.c_str(),
			 g_mqtt_server, g_mqtt_user, g_mqtt_pw);
	Serial.printf("%s\r\n", g_at_query_buf);
	return 0;
}

/**
 * @brief Command to set the custom variable
 *
 * @param str the new value for the variable without the AT part
 * @return int 0 if the command was succesfull, 5 if the parameter was wrong
 */
static int at_exec_wifi(char *str)
{
	char *param;

	param = strtok(str, ":");

	if (param != NULL)
	{
		/* Check WiFi AP 1 */
		g_ssid_prim = String(param);

		/* Check AP 1 PW */
		param = strtok(NULL, ":");
		if (param != NULL)
		{
			g_pw_prim = String(param);

			/** Check WiFi AP 2 */
			param = strtok(NULL, ":");
			if (param != NULL)
			{
				g_ssid_sec = String(param);

				/* Check AP 2 PW */
				param = strtok(NULL, ":");
				if (param != NULL)
				{
					g_pw_sec = String(param);

					// Check MQTT URL
					param = strtok(NULL, ":");
					if (param != NULL)
					{
						snprintf(g_mqtt_server, 128, param);

						// Check MQTT user
						param = strtok(NULL, ":");
						if (param != NULL)
						{
							snprintf(g_mqtt_user, 128, param);

							// Check MQTT password
							param = strtok(NULL, ":");
							if (param != NULL)
							{
								snprintf(g_mqtt_pw, 128, param);

								// // Check MQTT publish path
								// param = strtok(NULL, ":");
								// if (param != NULL)
								{
									snprintf(g_mqtt_pw, 128, param);
									// Save new settings
									Preferences preferences;
									preferences.begin("WiFiCred", false);
									preferences.putString("g_ssid_prim", g_ssid_prim);
									preferences.putString("g_ssid_sec", g_ssid_sec);
									preferences.putString("g_pw_prim", g_pw_prim);
									preferences.putString("g_pw_sec", g_pw_sec);
									preferences.putBool("valid", true);
									preferences.end();
									preferences.begin("MQTTCred", false);
									preferences.putString("g_mqtt_url", g_mqtt_server);
									preferences.putString("g_mqtt_user", g_mqtt_user);
									preferences.putString("g_mqtt_pw", g_mqtt_pw);
									preferences.putBool("valid", true);
									preferences.end();

									MYLOG("AT", "WiFi 1 >%s<", g_ssid_prim.c_str());
									MYLOG("AT", "WiFi 1 PW >%s<", g_pw_prim.c_str());
									MYLOG("AT", "WiFi 2 >%s<", g_ssid_sec.c_str());
									MYLOG("AT", "WiFi 2 PW >%s<", g_pw_sec.c_str());
									MYLOG("AT", "MQTT PW >%s<", g_mqtt_pw);
									MYLOG("AT", "MQTT Name >%s<", g_mqtt_user);
									MYLOG("AT", "MQTT URL >%s<", g_mqtt_server);

									return AT_SUCCESS;
								}
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

/*****************************************
 * Query modules AT commands
 *****************************************/

/**
 * @brief Query found modules
 *
 * @return int 0
 */
int at_query_modules(void)
{
	announce_modules(true);
	return AT_CB_PRINT;
}

/**
 * @brief Goto sleep
 *
 * @return int 0
 */
static int at_sleep(void)
{
	// Switch off module power
	digitalWrite(WB_IO2, LOW);

	// Cancel automatic sending
	api_timer_stop();

	// Put radio into sleep
	Radio.Sleep();

	// set sleep flag
	g_device_sleep = true;
	return 0;
}

/**
 * @brief Wakeup
 *
 * @return int 0
 */
int at_wake(void)
{
	MYLOG("USR_AT", "Wakeup");
	// Switch off module power
	digitalWrite(WB_IO2, HIGH);

	// Cancel automatic sending
	api_timer_restart(g_lorawan_settings.send_repeat_time);

	// remove sleep flag
	g_device_sleep = false;
	return 0;
}

/*****************************************
 * RTC AT commands
 *****************************************/

/**
 * @brief Set RTC time
 *
 * @param str time as string, format <year>:<month>:<date>:<hour>:<minute>
 * @return int 0 if successful, otherwise error value
 */
static int at_set_rtc(char *str)
{
	if (found_sensors[RTC_ID].found_sensor)
	{
		uint16_t year;
		uint8_t month;
		uint8_t date;
		uint8_t hour;
		uint8_t minute;

		char *param;

		param = strtok(str, ":");

		// year:month:date:hour:minute

		if (param != NULL)
		{
			/* Check year */
			year = strtoul(param, NULL, 0);

			if (year > 3000)
			{
				return AT_ERRNO_PARA_VAL;
			}

			/* Check month */
			param = strtok(NULL, ":");
			if (param != NULL)
			{
				month = strtoul(param, NULL, 0);

				if ((month < 1) || (month > 12))
				{
					return AT_ERRNO_PARA_VAL;
				}

				// Check day
				param = strtok(NULL, ":");
				if (param != NULL)
				{
					date = strtoul(param, NULL, 0);

					if ((date < 1) || (date > 31))
					{
						return AT_ERRNO_PARA_VAL;
					}

					// Check hour
					param = strtok(NULL, ":");
					if (param != NULL)
					{
						hour = strtoul(param, NULL, 0);

						if (hour > 24)
						{
							return AT_ERRNO_PARA_VAL;
						}

						// Check minute
						param = strtok(NULL, ":");
						if (param != NULL)
						{
							minute = strtoul(param, NULL, 0);

							if (minute > 59)
							{
								return AT_ERRNO_PARA_VAL;
							}

							set_rak12002(year, month, date, hour, minute);

							return 0;
						}
					}
				}
			}
		}
		return AT_ERRNO_PARA_NUM;
	}
	return AT_ERRNO_NOSUPP;
}

/**
 * @brief Get RTC time
 *
 * @return int 0
 */
static int at_query_rtc(void)
{
	if (found_sensors[RTC_ID].found_sensor)
	{
		// Get date/time from the RTC
		read_rak12002();
		// AT_PRINTF("%d.%02d.%02d %d:%02d:%02d", g_date_time.year, g_date_time.month, g_date_time.date, g_date_time.hour, g_date_time.minute, g_date_time.second);
		snprintf(g_at_query_buf, ATQUERY_SIZE, "%d.%02d.%02d %d:%02d:%02d", g_date_time.year, g_date_time.month, g_date_time.date, g_date_time.hour, g_date_time.minute, g_date_time.second);

		return 0;
	}
	return AT_ERRNO_NOSUPP;
}

/*****************************************
 * Altitude AT commands
 *****************************************/

/**
 * @brief Get altitude
 * @author kongduino
 *
 * @return int 0
 */
static int at_query_alt()
{
	if ((found_sensors[PRESS_ID].found_sensor) || (found_sensors[ENV_ID].found_sensor))
	{
		uint16_t result;
		if (found_sensors[PRESS_ID].found_sensor)
		{
			result = get_alt_rak1902();
			if (result == 0xFFFF)
			{
				return AT_ERRNO_EXEC_FAIL;
			}
			// AT_PRINTF("Altitude RAK1902: %d cm\r\n", result);
			snprintf(g_at_query_buf, ATQUERY_SIZE, "%dcm", result);
		}
		if (found_sensors[ENV_ID].found_sensor)
		{
#if USE_BSEC == 0
			result = get_alt_rak1906();
#else
			result = get_alt_rak1906_bsec();
#endif
			if (result == 0xFFFF)
			{
				return AT_ERRNO_EXEC_FAIL;
			}
			// AT_PRINTF("Altitude RAK1906: %d cm\r\n", result);
			snprintf(g_at_query_buf, ATQUERY_SIZE, "%dcm", result);
		}
		return 0;
	}
	return AT_ERRNO_NOSUPP;
}

/** Mean Sea Level Pressure */
float at_MSL = 1013.25;

/**
 * @brief Query the current MSL value
 *
 * @return int 0
 */
static int at_query_msl()
{
	if ((found_sensors[PRESS_ID].found_sensor) || (found_sensors[ENV_ID].found_sensor))
	{
		// AT_PRINTF("MSL: %d\r\n", at_MSL * 100);
		snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", at_MSL * 100);
		return 0;
	}
	return AT_ERRNO_NOSUPP;
}

/**
 * @brief Set MSL
 * @author kongduino
 *
 * @return int 0
 */
static int at_set_msl(char *str)
{
	if ((found_sensors[PRESS_ID].found_sensor) || (found_sensors[ENV_ID].found_sensor))
	{
		long v = strtol(str, NULL, 0);
		if ((v < 84000) || (v > 105000))
		{
			// in Pa, ie default 101325
			return AT_ERRNO_PARA_VAL;
		}
		at_MSL = v / 100.0;
		return 0;
	}
	return AT_ERRNO_NOSUPP;
}

/*****************************************
 * Battery check AT commands
 *****************************************/

/**
 * @brief Enable/Disable battery check
 *
 * @param str
 * @return int
 */
static int at_set_batt_check(char *str)
{
	long check_bat_request = strtol(str, NULL, 0);
	if (check_bat_request == 1)
	{
		battery_check_enabled = true;
		save_batt_settings(battery_check_enabled);
	}
	else if (check_bat_request == 0)
	{
		battery_check_enabled = false;
		save_batt_settings(battery_check_enabled);
	}
	else
	{
		return AT_ERRNO_PARA_VAL;
	}
	return 0;
}

/**
 * @brief Enable/Disable battery check
 *
 * @return int 0
 */
static int at_query_batt_check(void)
{
	// AT_PRINTF("Battery check is %s", battery_check_enabled ? "enabled" : "disabled");
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%s", battery_check_enabled ? "enabled" : "disabled");
	return 0;
}

/**
 * @brief Read saved setting for precision and packet format
 *
 */
void read_batt_settings(void)
{
	esp32_prefs.begin("bat", false);
	battery_check_enabled = esp32_prefs.getBool("bat", false);
	esp32_prefs.end();

	save_batt_settings(battery_check_enabled);
}

/**
 * @brief Save the GPS settings
 *
 */
void save_batt_settings(bool check_batt_enables)
{
	esp32_prefs.begin("bat", false);
	esp32_prefs.putBool("bat", battery_check_enabled);
	esp32_prefs.end();
}

/** Number of user defined AT commands */
extern uint8_t g_user_at_cmd_num;

/** Pointer to the combined user AT command structure */
atcmd_t *g_user_at_cmd_list;

atcmd_t g_user_at_cmd_new_list[] = {
	/*|    CMD    |     AT+CMD?      |    AT+CMD=?    |  AT+CMD=value |  AT+CMD  | Permissions |*/
	// Module commands
	{"+MOD", "List all connected I2C devices", at_query_modules, NULL, NULL, "R"},
	{"+RTC", "Get/Set RTC time and date", at_query_rtc, at_set_rtc, at_query_rtc, "RW"},
	{"+ALT", "Get Altitude", at_query_alt, NULL, at_query_alt, "R"},
	{"+MSL", "Get/Set MSL value", at_query_msl, at_set_msl, at_query_msl, "RW"},
	{"+BATCHK", "Enable/Disable the battery charge check", at_query_batt_check, at_set_batt_check, at_query_batt_check, "RW"},
	{"+WIFI", "Set/Get the WiFi and MQTT settings", at_query_wifi, at_exec_wifi, NULL, "RW"},
};

#define TEST_ALL_CMDS 0

/**
 * @brief Initialize the user defined AT command list
 *
 */
void init_user_at(void)
{
	// Assign custom AT command list to pointer used by WisBlock API
	g_user_at_cmd_list = g_user_at_cmd_new_list;

	// Add AT commands to structure
	g_user_at_cmd_num += sizeof(g_user_at_cmd_new_list) / sizeof(atcmd_t);
	MYLOG("USR_AT", "Added %d User AT commands", g_user_at_cmd_num);

#ifdef ESP32
	has_custom_at = true;
#endif
}
