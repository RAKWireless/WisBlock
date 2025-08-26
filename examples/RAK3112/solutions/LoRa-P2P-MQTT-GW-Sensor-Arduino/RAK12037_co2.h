/**
 * @file RAK12037_co2.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12037_H
#define RAK12037_H
#include <Arduino.h>

bool init_rak12037(void);
void read_rak12037(void);

#endif // RAK12037_H