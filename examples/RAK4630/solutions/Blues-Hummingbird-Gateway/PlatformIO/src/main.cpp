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
	MYLOG("INI", "WisBlock Hummingbird Blues Sensor");
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
	else
	{
		AT_PRINTF("+EVT:RAK13102");
		MYLOG("APP", "Start P2P RX");
		// Set to permanent listen
		g_lora_p2p_rx_mode = RX_MODE_RX;
		Radio.Rx(0);
	}

	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);
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
		if (g_lpwan_has_joined)
		{
			// Reset the packet
			g_solution_data.reset();

			// Get battery level
			float batt_level_f = read_batt();
			g_solution_data.addVoltage(LPP_CHANNEL_BATT, batt_level_f / 1000.0);

			// Read sensors and battery
			if (has_rak1906)
			{
				read_rak1906();
			}

			if (!has_blues)
			{
				if (g_lorawan_settings.lorawan_enable)
				{
					lmh_error_status result = send_lora_packet(g_solution_data.getBuffer(), g_solution_data.getSize());
					switch (result)
					{
					case LMH_SUCCESS:
						MYLOG("APP", "Packet enqueued");
						break;
					case LMH_BUSY:
						MYLOG("APP", "LoRa transceiver is busy");
						AT_PRINTF("+EVT:BUSY\n");
						break;
					case LMH_ERROR:
						AT_PRINTF("+EVT:SIZE_ERROR\n");
						MYLOG("APP", "Packet error, too big to send with current DR");
						break;
					}
				}
				else
				{
					// Add unique identifier in front of the P2P packet, here we use the DevEUI
					g_solution_data.addDevID(LPP_CHANNEL_DEVID, &g_lorawan_settings.node_device_eui[4]);
					// uint8_t packet_buffer[g_solution_data.getSize() + 8];
					// memcpy(packet_buffer, g_lorawan_settings.node_device_eui, 8);
					// memcpy(&packet_buffer[8], g_solution_data.getBuffer(), g_solution_data.getSize());

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
				}
			}
			else
			{
				MYLOG("APP", "Get hub sync status:");
				blues_hub_status();

				g_solution_data.addDevID(0, &g_lorawan_settings.node_device_eui[4]);
				blues_parse_send(g_solution_data.getBuffer(), g_solution_data.getSize());
			}
			// Reset the packet
			g_solution_data.reset();
		}
		else
		{
			MYLOG("APP", "Network not joined, skip sending");
		}
	}

	// Parse request event
	if ((g_task_event_type & PARSE) == PARSE)
	{
		g_task_event_type &= N_PARSE;

		if (has_blues)
		{
			if (!blues_parse_send(rcvd_data, rcvd_data_len))
			{
				MYLOG("APP", "Parsing or sending failed");

				MYLOG("APP", "**********************************************");
				MYLOG("APP", "Get hub sync status:");
				// {“req”:”hub.sync.status”}
				blues_start_req("hub.sync.status");
				blues_send_req();

				MYLOG("APP", "**********************************************");
				delay(2000);

				MYLOG("APP", "Get note card status:");
				// {“req”:”card.wireless”}
				blues_start_req("card.wireless");
				blues_send_req();

				MYLOG("APP", "**********************************************");
				delay(2000);
			}
		}
		else
		{
			MYLOG("APP", "Got PARSE request, but no Blues Notecard detected");
		}
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
		}
		else
		{
			MYLOG("APP", "Join network failed");
			/// \todo here join could be restarted.
			// lmh_join();
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

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;

		MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");

		if (!g_rx_fin_result)
		{
			// Increase fail send counter
			send_fail++;

			if (send_fail == 10)
			{
				// Too many failed sendings, reset node and try to rejoin
				delay(100);
				api_reset();
			}
		}
	}
}
