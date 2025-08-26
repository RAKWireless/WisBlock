/**
 * @file RAK1903_light.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK1903_H
#define RAK1903_H
#include <Arduino.h>

bool init_rak1903(void);
void read_rak1903();

#endif // RAK1903_H