/**
 * @file main.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief App event handlers
 * @version 0.1
 * @date 2023-04-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "main.h"

/** LoRaWAN packet */
WisCayenne g_solution_data(255);

/** Received package for parsing */
uint8_t rcvd_data[256];
/** Length of received package */
uint16_t rcvd_data_len = 0;

/** Send Fail counter **/
uint8_t send_fail = 0;

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "RAK";

/** Flag for RAK1906 sensor */
bool has_rak1906 = false;

/** Flag is Blues Notecard was found */
bool has_blues = false;

SoftwareTimer delayed_sending;
void delayed_cellular(TimerHandle_t unused);

/**
 * @brief Initial setup of the application (before LoRaWAN and BLE setup)
 *
 */
void setup_app(void)
{
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

	// Set firmware version
	api_set_version(SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);
	g_enable_ble = true;
}

/**
 * @brief Final setup of application  (after LoRaWAN and BLE setup)
 *
 * @return true
 * @return false
 */
bool init_app(void)
{
	MYLOG("APP", "init_app");

	MYLOG("INI", "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	MYLOG("INI", "WisBlock Blues Tracker");
	MYLOG("INI", "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

	// Initialize User AT commands
	init_user_at();

	// Check if RAK1906 is available
	has_rak1906 = init_rak1906();
	if (has_rak1906)
	{
		AT_PRINTF("+EVT:RAK1906");
	}

	// Initialize Blues Notecard
	has_blues = init_blues();
	if (!has_blues)
	{
		AT_PRINTF("+EVT:CELLULAR_ERROR");
	}

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);

	restart_advertising(30);

	delayed_sending.begin(15000, delayed_cellular, NULL, false);

	// Start the send interval timer and send a first message
	if (!g_lorawan_settings.auto_join)
	{
		MYLOG("APP", "Initialize LoRaWAN stack, but do not join");
		if (g_lorawan_settings.lorawan_enable)
		{
			API_LOG("API", "Auto join is enabled, start LoRaWAN and join");
			init_lorawan();
		}
		api_timer_start();
		api_wake_loop(STATUS);
	}
	return true;
}

/**
 * @brief Handle events
 * 		Events can be
 * 		- timer (setup with AT+SENDINT=xxx)
 * 		- interrupt events
 * 		- wake-up signals from other tasks
 */
void app_event_handler(void)
{
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;

		MYLOG("APP", "Timer wakeup");

		// Reset the packet
		g_solution_data.reset();

		if (!blues_get_location())
		{
			MYLOG("APP", "Failed to get location");
		}

		// Get battery level
		float batt_level_f = read_batt();
		g_solution_data.addVoltage(LPP_CHANNEL_BATT, batt_level_f / 1000.0);

		// Read sensors and battery
		if (has_rak1906)
		{
			read_rak1906();
		}

		bool check_rejoin = false;

		if (g_lpwan_has_joined)
		{
			/*************************************************************************************/
			/*                                                                                   */
			/* If the device is setup for LoRaWAN, try first to send the data as confirmed       */
			/* packet. If the sending fails, retry over cellular modem                           */
			/*                                                                                   */
			/* If the device is setup for LoRa P2P, send always as P2P packet AND over the       */
			/* cellular modem                                                           */
			/*                                                                                   */
			/*************************************************************************************/
			if (g_lorawan_settings.lorawan_enable)
			{
				lmh_error_status result = send_lora_packet(g_solution_data.getBuffer(), g_solution_data.getSize());
				switch (result)
				{
				case LMH_SUCCESS:
					MYLOG("APP", "Packet enqueued");
					break;
				case LMH_BUSY:
					re_init_lorawan();
					result = send_lora_packet(g_solution_data.getBuffer(), g_solution_data.getSize());
					if (result != LMH_SUCCESS)
					{
						// Send over cellular connection
						delayed_sending.start();
						check_rejoin = true;
						send_fail++;
						MYLOG("APP", "LoRa transceiver is busy");
						AT_PRINTF("+EVT:BUSY\n");
					}
					break;
				case LMH_ERROR:
					re_init_lorawan();
					result = send_lora_packet(g_solution_data.getBuffer(), g_solution_data.getSize());
					if (result != LMH_SUCCESS)
					{
						// Send over cellular connection
						delayed_sending.start();
						check_rejoin = true;
						send_fail++;
						AT_PRINTF("+EVT:SIZE_ERROR\n");
						MYLOG("APP", "Packet error, too big to send with current DR");
					}
					break;
				}
			}
			else
			{
				// Add unique identifier in front of the P2P packet, here we use the DevEUI
				g_solution_data.addDevID(LPP_CHANNEL_DEVID, &g_lorawan_settings.node_device_eui[4]);

				// Send packet over LoRa
				// if (send_p2p_packet(packet_buffer, g_solution_data.getSize() + 8))
				if (send_p2p_packet(g_solution_data.getBuffer(), g_solution_data.getSize()))
				{
					MYLOG("APP", "Packet enqueued");
				}
				else
				{
					AT_PRINTF("+EVT:SIZE_ERROR\n");
					MYLOG("APP", "Packet too big");
				}

				// Send as well over cellular connection
				delayed_sending.start();
			}
		}
		else
		{
			// delayed_sending.start();
			g_task_event_type |= USE_CELLULAR;
			if (g_lorawan_settings.lorawan_enable)
			{
				check_rejoin = true;
				send_fail++;
			}
			MYLOG("APP", "Network not joined, skip sending over LoRaWAN");
		}

		if (check_rejoin)
		{
			// Check how many times we send over LoRaWAN failed and retry to join LNS after 10 times failing
			if (send_fail >= 10)
			{
				// Too many failed sendings, try to rejoin
				MYLOG("APP", "Retry to join LNS");
				send_fail = 0;
				// int8_t init_result = re_init_lorawan();
				lmh_join();
			}
		}
	}

	// Send over Blues event
	if ((g_task_event_type & USE_CELLULAR) == USE_CELLULAR)
	{
		g_task_event_type &= N_USE_CELLULAR;
		// Send over cellular connection
		MYLOG("APP", "Get hub sync status:");
		blues_hub_status();

		g_solution_data.addDevID(0, &g_lorawan_settings.node_device_eui[4]);
		blues_send_payload(g_solution_data.getBuffer(), g_solution_data.getSize());

		// Request sync with NoteHub
		blues_start_req("hub.sync");
		blues_send_req();
	}

	// Blues ATTN event
	if ((g_task_event_type & BLUES_ATTN) == BLUES_ATTN)
	{
		g_task_event_type &= N_BLUES_ATTN;
		// Send over cellular connection
		MYLOG("APP", "Blues ATTN event");

		blues_start_req("card.attn");
		blues_send_req();

		blues_start_req("card.time");
		blues_send_req();

		// req = notecard.newRequest("card.attn");
		// if (!blues_send_req())
		// {
		// 	MYLOG("BLUES", "card.attn request failed");
		// 	return false;
		// }

		blues_enable_attn();
	}
}

/**
 * @brief Handle BLE events
 *
 */
void ble_data_handler(void)
{
	if (g_enable_ble)
	{
		if ((g_task_event_type & BLE_DATA) == BLE_DATA)
		{
			MYLOG("AT", "RECEIVED BLE");
			// BLE UART data arrived
			g_task_event_type &= N_BLE_DATA;

			while (g_ble_uart.available() > 0)
			{
				at_serial_input(uint8_t(g_ble_uart.read()));
				delay(5);
			}
			at_serial_input(uint8_t('\n'));
		}
	}
}

/**
 * @brief Handle LoRa events
 *
 */
void lora_data_handler(void)
{
	// LoRa Join finished handling
	if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
	{
		g_task_event_type &= N_LORA_JOIN_FIN;
		if (g_join_result)
		{
			MYLOG("APP", "Successfully joined network");
			send_fail = 0;
		}
		else
		{
			MYLOG("APP", "Join network failed");
		}
	}

	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		g_task_event_type &= N_LORA_DATA;
		MYLOG("APP", "Received package over LoRa");
		char log_buff[g_rx_data_len * 3] = {0};
		uint8_t log_idx = 0;
		for (int idx = 0; idx < g_rx_data_len; idx++)
		{
			sprintf(&log_buff[log_idx], "%02X ", g_rx_lora_data[idx]);
			log_idx += 3;
		}
		MYLOG("APP", "%s", log_buff);
	}

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;

		MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");

		if (!g_rx_fin_result)
		{
			if (g_lorawan_settings.lorawan_enable)
			{
				delayed_sending.start();
			}

			// Increase fail send counter
			send_fail++;
		}
		else
		{
			send_fail = 0;
		}
	}
}

/**
 * @brief Timer callback to decouple the LoRaWAN sending and the cellular sending
 * 
 * @param unused 
 */
void delayed_cellular(TimerHandle_t unused)
{
	api_wake_loop(USE_CELLULAR);
}