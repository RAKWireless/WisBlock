/**
 * @file RAK1906_bsec.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK1906_BSEC_H
#define RAK1906_BSEC_H
#include <Arduino.h>

bool init_rak1906_bsec(void);
bool read_rak1906_bsec(void);
void get_rak1906_bsec_values(float *values);
bool do_read_rak1906_bsec(void);
uint16_t get_alt_rak1906_bsec(void);

#endif // RAK1906_BSEC_H