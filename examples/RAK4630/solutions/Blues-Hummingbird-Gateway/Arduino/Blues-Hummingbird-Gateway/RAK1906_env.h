/**
 * @file RAK1906_env.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK1906_H
#define RAK1906_H
#include <Arduino.h>

// Function declarations
bool init_rak1906(void);
bool read_rak1906(void);
void get_rak1906_values(float *values);

// Cayenne LPP Channel numbers per sensor value
#define LPP_CHANNEL_HUMID_2 6 // RAK1906
#define LPP_CHANNEL_TEMP_2 7  // RAK1906
#define LPP_CHANNEL_PRESS_2 8 // RAK1906
#define LPP_CHANNEL_GAS_2 9	  // RAK1906

#endif // RAK1906_H