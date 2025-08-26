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

// Might need adjustments
#ifndef SEALEVELPRESSURE_HPA
#define SEALEVELPRESSURE_HPA (1010.0)
#endif

bool init_rak1906(void);
void start_rak1906(void);
bool read_rak1906(void);
void get_rak1906_values(float *values);
uint16_t get_alt_rak1906(void);

// Environment and barometric pressure sensor stuff
extern float at_MSL;

#endif // RAK1906_H