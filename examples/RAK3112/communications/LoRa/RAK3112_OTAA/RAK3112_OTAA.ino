/**
   @file RAK3112_OTAA.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Example application for RAK3112 and RAK3312
 * @version 0.1
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Arduino.h>
#include <LoRaWan-Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Ticker.h>

// Function declarations
static void lorawan_has_joined_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void lorawan_join_failed_handler(void);
static void lorawan_tx_finished(void);
static void lorawan_tx_conf_finished(bool result);
static void send_lora_frame(void);
void wifi_event_cb(WiFiEvent_t event);

// LoRaWAN settings
#define LORAWAN_APP_TX_DUTYCYCLE 30000 /**< Defines the application data transmission duty cycle. 30s, value in [ms]. */
#define LORAWAN_APP_DATA_BUFF_SIZE 64  /**< Size of the data to be transmitted. */
#define JOINREQ_NBTRIALS 3			   /**< Number of trials for the join request. */

/** Timer for sending packets */
Ticker send_timer;
/** LoRaWAN packet send interval in milliseconds*/
#define PACKET_INTERVAL 30000

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

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Change LoRaWAN credentials to unique EUI's and key.
// DevEUI can be taken from the label of the module
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/** OTAA DevEUI */
uint8_t nodeDeviceEUI[8] = {0xac, 0x1f, 0x09, 0xff, 0xfe, 0x00, 0x00, 0x00}; // ac1f09fffe000000
/** OTAA AppEUI */
uint8_t nodeAppEUI[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1}; // 70B3D57ED00201E1
/** OTAA AppKey */
uint8_t nodeAppKey[16] = {0x2b, 0x84, 0xe0, 0xb0, 0x9b, 0x68, 0xe5, 0xcb, 0x42, 0x17, 0x6f, 0xe7, 0x53, 0xdc, 0xee, 0x79}; // 2b84e0b09b68e5cb42176fe753dcee79

/** Semaphore used by events to wake up loop task */
SemaphoreHandle_t send_now = NULL;

/**
 * @brief Arduino setup function, called once
 *
 */
void setup(void)
{
	// Prepare LED's BLUE ==> TX, GREEN ==> Received a packet
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);

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

	Serial.println("RAK3312 SX126x LoRaWAN OTAA test");

	// Create the task event semaphore
	send_now = xSemaphoreCreateBinary();
	// Initialize semaphore
	xSemaphoreGive(send_now);
	// Take the semaphore so the loop will go to sleep until an event happens
	xSemaphoreTake(send_now, 10);

	// Initialize the LoRa chip
	Serial.println("Initialize SX1262");
	uint32_t err_code = lora_rak3112_init();
	if (err_code != 0)
	{
		Serial.printf("Initialize SX1262 failed - %d\n", err_code);
	}

	// Setup the EUIs and Keys
	lmh_setDevEui(nodeDeviceEUI);
	lmh_setAppEui(nodeAppEUI);
	lmh_setAppKey(nodeAppKey);

	// Initialize LoRaWan
	err_code = lmh_init(&lora_callbacks, lora_param_init, true, CLASS_A, LORAMAC_REGION_AS923_3);
	if (err_code != 0)
	{
		Serial.printf("lmh_init failed - %d\n", err_code);
	}

	// Start Join procedure
	lmh_join();

	Serial.println("----------------------");
	Serial.printf("PSRAM: %ldbytes\n", ESP.getPsramSize());
	Serial.printf("Free PSRAM: %ldbytes\n", ESP.getFreePsram());
	Serial.printf("RAM: %ldbytes\n", ESP.getHeapSize());
	Serial.printf("Free Heap: %ldbytes\n", esp_get_free_heap_size());
}

/**
 * @brief Arduino loop, runs forever
 *
 */
void loop(void)
{
	// Wait until semaphore is released (FreeRTOS)
	xSemaphoreTake(send_now, portMAX_DELAY);
	Serial.println("Sending frame");
	send_lora_frame();
	digitalWrite(LED_BLUE, HIGH);
	Serial.println("----------------------");
	Serial.printf("PSRAM: %ldbytes\n", ESP.getPsramSize());
	Serial.printf("Free PSRAM: %ldbytes\n", ESP.getFreePsram());
	Serial.printf("RAM: %ldbytes\n", ESP.getHeapSize());
	Serial.printf("Free Heap: %ldbytes\n", esp_get_free_heap_size());
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

	// Fake sensor data in Cayenne LPP format
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

/**
 * @brief Callback for timer to start uplink
 *
 */
static void tx_lora_periodic_handler(void)
{
	// Give semaphore to start sending from loop
	xSemaphoreGive(send_now);
}

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
	send_timer.attach_ms(PACKET_INTERVAL, tx_lora_periodic_handler);
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

	Serial.println("Packet data: ");
	for (int idx = 0; idx < app_data->buffsize; idx++)
	{
		Serial.printf("%02X", app_data->buffer[idx]);
	}
	Serial.println("");
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
