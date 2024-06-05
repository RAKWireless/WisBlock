/**
 * @file LoRa-P2P-Simple-Ard.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Simple LoRa P2P example with CAD
 * @version 0.1
 * @date 2024-06-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <Arduino.h>
#ifdef _VARIANT_RAK4630_
#include <Adafruit_TinyUSB.h>
#endif
#include <SX126x-Arduino.h>
#include <SPI.h>

// Function declarations
void OnTxDone(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxTimeout(void);
void OnRxTimeout(void);
void OnRxError(void);
void OnCadDone(bool cadResult);

// Define LoRa parameters
#define RF_FREQUENCY 916100000		  // Hz
#define TX_OUTPUT_POWER 22			  // dBm
#define LORA_BANDWIDTH 0			  // [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
#define LORA_SPREADING_FACTOR 7		  // [SF7..SF12]
#define LORA_CODINGRATE 1			  // [1: 4/5, 2: 4/6,  3: 4/7,  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8		  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0		  // Symbols
#define LORA_FIX_LENGTH_PAYLOAD false // No fixed payload length
#define LORA_IQ_INVERSION false		  // No IQ inversion
#define TX_TIMEOUT_VALUE 120000
#define RX_TIMEOUT_VALUE 30000
#define BUFFER_SIZE 64 // Define the payload size here

static RadioEvents_t RadioEvents;
static uint16_t BufferSize = BUFFER_SIZE;
static uint8_t RcvBuffer[BUFFER_SIZE];
static uint8_t TxdBuffer[BUFFER_SIZE];

time_t cadTime;

/**
 * @brief Arduino setup function. Called once after power-up or reset
 *
 */
void setup()
{
	pinMode(LED_GREEN, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, LOW);

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

	Serial.println("=====================================");
	Serial.println("SX126x LoRa P2P test");
	Serial.println("=====================================");

	// Initialize the LoRa chip
	Serial.println("Starting lora_hardware_init");

#ifdef _VARIANT_RAK4630_
	lora_rak4630_init();
#endif
#ifdef _VARIANT_RAK11200_
	lora_rak13300_init();
#endif
#ifdef _VARIANT_RAK11300_
	lora_rak11300_init();
#endif

	// Initialize the Radio callbacks
	RadioEvents.TxDone = OnTxDone;
	RadioEvents.RxDone = OnRxDone;
	RadioEvents.TxTimeout = OnTxTimeout;
	RadioEvents.RxTimeout = OnRxTimeout;
	RadioEvents.RxError = OnRxError;
	RadioEvents.CadDone = OnCadDone;

	// Initialize the Radio
	Radio.Init(&RadioEvents);

	// Set Radio channel
	Radio.SetChannel(RF_FREQUENCY);

	// Set Radio TX configuration
	Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
					  LORA_SPREADING_FACTOR, LORA_CODINGRATE,
					  LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD,
					  true, 0, 0, LORA_IQ_INVERSION, TX_TIMEOUT_VALUE);

	// Set Radio RX configuration
	Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
					  LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
					  LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD,
					  0, true, 0, 0, LORA_IQ_INVERSION, true);

	// Start LoRa
	Serial.println("Starting Radio.Rx");
	Radio.Rx(0);
}

/**
 * @brief Arduino loop function. Sleeps until semaphore is given by timer.
 * Then starts sending a packet by starting CAD (channel activity detection)
 *
 */
void loop()
{
	// Sleep until we are woken up by an event
	delay(5000);

	/******************************************************************************/
	/* Recommended, instead of just sending, use CAD (channel activity detection) */
	/** The packet will be sent if CAD returns no activity in the CAD callback    */
	/** OnCadDone                                                                 */
	/******************************************************************************/
	// Check if our channel is available for sending
	Radio.Standby();
	Radio.SetCadParams(LORA_CAD_08_SYMBOL, LORA_SPREADING_FACTOR + 13, 10, LORA_CAD_ONLY, 0);
	cadTime = millis();
	Radio.StartCad();

	/***************************************************************************/
	/* Optional, just send data, can lead to collisions and loss of data       */
	/***************************************************************************/
	// // Send the next PING frame
	// TxdBuffer[0] = 'P';
	// TxdBuffer[1] = 'I';
	// TxdBuffer[2] = 'N';
	// TxdBuffer[3] = 'G';
	// TxdBuffer[4] = 0x00;

	// Radio.Send(TxdBuffer, 5);

	digitalWrite(LED_GREEN, HIGH);
}

/**@brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void)
{
	Serial.println("OnTxDone");

	// Back to RX
	Radio.Rx(0);
}

/**@brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
	Serial.println("OnRxDone");

	delay(10);
	BufferSize = size;
	memcpy(RcvBuffer, payload, BufferSize);

	Serial.printf("RssiValue=%d dBm, SnrValue=%d\n", rssi, snr);

	for (int idx = 0; idx < size; idx++)
	{
		Serial.printf("%02X ", RcvBuffer[idx]);
	}
	Serial.println("");

	digitalWrite(LED_GREEN, LOW);

	// Back to RX
	Radio.Rx(0);
}

/**@brief Function to be executed on Radio Tx Timeout event
 */
void OnTxTimeout(void)
{
	// Radio.Sleep();
	Serial.println("OnTxTimeout");

	digitalWrite(LED_GREEN, LOW);

	// Back to RX
	Radio.Rx(0);
}

/**@brief Function to be executed on Radio Rx Timeout event
 */
void OnRxTimeout(void)
{
	Serial.println("OnRxTimeout");

	digitalWrite(LED_GREEN, LOW);

	// Back to RX
	Radio.Rx(0);
}

/**@brief Function to be executed on Radio Rx Error event
 */
void OnRxError(void)
{
	Serial.println("OnRxError");

	digitalWrite(LED_GREEN, LOW);

	// Back to RX
	Radio.Rx(0);
}

/**@brief Function to be executed on CAD Done event
 */
void OnCadDone(bool cadResult)
{
	time_t duration = millis() - cadTime;
	if (cadResult)
	{
		Serial.printf("CAD returned channel busy after %ldms\n", duration);

		// Back to RX
		Radio.Rx(0);
	}
	else
	{
		Serial.printf("CAD returned channel free after %ldms\n", duration);

		// Send the next PING frame
		TxdBuffer[0] = 'P';
		TxdBuffer[1] = 'I';
		TxdBuffer[2] = 'N';
		TxdBuffer[3] = 'G';
		TxdBuffer[4] = 0x00;

		Radio.Send(TxdBuffer, 5);
	}
}
