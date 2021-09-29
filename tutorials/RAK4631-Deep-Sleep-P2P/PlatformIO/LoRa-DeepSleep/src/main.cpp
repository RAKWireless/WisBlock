/**
 * @file main.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief LoRa-DeepSleep setup() and loop()
 * @version 0.1
 * @date 2021-01-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

/** Semaphore used by events to wake up loop task */
SemaphoreHandle_t taskEvent = NULL;

/** Timer to wakeup task frequently and send message */
SoftwareTimer taskWakeupTimer;

/** Buffer for received LoRaWan data */
uint8_t rcvdLoRaData[256];
/** Length of received data */
uint8_t rcvdDataLen = 0;

/**
 * @brief Flag for the event type
 * -1 => no event
 * 0 => LoRaWan data received
 * 1 => Timer wakeup
 * 2 => tbd
 * ...
 */
uint8_t eventType = -1;

/**
 * @brief Timer event that wakes up the loop task frequently
 * 
 * @param unused 
 */
void periodicWakeup(TimerHandle_t unused)
{
	// Switch on blue LED to show we are awake
#if MYLOG_LOG_LEVEL > MYLOG_LOG_LEVEL_NONE
	digitalWrite(LED_CONN, HIGH);
#endif
	eventType = 1;
	// Give the semaphore, so the loop task will wake up
	xSemaphoreGiveFromISR(taskEvent, pdFALSE);
}

void setup()
{
	// Setup the build in LED
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(LED_CONN, OUTPUT);
	digitalWrite(LED_CONN, LOW);

#if MYLOG_LOG_LEVEL > MYLOG_LOG_LEVEL_NONE
	digitalWrite(LED_BUILTIN, HIGH);
	// Start serial
	Serial.begin(115200);

	// Wait max 5 seconds for a terminal to connect
	time_t timeout = millis();
	while (!Serial)
	{
		if ((millis() - timeout) < 15000)
		{
			delay(100);
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		}
		else
		{
			break;
		}
	}
	myLog_d("====================================");
	myLog_d("WisBlock LoRa P2P deep sleep example");
	myLog_d("====================================");
#endif
	// Switch off LED
	digitalWrite(LED_BUILTIN, LOW);

	// Create the semaphore
	myLog_d("Create task semaphore");
	delay(100); // Give Serial time to send
	taskEvent = xSemaphoreCreateBinary();

	// Give the semaphore, seems to be required to initialize it
	myLog_d("Initialize task Semaphore");
	delay(100); // Give Serial time to send
	xSemaphoreGive(taskEvent);

	// Take the semaphore, so loop will be stopped waiting to get it
	myLog_d("Take task Semaphore");
	delay(100); // Give Serial time to send
	xSemaphoreTake(taskEvent, 10);

	// Start LoRa
	if (!initLoRa())
	{
		myLog_e("Init LoRa failed");
		while (1)
		{
		}
	}
	myLog_d("Init LoRa success");

	// Now we are connected, start the timer that will wakeup the loop frequently
	myLog_d("Start Wakeup Timer");
	taskWakeupTimer.begin(SLEEP_TIME, periodicWakeup);
	taskWakeupTimer.start();

#if MYLOG_LOG_LEVEL > MYLOG_LOG_LEVEL_NONE
	// Give Serial some time to send everything
	delay(1000);
#endif
}

void loop()
{
	// Sleep until we are woken up by an event
	if (xSemaphoreTake(taskEvent, portMAX_DELAY) == pdTRUE)
	{
		// Switch on green LED to show we are awake
#if MYLOG_LOG_LEVEL > MYLOG_LOG_LEVEL_NONE
		digitalWrite(LED_BUILTIN, HIGH);
		delay(500); // Only so we can see the green LED
#endif

		// Check the wake up reason
		switch (eventType)
		{
		case 0: // Wakeup reason is package downlink arrived
			myLog_d("Received package over LoRaWan");
			break;
		case 1: // Wakeup reason is timer
			myLog_d("Timer wakeup");

			/// \todo read sensor or whatever you need to do frequently

			// Send the data package
			myLog_d("Initiate sending");
			sendLoRa();
			break;
		default:
			myLog_d("This should never happen ;-)");
			break;
		}
		// Go back to sleep
		xSemaphoreTake(taskEvent, 10);
#if MYLOG_LOG_LEVEL > MYLOG_LOG_LEVEL_NONE
		digitalWrite(LED_BUILTIN, LOW);
#endif
	}
}