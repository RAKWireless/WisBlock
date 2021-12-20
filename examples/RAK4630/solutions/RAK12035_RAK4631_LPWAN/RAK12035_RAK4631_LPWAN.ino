/**
 * @file app.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Application specific functions. Mandatory to have init_app(), 
 *        app_event_handler(), ble_data_handler(), lora_data_handler()
 *        and lora_tx_finished()
 * @version 0.1
 * @date 2021-04-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "app.h"

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "RAK-SOIL";

/** Required for give semaphore from ISR */
BaseType_t g_higher_priority_task_woken = pdTRUE;

/** Battery level uinion */
batt_s batt_level;

/** Send Fail counter **/
uint8_t send_fail = 0;

/** Flag if the acc sensor is available */
bool has_acc = false;

/**
 * @brief Application specific setup functions
 * 
 */
void setup_app(void)
{
#if API_DEBUG == 0
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
#endif
	// Enable BLE
	g_enable_ble = true;
}

/**
 * @brief Application specific initializations
 * 
 * @return true Initialization success
 * @return false Initialization failure
 */
bool init_app(void)
{
	MYLOG("APP", "init_app");

	Wire.begin();

	if (init_acc())
	{
		has_acc = true;
	}
	// Initialize Soil module
	return init_soil();
}

/**
 * @brief Application specific event handler
 *        Requires as minimum the handling of STATUS event
 *        Here you handle as well your application specific events
 */
void app_event_handler(void)
{
	// To lower power consumption, switch off the LED immediately
	digitalWrite(LED_GREEN, LOW);
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");

		// If BLE is enabled, restart Advertising
		if (g_enable_ble)
		{
			restart_advertising(15);
		}

		// Get soil sensor values
		read_soil();

		// Get battery level
		float temp_batt = read_batt() / 10;
		for (int num = 0; num < 10; num++)
		{
			temp_batt += read_batt() / 10;
			temp_batt /= 2;
		}

		batt_level.batt16 = (uint16_t)temp_batt;
		g_soil_data.batt_1 = batt_level.batt8[1];
		g_soil_data.batt_2 = batt_level.batt8[0];

#if MY_DEBUG > 0
		Serial.printf("%02X", g_soil_data.data_flag0);
		Serial.printf("%02X", g_soil_data.data_flag1);
		Serial.printf("%02X", g_soil_data.humid_1);
		Serial.printf("%02X", g_soil_data.data_flag2);
		Serial.printf("%02X", g_soil_data.data_flag3);
		Serial.printf("%02X", g_soil_data.temp_1);
		Serial.printf("%02X", g_soil_data.temp_2);
		Serial.printf("%02X", g_soil_data.data_flag4);
		Serial.printf("%02X", g_soil_data.data_flag5);
		Serial.printf("%02X", g_soil_data.batt_1);
		Serial.printf("%02X", g_soil_data.batt_2);
		Serial.printf("%02X", g_soil_data.data_flag6);
		Serial.printf("%02X", g_soil_data.data_flag7);
		Serial.printf("%02X\n", g_soil_data.valid);
#endif

		lmh_error_status result = send_lora_packet((uint8_t *)&g_soil_data, SOIL_DATA_LEN);
		switch (result)
		{
		case LMH_SUCCESS:
			MYLOG("APP", "Packet enqueued");
			if (g_ble_uart_is_connected)
			{
				g_ble_uart.println("Packet enqueued");
			}
			break;
		case LMH_BUSY:
			MYLOG("APP", "LoRa transceiver is busy");
			if (g_ble_uart_is_connected)
			{
				g_ble_uart.println("LoRa transceiver is busy");
			}
			break;
		case LMH_ERROR:
			MYLOG("APP", "Packet error, too big to send with current DR");
			if (g_ble_uart_is_connected)
			{
				g_ble_uart.println("Packet error, too big to send with current DR");
			}
			break;
		}
	}

	// ACC trigger event
	if ((g_task_event_type & ACC_TRIGGER) == ACC_TRIGGER)
	{
		g_task_event_type &= N_ACC_TRIGGER;
		MYLOG("APP", "ACC triggered");

		if (has_acc)
		{
			read_acc();
			clear_acc_int();
			restart_advertising(15);
		}
	}
}

/**
 * @brief Handle BLE UART data
 * 
 */
void ble_data_handler(void)
{
	// To lower power consumption, switch off the LED immediately
	digitalWrite(LED_GREEN, LOW);
	if (g_enable_ble)
	{
		// BLE UART data handling
		if ((g_task_event_type & BLE_DATA) == BLE_DATA)
		{
			MYLOG("AT", "RECEIVED BLE");
			/** BLE UART data arrived */
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
 * @brief Handle received LoRa Data
 * 
 */
void lora_data_handler(void)
{
	// To lower power consumption, switch off the LED immediately
	digitalWrite(LED_GREEN, LOW);

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
			// Restart Join Request
			lmh_join();
		}
	}

	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRa data arrived
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
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

		if (g_ble_uart_is_connected && g_enable_ble)
		{
			for (int idx = 0; idx < g_rx_data_len; idx++)
			{
				g_ble_uart.printf("%02X ", g_rx_lora_data[idx]);
			}
			g_ble_uart.println("");
		}
	}

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;

		MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");
		if (g_ble_uart_is_connected)
		{
			g_ble_uart.printf("LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");
		}

		if (!g_rx_fin_result)
		{
			// Increase fail send counter
			send_fail++;

			if (send_fail == 10)
			{
				// Too many failed sendings, reset node and try to rejoin
				delay(100);
				sd_nvic_SystemReset();
			}
		}
	}
}
