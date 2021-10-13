/**
 * @file at_serial.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Thread to handle Serial input
 * @version 0.1
 * @date 2021-10-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

/** The event handler thread */
Thread _thread_handle_serial(osPriorityNormal, 4096);

/** Thread id for lora event thread */
osThreadId _serial_task_thread = NULL;

void serial1_rx_handler(void)
{
	if (_serial_task_thread != NULL)
	{
		osSignalSet(_serial_task_thread, SIGNAL_RX);
	}
}

// Task to handle timer events
void _serial_task()
{
	_serial_task_thread = osThreadGetId();
	// attachInterrupt(SERIAL1_RX, serial1_rx_handler, RISING);

	// Flush for serial USB RX
	if (Serial.available())
	{
		while (Serial.available() > 0)
		{
			Serial.read();
			delay(10);
		}
	}

	// Flush for serial 1 RX
	if (Serial1.available())
	{
		while (Serial1.available() > 0)
		{
			Serial1.read();
			delay(10);
		}
	}

	while (true)
	{
		// Wait for serial USB RX
		if (Serial.available())
		{
			while (Serial.available() > 0)
			{
				at_serial_input(uint8_t(Serial.read()));
				delay(5);
			}
		}

		// Wait for serial 1 RX
		// osEvent event = osSignalWait(0, osWaitForever);
		if (Serial1.available())
		{
			while (Serial1.available() > 0)
			{
				at_serial_input(uint8_t(Serial1.read()));
				delay(5);
			}
		}
		delay(100);
		yield();
	}
}

bool init_serial_task(void)
{
	_thread_handle_serial.start(_serial_task);
	_thread_handle_serial.set_priority(osPriorityNormal);

	/// \todo how to detect that the task is really created
	return true;
}
