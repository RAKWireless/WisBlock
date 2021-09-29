/**
 * @file main.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Includes and defines for LoRa-DeepSleep example
 * @version 0.1
 * @date 2021-01-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

/**
 * Set log output level
 * 0 => no debug output
 * 1 => Error output myLog_e
 * 2 => Warning output myLog_w
 * 3 => Info output myLog_i
 * 4 => Debug output myLog_d
 * 5 => Verbose output myLog_v
 */
#define MYLOG_LOG_LEVEL 0 

// Debug
#include "myLog.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>


// LoRa stuff
#include <SX126x-RAK4630.h>
/* Time the device is sleeping in milliseconds = 2 minutes * 60 seconds * 1000 milliseconds */
// #define SLEEP_TIME 2 * 60 * 1000
#define SLEEP_TIME 10 * 1000

// LoRaWan stuff
bool initLoRa(void);
void sendLoRa(void);

// Main loop stuff
void periodicWakeup(TimerHandle_t unused);
extern SemaphoreHandle_t taskEvent;
extern uint8_t rcvdLoRaData[];
extern uint8_t rcvdDataLen;
extern uint8_t eventType;
extern SoftwareTimer taskWakeupTimer;
