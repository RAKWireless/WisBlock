/**
 * @file RAK12047_voc.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12047_H
#define RAK12047_H
#include <Arduino.h>

bool init_rak12047(void);
void read_rak12047(void);
void do_read_rak12047(void);

#endif // RAK12047_H