/**
 * @file lorawan.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief LoRaWAN setup and handler
 * @version 0.1
 * @date 2021-10-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

/** LoRaWAN setting from flash */
s_lorawan_settings g_lorawan_settings;

/** Buffer for received LoRaWan data */
uint8_t g_rx_lora_data[256];
/** Length of received data */
uint8_t g_rx_data_len = 0;
/** Buffer for received LoRaWan data */
uint8_t g_tx_lora_data[256];
/** Length of received data */
uint8_t g_tx_data_len = 0;

/** RSSI of last received packet */
int16_t g_last_rssi = 0;
/** SNR of last received packet */
int8_t g_last_snr = 0;
/** fPort of last received packet */
uint8_t g_last_fport = 0;

/** Flag if LoRaWAN is initialized and started */
bool g_lorawan_initialized = false;
/** Result of last TX */
bool g_rx_fin_result;
/** Result of join request */
bool g_join_result = false;

/**************************************************************/
/* LoRaWAN properties                                            */
/**************************************************************/
/** LoRaWAN application data buffer. */
uint8_t m_lora_app_data_buffer[256];
/** Lora application data structure. */
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0};

// LoRaWAN event handlers
/** LoRaWAN callback when join network finished */
static void lpwan_joined_handler(void);
/** LoRaWAN callback when join network failed */
static void lpwan_join_fail_handler(void);
/** LoRaWAN callback when data arrived */
static void lpwan_rx_handler(lmh_app_data_t *app_data);
/** LoRaWAN callback after class change request finished */
static void lpwan_class_confirm_handler(DeviceClass_t Class);
/** LoRaWAN callback after class change request finished */
static void lpwan_unconfirm_tx_finished(void);
/** LoRaWAN callback after class change request finished */
static void lpwan_confirm_tx_finished(bool result);
/** LoRaWAN Function to send a package */
bool send_lpwan_packet(void);

/**@brief Structure containing LoRaWAN parameters, needed for lmh_init()
 * 
 * Set structure members to
 * LORAWAN_ADR_ON or LORAWAN_ADR_OFF to enable or disable adaptive data rate
 * LORAWAN_DEFAULT_DATARATE OR DR_0 ... DR_5 for default data rate or specific data rate selection
 * LORAWAN_PUBLIC_NETWORK or LORAWAN_PRIVATE_NETWORK to select the use of a public or private network
 * JOINREQ_NBTRIALS or a specific number to set the number of trials to join the network
 * LORAWAN_DEFAULT_TX_POWER or a specific number to set the TX power used
 * LORAWAN_DUTYCYCLE_ON or LORAWAN_DUTYCYCLE_OFF to enable or disable duty cycles
 *                   Please note that ETSI mandates duty cycled transmissions. 
 */
static lmh_param_t lora_param_init;

/** Structure containing LoRaWan callback functions, needed for lmh_init() */
static lmh_callback_t lora_callbacks = {get_lora_batt, BoardGetUniqueId, BoardGetRandomSeed, lpwan_rx_handler,
										lpwan_joined_handler, lpwan_class_confirm_handler, lpwan_join_fail_handler,
										lpwan_unconfirm_tx_finished, lpwan_confirm_tx_finished};

bool g_lpwan_has_joined = false;

#if APP_DEBUG > 0
char *region_names[] = {(char *)"AS923", (char *)"AU915", (char *)"CN470", (char *)"CN779",
						(char *)"EU433", (char *)"EU868", (char *)"KR920", (char *)"IN865",
						(char *)"US915", (char *)"AS923-2", (char *)"AS923-3", (char *)"AS923-4", (char *)"RU864"};
#endif
/**
 * @brief Initialize LoRa HW and LoRaWan MAC layer
 * 
 * @return int8_t result
 *  0 => OK
 * -1 => SX126x HW init failure
 * -2 => LoRaWan MAC initialization failure
 * -3 => Subband selection failure
 */
int8_t init_lora(void)
{
	// Initialize LoRa chip.
	if (lora_rak11300_init() != 0)
	{
		APP_LOG("LORA", "Failed to initialize SX1262");
		return -1;
	}
	APP_LOG("LORA", "Initialized SX1262");

	// Setup the EUIs and Keys
	lmh_setDevEui(g_lorawan_settings.node_device_eui);
	lmh_setAppEui(g_lorawan_settings.node_app_eui);
	lmh_setAppKey(g_lorawan_settings.node_app_key);
	lmh_setNwkSKey(g_lorawan_settings.node_nws_key);
	lmh_setAppSKey(g_lorawan_settings.node_apps_key);
	lmh_setDevAddr(g_lorawan_settings.node_dev_addr);

	// Setup the LoRaWan init structure
	lora_param_init.adr_enable = g_lorawan_settings.adr_enabled;
	lora_param_init.tx_data_rate = g_lorawan_settings.data_rate;
	lora_param_init.enable_public_network = g_lorawan_settings.public_network;
	lora_param_init.nb_trials = g_lorawan_settings.join_trials;
	lora_param_init.tx_power = g_lorawan_settings.tx_power;
	lora_param_init.duty_cycle = g_lorawan_settings.duty_cycle_enabled;

	APP_LOG("LORA", "Initialize LoRaWAN for region %s", region_names[g_lorawan_settings.lora_region]);
	// Initialize LoRaWan
	if (lmh_init(&lora_callbacks, lora_param_init, g_lorawan_settings.otaa_enabled, (eDeviceClass)g_lorawan_settings.lora_class, (LoRaMacRegion_t)g_lorawan_settings.lora_region) != 0)
	{
		APP_LOG("LORA", "Failed to initialize LoRaWAN");
		return -2;
	}

	// For some regions we might need to define the sub band the gateway is listening to
	// This must be called AFTER lmh_init()
	if (!lmh_setSubBandChannels(g_lorawan_settings.subband_channels))
	{
		APP_LOG("LORA", "lmh_setSubBandChannels failed. Wrong sub band requested?");
		return -3;
	}

	// Start Join process
	lmh_join();

	g_lorawan_initialized = true;
	return 0;
}

/**************************************************************/
/* LoRaWAN callback functions                                            */
/**************************************************************/
/**
   @brief LoRa function when join has failed
*/
void lpwan_join_fail_handler(void)
{
	APP_LOG("LORA", "OTAA joined failed");
	APP_LOG("LORA", "Check LPWAN credentials and if a gateway is in range");
	// Restart Join procedure
	APP_LOG("LORA", "Restart network join request");
	g_join_result = false;
	// Wake up task to report failed join
	if (loop_thread != NULL)
	{
		APP_LOG("LORA", "Join failed, report event");
		osSignalSet(loop_thread, SIGNAL_JOIN_FAIL);
	}
}

/**
 * @brief LoRa function for handling HasJoined event.
 */
static void lpwan_joined_handler(void)
{
	digitalWrite(LED_BUILTIN, LOW);

#if APP_DEBUG > 0
	if (g_lorawan_settings.otaa_enabled)
	{
		uint32_t otaaDevAddr = lmh_getDevAddr();
		APP_LOG("LORA", "OTAA joined and got dev address %08lX", otaaDevAddr);
	}
	else
	{
		APP_LOG("LORA", "ABP joined");
	}

	delay(100); // Just to enable the serial port to send the message
#endif

	// Class A is default in the LoRaWAN lib. If app needs different class, request change here
	if (g_lorawan_settings.lora_class != CLASS_A)
	{
		// Switch to configured class
		lmh_class_request((DeviceClass_t)g_lorawan_settings.lora_class);
	}
	else
	{
		g_lpwan_has_joined = true;

		if (g_lorawan_settings.send_repeat_time != 0)
		{
			// Now we are connected, start the timer that will wakeup the loop frequently
			// Initialize application timer
			// app_ticker.attach_us(trigger_sending, g_lorawan_settings.send_repeat_time * 1000);
			app_timer.oneShot = false;
			app_timer.ReloadValue = g_lorawan_settings.send_repeat_time;
			TimerInit(&app_timer, trigger_sending);
			TimerSetValue(&app_timer, g_lorawan_settings.send_repeat_time);
		}
	}

	g_join_result = true;
	// Wake up task to report succesful join
	if (loop_thread != NULL)
	{
		APP_LOG("LORA", "Join success, report event");
		osSignalSet(loop_thread, SIGNAL_JOIN_SUCCESS);
	}
}

/**
 * @brief Function for handling LoRaWan received data from Gateway
 *
 * @param app_data  Pointer to rx data
 */
static void lpwan_rx_handler(lmh_app_data_t *app_data)
{
	APP_LOG("LORA", "LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d",
			app_data->port, app_data->buffsize, app_data->rssi, app_data->snr);

	g_last_rssi = app_data->rssi;
	g_last_snr = app_data->snr;
	g_last_fport = app_data->port;

	// Copy the data into loop data buffer
	memcpy(g_rx_lora_data, app_data->buffer, app_data->buffsize);
	g_rx_data_len = app_data->buffsize;
	// Wake up task to report succesful join
	if (loop_thread != NULL)
	{
		APP_LOG("LORA", "Packet received, report event");
		osSignalSet(loop_thread, SIGNAL_RX);
	}
}

/**
 * @brief Callback for class switch confirmation
 * 
 * @param Class The new class
 */
static void lpwan_class_confirm_handler(DeviceClass_t Class)
{
	APP_LOG("LORA", "switch to class %c done", "ABC"[Class]);

	// Wake up task to report succesful join
	if (loop_thread != NULL)
	{
		APP_LOG("LORA", "Join success, report event");
		osSignalSet(loop_thread, SIGNAL_JOIN_SUCCESS);
	}
	g_lpwan_has_joined = true;
}

/**
 * @brief Called after unconfirmed packet was sent
 * 
 */
static void lpwan_unconfirm_tx_finished(void)
{
	APP_LOG("LORA", "Uncomfirmed TX finished");
	g_rx_fin_result = true;
	// Wake up task to report succesful join
	if (loop_thread != NULL)
	{
		APP_LOG("LORA", "TX success, report event");
		osSignalSet(loop_thread, SIGNAL_UNCONF_TX);
	}
}

/**
 * @brief Called after confirmed packet was sent
 * 
 * @param result Result of sending true = ACK received false = No ACK
 */
static void lpwan_confirm_tx_finished(bool result)
{
	APP_LOG("LORA", "Comfirmed TX finished with result %s", result ? "ACK" : "NAK");
	g_rx_fin_result = result;
	// Wake up task to report succesful join
	if (loop_thread != NULL)
	{
		if (g_rx_fin_result)
		{
			APP_LOG("LORA", "TX success, report event");
			osSignalSet(loop_thread, SIGNAL_CONF_TX_ACK);
		}
		else
		{
			APP_LOG("LORA", "TX failed, report event");
			osSignalSet(loop_thread, SIGNAL_CONF_TX_NAK);
		}
	}
}

/**
 * @brief Send a LoRaWan package
 * 
 * @return result of send request
 */
lmh_error_status send_lora_packet(uint8_t *data, uint8_t size, uint8_t fport)
{
	if (lmh_join_status_get() != LMH_SET)
	{
		//Not joined, try again later
		APP_LOG("LORA", "Did not join network, skip sending frame");
		return LMH_ERROR;
	}

	if (fport != NULL)
	{
		m_lora_app_data.port = fport;
	}
	else
	{
		m_lora_app_data.port = g_lorawan_settings.app_port;
	}

	m_lora_app_data.buffsize = size;

	memcpy(m_lora_app_data_buffer, data, size);

	return lmh_send(&m_lora_app_data, g_lorawan_settings.confirmed_msg_enabled);
}
