/**
 * @file debug_log.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Debug macro definitions
 * @version 0.1
 * @date 2022-09-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef DEBUG_LOG_H
#define DEBUG_LOG_H
#include <Arduino.h>

// Debug output set to 0 to disable app debug output
#ifndef MY_DEBUG
#define MY_DEBUG 0
#endif

#if defined NRF52_SERIES
#if MY_DEBUG > 0
#define MYLOG(tag, ...)                     \
	do                                      \
	{                                       \
		if (tag)                            \
			PRINTF("[%s] ", tag);           \
		PRINTF(__VA_ARGS__);                \
		PRINTF("\n");                       \
		if (g_ble_uart_is_connected)        \
		{                                   \
			g_ble_uart.printf(__VA_ARGS__); \
			g_ble_uart.printf("\n");        \
		}                                   \
	} while (0)
#else
#define MYLOG(...)
#endif
#endif
#if defined ARDUINO_ARCH_RP2040
#if MY_DEBUG > 0
#define MYLOG(tag, ...)                  \
	do                                   \
	{                                    \
		if (tag)                         \
			Serial.printf("[%s] ", tag); \
		Serial.printf(__VA_ARGS__);      \
		Serial.printf("\n");             \
	} while (0)
#else
#define MYLOG(...)
#endif
#endif

#if defined ESP32
#if MY_DEBUG > 0
#define MYLOG(tag, ...)                                                 \
	if (tag)                                                            \
		Serial.printf("[%s] ", tag);                                    \
	Serial.printf(__VA_ARGS__);                                         \
	Serial.printf("\n");                                                \
	if (g_ble_uart_is_connected)                                        \
	{                                                                   \
		char buff[255];                                                 \
		int len = sprintf(buff, __VA_ARGS__);                           \
		uart_tx_characteristic->setValue((uint8_t *)buff, (size_t)len); \
		uart_tx_characteristic->notify(true);                           \
		delay(50);                                                      \
	}
#else
#define MYLOG(...)
#endif
#endif
#endif // DEBUG_LOG_H