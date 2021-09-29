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

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

// Debug
#include <myLog.h>

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
