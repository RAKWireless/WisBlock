/**
 * @file app.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Application specific functions. Mandatory to have init_app(),
 *        app_event_handler(), ble_data_handler(), lora_data_handler()
 *        and lora_tx_finished()
 * @version 0.2
 * @date 2022-01-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "app.h"

/** Timer since last position message was sent */
time_t last_pos_send = 0;
/** Timer for delayed sending to keep duty cycle */
Ticker delayed_sending;

/** Flag if delayed sending is already activated */
bool delayed_active = false;

/** Minimum delay between sending new locations, set to 30 seconds */
time_t min_delay = 30000;

/** GPS precision */
bool g_gps_prec_6 = true;

/** Switch between Cayenne LPP and Helium Mapper data packet */
bool g_is_helium = false;

/** Switch to Field Tester data packet */
bool g_is_tester = false;

/** Switch to enable/disable GNSS module power */
bool g_gnss_power_off = false;

/** Flag for battery protection enabled */
bool battery_check_enabled = false;

/** Send Fail counter **/
uint8_t join_send_fail = 0;

/** Flag for low battery protection */
bool low_batt_protection = false;

/** LoRaWAN packet */
WisCayenne g_solution_data(255);

/** Received package for parsing */
uint8_t rcvd_data[256];
/** Length of received package */
uint16_t rcvd_data_len = 0;

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "RAK-GW";
/** Initialization result */
bool init_result = true;

char disp_txt[64] = {0};

/**
 * @brief Initial setup of the application (before LoRaWAN and BLE setup)
 *
 */
void setup_app(void)
{
	/** For ESP32 flag that we have custom AT commands. */
	has_custom_at = true;

	// Initialize WiFi and MQTT connection
	setup_wifi();

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

	/************************************************************/
	/** This code works only in LoRa P2P mode.                  */
	/** Forcing here the usage of LoRa P2P, independant of      */
	/** saved settings.                                         */
	/************************************************************/
	// Read LoRaWAN settings from flash
	api_read_credentials();
	// Force LoRa P2P
	g_lorawan_settings.lorawan_enable = false;
	// Check if DevEUI was setup before or if it is "default" 0x56, 0x4D, 0xC1, 0xF3
	if ((g_lorawan_settings.node_device_eui[4] == 0x56) &&
		(g_lorawan_settings.node_device_eui[5] == 0x4D) &&
		(g_lorawan_settings.node_device_eui[6] == 0xC1) &&
		(g_lorawan_settings.node_device_eui[7] == 0xF3))
	{
		MYLOG("SETUP", "No valid DevEUI found, use BLE-MAC");

		// Variable to store the MAC address
		uint8_t baseMac[6];

		// Get the MAC address of the Bluetooth interface
		esp_read_mac(baseMac, ESP_MAC_BT);

		MYLOG("SETUP", "Device ID from BLE MAC %02X%02X%02X%02X", baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
		g_lorawan_settings.node_device_eui[0] = 0xAC;
		g_lorawan_settings.node_device_eui[1] = 0x1F;
		g_lorawan_settings.node_device_eui[2] = baseMac[0];
		g_lorawan_settings.node_device_eui[3] = baseMac[1];
		g_lorawan_settings.node_device_eui[4] = baseMac[2];
		g_lorawan_settings.node_device_eui[5] = baseMac[3];
		g_lorawan_settings.node_device_eui[6] = baseMac[4];
		g_lorawan_settings.node_device_eui[7] = baseMac[5];
	}
	else
	{
		MYLOG("SETUP", "Device ID from DEV EUI %02X%02X%02X%02X", g_lorawan_settings.node_device_eui[4], g_lorawan_settings.node_device_eui[5],
			  g_lorawan_settings.node_device_eui[6], g_lorawan_settings.node_device_eui[7]);
	}
	// Save LoRaWAN settings
	api_set_credentials();

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);

	delay(500);

	// Scan the I2C interfaces for devices
	find_modules();

	// Initialize the User AT command list
	init_user_at();

#ifdef BLE_OFF
	// Enable BLE
	g_enable_ble = false;
#else
	// Enable BLE
	g_enable_ble = true;
#endif

	sprintf(g_custom_fw_ver, "WisBlock Sensor V%d.%d.%d\n", g_sw_ver_1, g_sw_ver_2, g_sw_ver_3);
}

/**
 * @brief Final setup of application  (after LoRaWAN and BLE setup)
 *
 * @return true Initialization success
 * @return false Initialization failure
 */
bool init_app(void)
{
	/** Set permanent RX mode for LoRa P2P */
	g_lora_p2p_rx_mode = RX_MODE_RX;

	MYLOG("APP", "init_app");

	uint32_t node_id_dec = g_lorawan_settings.node_device_eui[7];
	node_id_dec |= (uint32_t)g_lorawan_settings.node_device_eui[6] << 8;
	node_id_dec |= (uint32_t)g_lorawan_settings.node_device_eui[5] << 16;
	node_id_dec |= (uint32_t)g_lorawan_settings.node_device_eui[4] << 24;
	Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	Serial.println("WisBlock P2P MQTT gateway");
	Serial.printf("Device ID %02X%02X%02X%02X = %u\r\n", g_lorawan_settings.node_device_eui[4], g_lorawan_settings.node_device_eui[5],
				  g_lorawan_settings.node_device_eui[6], g_lorawan_settings.node_device_eui[7], node_id_dec);
	Serial.println("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

	snprintf(g_custom_fw_ver, 63, "RAK3112-MQTT-GW-V%d.%d.%d", SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);

	api_set_version(SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);

	// Get the battery check setting
	read_batt_settings();

	AT_PRINTF("============================\n");
	if (found_sensors[ENV_ID].found_sensor)
	{
		AT_PRINTF("LPWAN Environment Solution\n");
	}
	else if (found_sensors[PRESS_ID].found_sensor)
	{
		AT_PRINTF("LPWAN Weather Sensor\n");
	}
	else if (found_sensors[VOC_ID].found_sensor)
	{
		AT_PRINTF("LPWAN VOC Sensor\n");
	}
	else
	{
		AT_PRINTF("LPWAN WisBlock Node\n");
	}
	AT_PRINTF("Built with RAK's WisBlock\n");
	AT_PRINTF("SW Version %d.%d.%d\n", g_sw_ver_1, g_sw_ver_2, g_sw_ver_3);
	AT_PRINTF("LoRa(R) is a registered trademark or service mark of Semtech Corporation or its affiliates.\nLoRaWAN(R) is a licensed mark.\n");
	AT_PRINTF("============================\n");
	api_log_settings();

	// Announce found modules with +EVT: over Serial
	announce_modules();

	AT_PRINTF("============================\n");

	Serial.flush();
	// Reset the packet
	g_solution_data.reset();

	// Enable P2P RX continuous
	g_lora_p2p_rx_mode = RX_MODE_RX;
	g_lora_p2p_rx_time = 0;
	g_rx_continuous = true;
	// Put Radio into continuous RX mode
	Radio.Standby();
	Radio.Rx(0);

	restart_advertising(300);

	return true;
}

/**
 * @brief Application specific event handler
 *        Requires as minimum the handling of STATUS event
 *        Here you handle as well your application specific events
 */
void app_event_handler(void)
{
	// Handle wake up call
	if ((g_task_event_type & AT_CMD) == AT_CMD)
	{
		if (g_device_sleep)
		{
			at_wake();
		}
		return;
	}

	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");

		// Keep MQTT alive
		check_mqtt();

#if USE_BSEC == 0
		/*********************************************/
		/** Select between Bosch BSEC algorithm for  */
		/** IAQ index or simple T/H/P readings       */
		/*********************************************/
		if (found_sensors[ENV_ID].found_sensor && !g_is_helium && !g_is_tester) // Using simple T/H/P readings
		{
			// Startup the BME680
			start_rak1906();
		}
#endif
		if (found_sensors[PRESS_ID].found_sensor && !g_is_helium && !g_is_tester)
		{
			// Startup the LPS22HB
			start_rak1902();
		}

		// If BLE is enabled, restart Advertising
		if (g_enable_ble)
		{
			Serial.println("Restart BLE from STATUS");
			restart_advertising(15);
		}

		// Reset the packet
		g_solution_data.reset();

		if (!low_batt_protection)
		{
			if (!g_is_helium && !g_is_tester)
			{
				// Get values from the connected modules
				get_sensor_values();
			}
		}

		// Get battery level
		float batt_level_f = read_batt();
		g_solution_data.addVoltage(LPP_CHANNEL_BATT, batt_level_f / 1000.0);

		// Protection against battery drain if battery check is enabled
		if (battery_check_enabled)
		{
			if (batt_level_f < 3300)
			{
				// Battery is very low, change send time to 1 hour to protect battery
				low_batt_protection = true; // Set low_batt_protection active
				api_timer_restart(1 * 60 * 60 * 1000);
				MYLOG("APP", "Battery protection activated");
			}
			else if ((batt_level_f > 4100) && low_batt_protection)
			{
				// Battery is higher than 4V, change send time back to original setting
				low_batt_protection = false;
				api_timer_restart(g_lorawan_settings.send_repeat_time);
				MYLOG("APP", "Battery protection deactivated");
			}
		}

		// Get data from the slower sensors
#if USE_BSEC == 0
		/*********************************************/
		/** Select between Bosch BSEC algorithm for  */
		/** IAQ index or simple T/H/P readings       */
		/*********************************************/
		if (found_sensors[ENV_ID].found_sensor) // Using simple T/H/P readings
		{
			// Read environment data
			read_rak1906();
		}
#endif
		if (found_sensors[PRESS_ID].found_sensor)
		{
			// Read environment data
			read_rak1902();
		}
		// Read the CO2 sensor last, it needs temperature and humidity values first
		if (found_sensors[SCT31_ID].found_sensor)
		{
			// Read CO2 data
			read_rak12008();
		}

		uint8_t *packet = g_solution_data.getBuffer();
		MYLOG("APP", "Packet size %d Content:", g_solution_data.getSize());

#if MY_DEBUG > 0
		for (int idx = 0; idx < g_solution_data.getSize(); idx++)
		{
			Serial.printf("%02X ", packet[idx]);
		}
		Serial.println("");
#endif
		// Add the device DevEUI as a device ID to the packet
		g_solution_data.addDevID(LPP_CHANNEL_DEVID, &g_lorawan_settings.node_device_eui[4]);

		if (mqtt_parse_send(packet, g_solution_data.getSize()))
		{
			MYLOG("APP", "GW MQTT sent");
		}
		else
		{
			MYLOG("APP", "GW MQTT failed");
		}
		// Reset the packet
		g_solution_data.reset();
	}

	// VOC read request event
	if ((g_task_event_type & VOC_REQ) == VOC_REQ)
	{
		g_task_event_type &= N_VOC_REQ;

		do_read_rak12047();
	}

	/*********************************************/
	/** Select between Bosch BSEC algorithm for  */
	/** IAQ index or simple T/H/P readings       */
	/*********************************************/
	// BSEC read request event
	if ((g_task_event_type & BSEC_REQ) == BSEC_REQ)
	{
		g_task_event_type &= N_BSEC_REQ;

#if USE_BSEC == 1
		do_read_rak1906_bsec();
#endif
	}

	// Parse request event
	if ((g_task_event_type & PARSE) == PARSE)
	{
		g_task_event_type &= N_PARSE;

		// Keep MQTT alive
		check_mqtt();

		if (mqtt_parse_send(rcvd_data, rcvd_data_len))
		{
			MYLOG("APP", "Node MQTT sent");
		}
		else
		{
			MYLOG("APP", "Node MQTT failed");
		}
	}
}

/**
 * @brief Handle received LoRa Data
 *
 */
void lora_data_handler(void)
{
	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		g_task_event_type &= N_LORA_DATA;
		MYLOG("APP", "Received package over LoRa");
		if (g_rx_data_len <= 8)
		{
			MYLOG("APP", "Received package too short");
		}
		else
		{
#if MY_DEBUG > 0
			CayenneLPP lpp(g_rx_data_len - 8);
			memcpy(lpp.getBuffer(), &g_rx_lora_data[8], g_rx_data_len - 8);
			DynamicJsonDocument jsonBuffer(4096);
			JsonObject root = jsonBuffer.to<JsonObject>();
			lpp.decodeTTN(lpp.getBuffer(), g_rx_data_len - 8, root);
			serializeJsonPretty(root, Serial);
			Serial.println();
#endif
			memcpy(rcvd_data, g_rx_lora_data, g_rx_data_len);
			rcvd_data_len = g_rx_data_len;
			api_wake_loop(PARSE);
		}
	}
}

/**
 * @brief Timer function used to avoid sending packages too often.
 * 			Delays the next package by 10 seconds
 *
 * @param unused
 * 			Timer handle, not used
 */
void send_delayed(void)
{
	api_wake_loop(STATUS);
	delayed_sending.detach();
}
