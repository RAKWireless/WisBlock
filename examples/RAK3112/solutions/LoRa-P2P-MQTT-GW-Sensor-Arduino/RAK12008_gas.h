/**
 * @file RAK12008_gas.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12008_H
#define RAK12008_H
#include <Arduino.h>

bool init_rak12008(void);
void read_rak12008(void);

#endif // RAK12008_H