/**
 * @file RAK12019_uv.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12019_H
#define RAK12019_H
#include <Arduino.h>

bool init_rak12019(void);
void read_rak12019(void);

#endif // RAK12019_H