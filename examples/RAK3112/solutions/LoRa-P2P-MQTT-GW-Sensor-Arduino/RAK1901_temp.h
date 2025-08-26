/**
 * @file RAK1901_temp.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef RAK1901_H
#define RAK1901_H
#include <Arduino.h>

bool init_rak1901(void);
void read_rak1901(void);
void get_rak1901_values(float *values);

#endif // RAK1901_H