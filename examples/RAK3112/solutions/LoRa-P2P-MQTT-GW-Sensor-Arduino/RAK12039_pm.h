/**
 * @file RAK12039_pm.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12039_H
#define RAK12039_H
#include <Arduino.h>

bool init_rak12039(void);
void read_rak12039(void);

#endif // RAK12039_H