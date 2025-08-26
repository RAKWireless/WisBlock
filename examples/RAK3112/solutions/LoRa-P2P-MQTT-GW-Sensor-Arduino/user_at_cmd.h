/**
 * @file user_at_cmd.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-24
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef USER_AT_CMD_H
#define USER_AT_CMD_H
#include <Arduino.h>

// Battery AT command
void read_batt_settings(void);
void save_batt_settings(bool check_batt_enables);

extern bool battery_check_enabled;

/** Battery level uinion */
union batt_s
{
	uint16_t batt16 = 0;
	uint8_t batt8[2];
};

// Sleep AT command
extern bool g_device_sleep;
int at_wake(void);

#endif // USER_AT_CMD_H