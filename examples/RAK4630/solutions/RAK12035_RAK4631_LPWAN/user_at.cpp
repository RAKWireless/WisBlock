/**
 * @file user_at.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Handle user defined AT commands
 * @version 0.1
 * @date 2021-11-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "app.h"

#define AT_PRINTF(...)                  \
	Serial.printf(__VA_ARGS__);         \
	if (g_ble_uart_is_connected)        \
	{                                   \
		g_ble_uart.printf(__VA_ARGS__); \
	}

bool user_at_handler(char *user_cmd, uint8_t cmd_size)
{
	MYLOG("APP", "Received User AT commmand >>%s<< len %d", user_cmd, cmd_size);

	// Get the command itself
	char *param;

	param = strtok(user_cmd, ":");

	// Check if the command is supported
	if (strcmp(param, (const char *)"+DRY=?") == 0)
	{
		// Dry calibration value query
		AT_PRINTF("Dry Calibration Value: %d", get_calib(true));
		return true;
	}
	if (strcmp(param, (const char *)"+WET=?") == 0)
	{
		// Dry calibration requested
		AT_PRINTF("Wet Calibration Value: %d", get_calib(false));
		return true;
	}
	if (strcmp(param, (const char *)"+DRY") == 0)
	{
		// Dry calibration requested
		AT_PRINTF("Start Dry Calibration\n");
		uint16_t new_val = start_calib(true);
		AT_PRINTF("New Dry Calibration Value: %d", new_val);
		return true;
	}
	if (strcmp(param, (const char *)"+WET") == 0)
	{
		// Wet calibration requested
		AT_PRINTF("Start Wet Calibration\n");
		uint16_t new_val = start_calib(false);
		AT_PRINTF("New Wet Calibration Value: %d", new_val);
		return true;
	}

	return false;
}
