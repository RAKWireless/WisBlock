/**
 * @file ble_gateway.ino
 * @author rakwireless.com
 * @brief This sketch demonstrate how to get environment data from BME680
 *    and send the data to lora gateway.
 *    ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //Click here to get the library: http://librarymanager/All#SX126x
#include <SPI.h>
#include <bluefruit.h>

// RAK4630 supply two LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif

bool doOTAA = true;
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60										  /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0									  /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5								  /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3										  /**< Number of trials for the join request. */
DeviceClass_t gCurrentClass = CLASS_A;							  /* class definition*/
lmh_confirm gCurrentConfirm = LMH_CONFIRMED_MSG;				  /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;							  /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
 */
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
										lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler};

//OTAA keys
uint8_t nodeDeviceEUI[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x33, 0x33};
uint8_t nodeAppEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x33, 0x33, 0x33, 0x33, 0x33};

// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64										  /**< buffer size of the data to be transmitted. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];			  //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

static uint32_t count = 0;
static uint32_t count_fail = 0;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Initialize LoRa chip.
	lora_rak4630_init();

	// Initialize Serial for debug output
	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}
	Serial.println("=====================================");
	Serial.println("Welcome to RAK4630 LoRaWan!!!");
	Serial.println("Type: OTAA");

#if defined(REGION_AS923)
	Serial.println("Region: AS923");
#elif defined(REGION_AU915)
	Serial.println("Region: AU915");
#elif defined(REGION_CN470)
	Serial.println("Region: CN470");
#elif defined(REGION_CN779)
	Serial.println("Region: CN779");
#elif defined(REGION_EU433)
	Serial.println("Region: EU433");
#elif defined(REGION_IN865)
	Serial.println("Region: IN865");
#elif defined(REGION_EU868)
	Serial.println("Region: EU868");
#elif defined(REGION_KR920)
	Serial.println("Region: KR920");
#elif defined(REGION_US915)
	Serial.println("Region: US915");
#elif defined(REGION_US915_HYBRID)
	Serial.println("Region: US915_HYBRID");
#else
	Serial.println("Please define a region in the compiler options.");
#endif
	Serial.println("=====================================");

	// Setup the EUIs and Keys
	lmh_setDevEui(nodeDeviceEUI);
	lmh_setAppEui(nodeAppEUI);
	lmh_setAppKey(nodeAppKey);

	// Initialize LoRaWan
	uint32_t err_code = lmh_init(&lora_callbacks, lora_param_init, doOTAA);
	if (err_code != 0)
	{
		Serial.printf("lmh_init failed - %d\n", err_code);
	}

	// Start Join procedure
	lmh_join();
}

void loop()
{
	// Handle Radio events
	Radio.IrqProcess();
}

/**@brief LoRa function for handling HasJoined event.
*/
void lorawan_has_joined_handler(void)
{
	Serial.println("OTAA Mode, Network Joined!");

	lmh_error_status ret = lmh_class_request(gCurrentClass);
	if (ret == LMH_SUCCESS)
	{
		delay(1000);
		start_ble();
	}
}

/**@brief Function for handling LoRaWan received data from Gateway
   @param[in] app_data  Pointer to rx data
*/
void lorawan_rx_handler(lmh_app_data_t *app_data)
{
	Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
				  app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

void lorawan_confirm_class_handler(DeviceClass_t Class)
{
	Serial.printf("switch to class %c done\n", "ABC"[Class]);
	// Informs the server that switch has occurred ASAP
	m_lora_app_data.buffsize = 0;
	m_lora_app_data.port = gAppPort;
	lmh_send(&m_lora_app_data, gCurrentConfirm);
}

/* Environment Service Definitions
   Environment Monitor Service:  0x181A
   Pressure Measurement Char: 0x2A6D
   Temperature Measurement Char: 0x2A6E
   Temperature Measurement Char: 0x2A6F
*/
BLEClientService envms(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLEClientCharacteristic pressuremc(0x2A6D); // pressure
BLEClientCharacteristic temprtmc(UUID16_CHR_TEMPERATURE);
BLEClientCharacteristic humidmc(0x2A6F); // humidity

void start_ble()
{
	// Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
	// SRAM usage required by SoftDevice will increase dramatically with number of connections
	Bluefruit.begin(0, 1);
	Bluefruit.setName("RAKwisnode Central");
	Bluefruit.setTxPower(4);

	// Initialize environment client
	envms.begin();

	// Initialize client characteristics of environment.
	// Note: Client Char will be added to the last service that is begin()ed.
	// set up callback for receiving measurement
	pressuremc.setNotifyCallback(environment_notify_callback);
	pressuremc.begin();

	temprtmc.setNotifyCallback(environment_notify_callback);
	temprtmc.begin();

	humidmc.setNotifyCallback(environment_notify_callback);
	humidmc.begin();

	// Increase Blink rate to different from PrPh advertising mode
	Bluefruit.setConnLedInterval(250);

	// Callbacks for Central
	Bluefruit.Central.setDisconnectCallback(disconnect_callback);
	Bluefruit.Central.setConnectCallback(connect_callback);

	/* Start Central Scanning
     - Enable auto scan if disconnected
     - Interval = 100 ms, window = 80 ms
     - Don't use active scan
     - Filter only accept envms service
     - Start(timeout) with timeout = 0 will scan forever (until connected)
  */
	Bluefruit.Scanner.setRxCallback(scan_callback);
	Bluefruit.Scanner.restartOnDisconnect(true);
	Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
	Bluefruit.Scanner.useActiveScan(true);
	Bluefruit.Scanner.filterUuid(envms.uuid);
	Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds
}

/**
   Callback invoked when scanner pick up an advertising data
   @param report Structural advertising data
*/
void scan_callback(ble_gap_evt_adv_report_t *report)
{
	// Since we configure the scanner with filterUuid()
	// Scan callback only invoked for device with envms service advertised
	// Connect to device with envms service in advertising
	Bluefruit.Central.connect(report);
}

/**
   Callback invoked when an connection is established
   @param conn_handle
*/
void connect_callback(uint16_t conn_handle)
{
	Serial.println("Connected");
	Serial.print("Discovering envms Service ... ");

	// If envms is not found, disconnect and return
	if (!envms.discover(conn_handle))
	{
		Serial.println("Found NONE");

		// disconnect since we couldn't find envms service
		Bluefruit.disconnect(conn_handle);

		return;
	}

	// Once envms service is found, we continue to discover its characteristic
	Serial.println("Found it");

	Serial.println("Discovering Measurement characteristic ... ");
	if (!pressuremc.discover() || !temprtmc.discover() || !humidmc.discover())
	{
		// Measurement chr is mandatory, if it is not found (valid), then disconnect
		Serial.println("Measurement characteristic is mandatory but not found");
		Bluefruit.disconnect(conn_handle);
		return;
	}
	else
	{
		Serial.println("Found measurement characteristic.");
	}

	// Reaching here means we are ready to go, let's enable notification on measurement chr
	if (pressuremc.enableNotify())
	{
		Serial.println("Ready to receive Pressure value.");
	}
	else
	{
		Serial.println("Couldn't enable notify for Pressure characteristic.");
	}
	if (temprtmc.enableNotify())
	{
		Serial.println("Ready to receive Temperature value.");
	}
	else
	{
		Serial.println("Couldn't enable notify for Temperature characteristic.");
	}
	if (humidmc.enableNotify())
	{
		Serial.println("Ready to receive Humidity value.");
	}
	else
	{
		Serial.println("Couldn't enable notify for Humidity characteristic.");
	}
}

/**
   Callback invoked when a connection is dropped
   @param conn_handle
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
	(void)conn_handle;
	(void)reason;

	Serial.print("Disconnected, reason = 0x");
	Serial.println(reason, HEX);
}

/**
   Hooked callback that triggered when a measurement value is sent from peripheral
   @param chr   Pointer client characteristic that even occurred,
                in this example it should be humidity, pressure or temperature
   @param data  Pointer to received data
   @param len   Length of received data
*/
uint8_t temperature_flag = 0;
uint8_t humdity_flag = 0;
uint8_t pressure_flag = 0;
int16_t temperature;
int32_t pressure;
uint16_t humidity;
void environment_notify_callback(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
	//https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml

	switch (chr->uuid._uuid.uuid)
	{
	case 0x2A6D:
	{
		if (len == 4)
		{
			memcpy(&pressure, data, len);
			Serial.printf("Pressure data: %.1f Pa\n", (float)pressure * 0.1);
			pressure_flag = 1;
		}
		else
		{
			Serial.printf("Invalid pressure data\n");
		}
	}
	break;
	case 0x2A6E:
	{
		if (len == 2)
		{
			memcpy(&temperature, data, len);
			Serial.printf("Temperature data: %.2f Cel\n", (float)temperature * 0.01);
			temperature_flag = 1;
		}
		else
		{
			Serial.printf("Invalid temperature data\n");
		}
	}
	break;
	case 0x2A6F:
	{
		if (len == 2)
		{
			memcpy(&humidity, data, len);
			Serial.printf("Humidity data: %.2f %%\n", (float)humidity * 0.01);
			humdity_flag = 1;
		}
		else
		{
			Serial.printf("Invalid humidity data\n");
		}
	}
	break;
	default:
		break;
	}

	if (temperature_flag == 1 && humdity_flag == 1 && pressure_flag == 1)
	{
		uint32_t i = 0;
		memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
		m_lora_app_data.port = gAppPort;
		m_lora_app_data.buffer[i++] = 0x01; // temperature
		m_lora_app_data.buffer[i++] = (uint8_t)(temperature >> 8) & 0xff;
		m_lora_app_data.buffer[i++] = (uint8_t)(temperature)&0xff;

		m_lora_app_data.buffer[i++] = 0x03; // humidity
		m_lora_app_data.buffer[i++] = (uint8_t)(humidity >> 8) & 0xff;
		m_lora_app_data.buffer[i++] = (uint8_t)(humidity)&0xff;

		m_lora_app_data.buffer[i++] = 0x04; // pressure
		m_lora_app_data.buffer[i++] = (uint8_t)(pressure >> 24) & 0xff;
		m_lora_app_data.buffer[i++] = (uint8_t)(pressure >> 16) & 0xff;
		m_lora_app_data.buffer[i++] = (uint8_t)(pressure >> 8) & 0xff;
		m_lora_app_data.buffer[i++] = (uint8_t)(pressure)&0xff;

		m_lora_app_data.buffsize = i;

		lmh_error_status error = lmh_send(&m_lora_app_data, gCurrentConfirm);
		Serial.printf("lmh_send count %d\n", ++count);
		if (error != LMH_SUCCESS)
		{
			Serial.printf("lmh_send fail count %d\n", ++count_fail);
		}
		else
		{
			digitalWrite(LED_BUILTIN, HIGH);
			delay(100);
			digitalWrite(LED_BUILTIN, LOW);
		}
		temperature_flag = 0;
		humdity_flag = 0;
		pressure_flag = 0;
	}
}
