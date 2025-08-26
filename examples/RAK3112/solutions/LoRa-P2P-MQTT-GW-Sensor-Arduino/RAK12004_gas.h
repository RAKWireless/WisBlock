/**
 * @file RAK12004_gas.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Global definitions and forward declarations
 * @version 0.1
 * @date 2022-09-23
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef RAK12004_H
#define RAK12004_H
#include <Arduino.h>

bool init_rak12004(void);
void read_rak12004(void);

/** Gas Sensor stuff RAK12004 and RAK12009 */
/** Logic high enables the device. Logic low disables the device */
#ifndef EN_PIN
#define EN_PIN WB_IO6
#endif
#ifndef ALERT_PIN
/** Alarm trigger, a high indicates that the respective limit has been violated. */
#define ALERT_PIN WB_IO5
#endif
#ifndef RatioGasCleanAir
/** Clean Air ratio. RS / R0 = 1.0 ppm */
#define RatioGasCleanAir (1.0)
#endif
#ifndef Gas_RL
/** RL on the modules, RL = 10KΩ  can adjust */
#define Gas_RL (10.0)
#endif
#ifndef V_RATIO
/** Voltage amplification factor */
#define V_RATIO 3.0
#endif

extern float sensorPPM;
extern float PPMpercentage;
extern uint16_t last_gas;

#endif // RAK12004_H