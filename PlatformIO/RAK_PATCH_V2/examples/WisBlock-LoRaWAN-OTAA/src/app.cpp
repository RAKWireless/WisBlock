/**
   @file main.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Example application for RAK3112 and RAK3312
 * @version 0.1
 * @date 2025-03-30
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Arduino.h>
#include <LoRaWan-Arduino.h>
#include <SPI.h>

// OTAA Device EUI MSB
uint8_t node_device_eui[8] = {0x00, 0x0D, 0x75, 0xE6, 0x56, 0x4D, 0xC1, 0xF3};
// OTAA Application EUI MSB
uint8_t node_app_eui[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1};
// OTAA Application Key MSB
uint8_t node_app_key[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};

/** Flag for the event type */ 
volatile uint16_t g_task_event_type;

/** Wake up events */
#define NO_EVENT 0
#define STATUS 0b0000000000000001
#define N_STATUS 0b1111111111111110

// Function declarations
static void lorawan_has_joined_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void lorawan_join_failed_handler(void);
static void lorawan_tx_finished(void);
static void lorawan_tx_conf_finished(bool result);
static void send_lora_frame(void);

// LoRaWAN settings
#define LORAWAN_APP_TX_DUTYCYCLE 10000 /**< Defines the application data transmission duty cycle. 10s, value in [ms]. */
#define LORAWAN_APP_DATA_BUFF_SIZE 64  /**< Size of the data to be transmitted. */
#define JOINREQ_NBTRIALS 3			   /**< Number of trials for the join request. */

/** Timer for sending packets */
#if defined(_VARIANT_RAK3112_) || defined(_VARIANT_RAK11200_)
Ticker send_timer;
#elif defined(_VARIANT_RAK4630_)
SoftwareTimer send_timer;
#elif defined(_VARIANT_RAK11300_)
TimerEvent_t send_timer;
#endif

/** LoRaWAN packet send interval in milliseconds*/
#define PACKET_INTERVAL 30000

/** Callback for timer */
#if defined(_VARIANT_RAK3112_) || defined(_VARIANT_RAK11200_)
static void tx_lora_periodic_handler(void);
#elif defined(_VARIANT_RAK4630_)
void tx_lora_periodic_handler(TimerHandle_t unused);
#elif defined(_VARIANT_RAK11300_)
void tx_lora_periodic_handler(void);
#endif

/** Lora packer data buffer. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];
/** Lora user application data structure. */
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0};

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
 */
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DEFAULT_DATARATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_DEFAULT_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
 */
static lmh_callback_t lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
										lorawan_rx_handler, lorawan_has_joined_handler,
										lorawan_confirm_class_handler, lorawan_join_failed_handler,
										lorawan_tx_finished, lorawan_tx_conf_finished};

/** Semaphore used by events to wake up loop task */
SemaphoreHandle_t event_semaphore = NULL;

/**
 * @brief Arduino setup function, called once
 *
 */
void setup(void)
{
	// Prepare LED's BLUE ==> TX, GREEN ==> Received a packet
	// pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);

	// Initialize Serial for debug output
	Serial.begin(115200);
	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	Serial.println("RAK3312 SX126x LoRaWAN test");

	// Create the task event semaphore
	event_semaphore = xSemaphoreCreateBinary();
	// Initialize semaphore
	xSemaphoreGive(event_semaphore);
	// Take the semaphore so the loop will go to sleep until an event happens
	xSemaphoreTake(event_semaphore, 10);

	// Initialize the LoRa chip
	Serial.println("Initialize SX1262");
	uint32_t err_code = lora_rak3112_init();
	if (err_code != 0)
	{
		Serial.printf("Initialize SX1262 failed - %d\n", err_code);
	}

	// Setup the EUIs and Keys
	lmh_setDevEui(node_device_eui);
	lmh_setAppEui(node_app_eui);
	lmh_setAppKey(node_app_key);

	// Initialize timers
#if defined(_VARIANT_RAK3112_) || defined(_VARIANT_RAK11200_)
	// Not required with ESP32
#elif defined(_VARIANT_RAK4630_)
	send_timer.begin(60000, tx_lora_periodic_handler, NULL, true);
#elif defined(_VARIANT_RAK11300_)
	send_timer.oneShot = false;
	send_timer.ReloadValue = 60000;
	send_timer.Callback = tx_lora_periodic_handler;
	TimerInit(&send_timer, tx_lora_periodic_handler);
	TimerSetValue(&send_timer, 60000);

#endif

	// Initialize LoRaWan
	err_code = lmh_init(&lora_callbacks, lora_param_init, true, (eDeviceClass)CLASS_A, (LoRaMacRegion_t)LORAMAC_REGION_AS923_3);
	if (err_code != 0)
	{
		Serial.printf("lmh_init failed - %d\n", err_code);
	}

	// Start Join procedure
	lmh_join();
}

/**
 * @brief Arduino loop, runs forever
 *
 */
void loop(void)
{
	// Wait until semaphore is released (FreeRTOS)
	xSemaphoreTake(event_semaphore, portMAX_DELAY);
	while (g_task_event_type != 0)
	{
		if ((g_task_event_type & STATUS) == STATUS)
		{
			g_task_event_type &= N_STATUS;
			Serial.println("Sending frame");
			send_lora_frame();
			digitalWrite(LED_BLUE, HIGH);
			Serial.println("----------------------");
		}
	}
}

/**
 * @brief Initiate sending an uplink
 *
 */
static void send_lora_frame(void)
{
	if (lmh_join_status_get() != LMH_SET)
	{
		// Not joined, try again later
		Serial.println("Did not join network, skip sending frame");
		return;
	}

	uint32_t i = 0;
	m_lora_app_data.port = LORAWAN_APP_PORT; // 0267008003670128017401a4
	m_lora_app_data.buffer[i++] = 0x02;
	m_lora_app_data.buffer[i++] = 0x67;
	m_lora_app_data.buffer[i++] = 0x00;
	m_lora_app_data.buffer[i++] = 0x80;
	m_lora_app_data.buffer[i++] = 0x03;
	m_lora_app_data.buffer[i++] = 0x67;
	m_lora_app_data.buffer[i++] = 0x01;
	m_lora_app_data.buffer[i++] = 0x28;
	m_lora_app_data.buffer[i++] = 0x01;
	m_lora_app_data.buffer[i++] = 0x74;
	m_lora_app_data.buffer[i++] = 0x01;
	m_lora_app_data.buffer[i++] = 0xa4;
	m_lora_app_data.buffsize = i;

	lmh_error_status error = lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG);
	Serial.printf("lmh_send result %d\n", error);
}

#if defined(_VARIANT_RAK3112_) || defined(_VARIANT_RAK11200_) || defined(_VARIANT_RAK11300_)
/**
 * @brief Callback for timer to start uplink
 *
 */
void tx_lora_periodic_handler(void)
{
	// Give semaphore to start sending from loop
	g_task_event_type |= STATUS;
	xSemaphoreGive(event_semaphore);
}
#elif defined(_VARIANT_RAK4630_)
/**
 * @brief Callback for timer to start uplink
 *
 * @param unused
 */
void tx_lora_periodic_handler(TimerHandle_t unused)
{
	// Give semaphore to start sending from loop
	g_task_event_type |= STATUS;
	xSemaphoreGive(event_semaphore);
}
// #elif defined(_VARIANT_RAK11300_)
// #error RP2040 timer not implemented
#endif

/**
 * @brief LoRa function for handling OTAA join failed
 *
 */
static void lorawan_join_failed_handler(void)
{
	Serial.println("OVER_THE_AIR_ACTIVATION failed!");
	Serial.println("Check your EUI's and Keys's!");
	Serial.println("Check if a Gateway is in range!");
}

/**
 * @brief LoRa function for handling HasJoined event.
 *
 */
static void lorawan_has_joined_handler(void)
{
	Serial.println("Network Joined");

	digitalWrite(LED_BLUE, LOW);

	// Start timer for frequent uplink sending
#if defined(_VARIANT_RAK3112_) || defined(_VARIANT_RAK11200_)
	send_timer.attach_ms(60000, tx_lora_periodic_handler);
#elif defined(_VARIANT_RAK4630_)
	send_timer.start();
#elif defined(_VARIANT_RAK11300_)
	TimerStart(&send_timer);
#endif
}

/**
 * @brief Function for handling LoRaWan received data from Gateway
 *
 * @param app_data Pointer to rx data
 */
static void lorawan_rx_handler(lmh_app_data_t *app_data)
{
	Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d\n",
				  app_data->port, app_data->buffsize, app_data->rssi, app_data->snr);

	switch (app_data->port)
	{
	case 3:
		// Port 3 switches the class
		if (app_data->buffsize == 1)
		{
			switch (app_data->buffer[0])
			{
			case 0:
				lmh_class_request(CLASS_A);
				break;

			case 1:
				lmh_class_request(CLASS_B);
				break;

			case 2:
				lmh_class_request(CLASS_C);
				break;

			default:
				break;
			}
		}
		break;

	case LORAWAN_APP_PORT:
		// YOUR_JOB: Take action on received data
		break;

	default:
		break;
	}
}

/**
 * @brief Callback for switching LoRaWAN device class
 *
 * @param Class new device class
 */
static void lorawan_confirm_class_handler(DeviceClass_t Class)
{
	Serial.printf("switch to class %c done\n", "ABC"[Class]);

	// Informs the server that switch has occurred ASAP
	m_lora_app_data.buffsize = 0;
	m_lora_app_data.port = LORAWAN_APP_PORT;
	lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG);
}

/**
 * @brief Callback after unconfirmed uplink is finished
 *
 */
static void lorawan_tx_finished(void)
{
	Serial.printf("TX finished\n");

	digitalWrite(LED_BLUE, LOW);
}

/**
 * @brief Callback after confirmed uplink is finished
 *
 * @param result true if ACK was received, false if no response
 */
static void lorawan_tx_conf_finished(bool result)
{
	Serial.printf("TX confirmed %s\n", result ? "finished with ACK" : "failed");
	digitalWrite(LED_BLUE, LOW);
}