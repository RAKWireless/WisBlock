/**
 * @file RAK1902_press.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK1902_H
#define RAK1902_H
#include <Arduino.h>

// Might need adjustments
#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA (1010.0)
#endif

bool init_rak1902(void);
void start_rak1902(void);
void read_rak1902(void);
float get_rak1902(void);
uint16_t get_alt_rak1902(void);

// Environment and barometric pressure sensor stuff
extern float at_MSL;

#endif // RAK1902_H