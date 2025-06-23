/**
 * @file app.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief For application specific includes and definitions
 *        Will be included from main.h
 * @version 0.3
 * @date 2023-03-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef APP_H
#define APP_H

//**********************************************/
//** Set the application firmware version here */
//**********************************************/
// major version increase on API change / not backwards compatible
#ifndef SW_VERSION_1
#define SW_VERSION_1 1
#endif
// minor version increase on API change / backward compatible
#ifndef SW_VERSION_2
#define SW_VERSION_2 0
#endif
// patch version increase on bugfix, no affect on API
#ifndef SW_VERSION_3
#define SW_VERSION_3 11
#endif

#include <Arduino.h>

/** Add you required includes after Arduino.h */

// #ifdef ARDUINO_ARCH_RP2040
// #include <mbed.h>
// #include <rtos.h>
// #include <multicore.h>
// #include <time.h>
// #include <Stream.h>

// using namespace rtos;
// using namespace mbed;
// using namespace std::chrono_literals;
// using namespace std::chrono;

// #define Stream arduino::Stream
// #define SPI mbed::SPI

// #endif // ARDUINO_ARCH_RP2040

// RAK11300/RAK11310 using earlephilhower RP2040 core
// #if defined(ARDUINO_RAKWIRELESS_RAK11300)
// #include "..\rakwireless\variants\rak11300\rak_variant.h"
// #endif

/** Include the WisBlock-API */
#include <WisBlock-API-V2.h> 

// Add debug output macros
#include "debug_log.h"

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);
void init_user_at(void);

extern bool init_result;
extern uint8_t g_last_fport;

#ifdef NRF52_SERIES
extern SoftwareTimer delayed_sending;
void send_delayed(TimerHandle_t unused);
#endif
#ifdef ESP32
extern Ticker delayed_sending;
void send_delayed(void);
#endif
#ifdef _VARIANT_RAK11300_
TimerEvent_t send_timer;
void send_delayed(void);
#endif

#endif