/**
 * @file at_cmd.cpp
 * @author Taylor Lee (taylor.lee@rakwireless.com)
 * @brief AT command parser
 * @version 0.1
 * @date 2021-04-27
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "at_cmd.h"

#define AT_ERROR "+CME ERROR:"
#define ATCMD_SIZE 160
#define ATQUERY_SIZE 128

#define AT_ERRNO_NOSUPP (1)
#define AT_ERRNO_NOALLOW (2)
#define AT_ERRNO_PARA_VAL (5)
#define AT_ERRNO_PARA_NUM (6)
#define AT_ERRNO_SYS (8)
#define AT_CB_PRINT (0xFF)

static char atcmd[ATCMD_SIZE];
static uint16_t atcmd_index = 0;
static char g_at_query_buf[ATQUERY_SIZE];

typedef struct atcmd_s
{
	const char *cmd_name;		   // CMD NAME
	const char *cmd_desc;		   // AT+CMD?
	int (*query_cmd)(void);		   // AT+CMD=?
	int (*exec_cmd)(char *str);	   // AT+CMD=value
	int (*exec_cmd_no_para)(void); // AT+CMD
} atcmd_t;

static int hex2bin(const char *hex, uint8_t *bin, uint16_t bin_length)
{
	uint16_t hex_length = strlen(hex);
	const char *hex_end = hex + hex_length;
	uint8_t *cur = bin;
	uint8_t num_chars = hex_length & 1;
	uint8_t byte = 0;

	if (hex_length % 2 != 0)
	{
		return -1;
	}

	if (hex_length / 2 > bin_length)
	{
		return -1;
	}

	while (hex < hex_end)
	{
		if ('A' <= *hex && *hex <= 'F')
		{
			byte |= 10 + (*hex - 'A');
		}
		else if ('a' <= *hex && *hex <= 'f')
		{
			byte |= 10 + (*hex - 'a');
		}
		else if ('0' <= *hex && *hex <= '9')
		{
			byte |= *hex - '0';
		}
		else
		{
			return -1;
		}
		hex++;
		num_chars++;

		if (num_chars >= 2)
		{
			num_chars = 0;
			*cur++ = byte;
			byte = 0;
		}
		else
		{
			byte <<= 4;
		}
	}
	return cur - bin;
}

/**
 * @brief AT+BAND=? Get regional frequency band
 * 
 * @return int always 0
 */
static int at_query_region(void)
{
	// 0: AS923 1: AU915 2: CN470 3: CN779 4: EU433 5: EU868 6: KR920 7: IN865 8: US915 9: AS923-2 10: AS923-3 11: AS923-4 12: RU864
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.lora_region);

	return 0;
}

/**
 * @brief AT+BAND=xx Set regional frequency band
 *  Values: 0: AS923 1: AU915 2: CN470 3: CN779 4: EU433 5: EU868 6: KR920 7: IN865 8: US915 9: AS923-2 10: AS923-3 11: AS923-4 12: RU864
 * @return int 0 if valid parameter
 */
static int at_exec_region(char *str)
{
	char *param;
	uint8_t region;

	param = strtok(str, ",");
	if (param != NULL)
	{
		region = strtol(param, NULL, 0);
		// RAK4630 0: AS923 1: AU915 2: CN470 3: CN779 4: EU433 5: EU868 6: KR920 7: IN865 8: US915 9: AS923-2 10: AS923-3 11: AS923-4 12: RU864
		if (region > 12)
		{
			return -1;
		}
		g_lorawan_settings.lora_region = region;
		save_settings();
	}
	else
	{
		return -1;
	}

	return 0;
}

/**
 * @brief AT+MASK=? Get channel mask
 *  Only available for regions 1: AU915 2: CN470 8: US915
 * @return int always 0
 */
static int at_query_mask(void)
{
	if ((g_lorawan_settings.lora_region == 1) || (g_lorawan_settings.lora_region == 2) || (g_lorawan_settings.lora_region == 8))
	{
		snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.subband_channels);

		return 0;
	}
	return AT_ERRNO_PARA_VAL;
}

/**
 * @brief AT+MASK=xx Set channel mask
 *  Only available for regions 1: AU915 2: CN470 8: US915
 * @return int 0 if valid parameter
 */
static int at_exec_mask(char *str)
{
	char *param;
	uint8_t mask;

	param = strtok(str, ",");
	if (param != NULL)
	{
		mask = strtol(param, NULL, 0);

		uint8_t maxBand = 1;
		switch (g_lorawan_settings.lora_region)
		{
		case LORAMAC_REGION_AU915:
			maxBand = 9;
			break;
		case LORAMAC_REGION_CN470:
			maxBand = 12;
			break;
		case LORAMAC_REGION_US915:
			maxBand = 9;
			break;
		default:
			return AT_ERRNO_PARA_VAL;
		}
		if ((mask == 0) || (mask > maxBand))
		{
			return -1;
		}
		g_lorawan_settings.subband_channels = mask;
		save_settings();
	}
	else
	{
		return -1;
	}

	return 0;
}

/**
 * @brief AT+NJM=? Return current join modus
 * 
 * @return int always 0
 */
static int at_query_joinmode(void)
{
	int mode;
	mode = g_lorawan_settings.otaa_enabled == true ? 0 : 1;

	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", mode);
	return 0;
}

/**
 * @brief AT+NJM=x Set current join modus
 * 
 * @param str 0 = OTAA 1 = ABP
 * @return int 0 if valid parameter
 */
static int at_exec_joinmode(char *str)
{
	int mode = strtol(str, NULL, 0);

	if (mode != 0 && mode != 1)
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.otaa_enabled = (mode == 0 ? true : false);
	save_settings();

	return 0;
}

/**
 * @brief AT+DEVEUI=? Get current Device EUI
 * 
 * @return int always 0
 */
static int at_query_deveui(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE,
			 "%02X%02X%02X%02X%02X%02X%02X%02X",
			 g_lorawan_settings.node_device_eui[0],
			 g_lorawan_settings.node_device_eui[1],
			 g_lorawan_settings.node_device_eui[2],
			 g_lorawan_settings.node_device_eui[3],
			 g_lorawan_settings.node_device_eui[4],
			 g_lorawan_settings.node_device_eui[5],
			 g_lorawan_settings.node_device_eui[6],
			 g_lorawan_settings.node_device_eui[7]);
	return 0;
}

/**
 * @brief AT+DEVEUI=<XXXXXXXXXXXXXXXX> Set current Device EUI
 * @return int 0 if Dev EUI has correct length and was valid HEX
 */
static int at_exec_deveui(char *str)
{
	uint8_t len;
	uint8_t buf[8];

	len = hex2bin(str, buf, 8);

	if (len != 8)
	{
		return AT_ERRNO_PARA_VAL;
	}

	memcpy(g_lorawan_settings.node_device_eui, buf, 8);
	save_settings();

	return 0;
}

/**
 * @brief AT+APPEUI=? Get current Application (Join) EUI
 * 
 * @return int always 0
 */
static int at_query_appeui(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE,
			 "%02X%02X%02X%02X%02X%02X%02X%02X",
			 g_lorawan_settings.node_app_eui[0],
			 g_lorawan_settings.node_app_eui[1],
			 g_lorawan_settings.node_app_eui[2],
			 g_lorawan_settings.node_app_eui[3],
			 g_lorawan_settings.node_app_eui[4],
			 g_lorawan_settings.node_app_eui[5],
			 g_lorawan_settings.node_app_eui[6],
			 g_lorawan_settings.node_app_eui[7]);
	return 0;
}

/**
 * @brief AT+APPEUI=<XXXXXXXXXXXXXXXX> Set current Application (Join) EUI
 * 
 * @return int 0 if App EUI has correct length and was valid HEX
 */
static int at_exec_appeui(char *str)
{
	uint8_t len;
	uint8_t buf[8];

	len = hex2bin(str, buf, 8);
	if (len != 8)
	{
		return AT_ERRNO_PARA_VAL;
	}

	memcpy(g_lorawan_settings.node_app_eui, buf, 8);
	save_settings();

	return 0;
}

/**
 * @brief AT+APPKEY=? Get current Application Key
 * 
 * @return int always 0
 */
static int at_query_appkey(void)
{
	uint8_t i;
	uint8_t len = 0;

	for (i = 0; i < 16; i++)
	{
		len += snprintf(g_at_query_buf + len, ATQUERY_SIZE - len, "%02X", g_lorawan_settings.node_app_key[i]);
		if (ATQUERY_SIZE <= len)
		{
			return -1;
		}
	}
	return 0;
}

/**
 * @brief AT+APPKEY=<XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX> Set current Application (Join) EUI
 * 
 * @return int 0 if App Key has correct length and was valid HEX
 */
static int at_exec_appkey(char *str)
{
	uint8_t buf[16];
	uint8_t len;

	len = hex2bin(str, buf, 16);
	if (len != 16)
	{
		return AT_ERRNO_PARA_VAL;
	}

	memcpy(g_lorawan_settings.node_app_key, buf, 16);
	save_settings();

	return 0;
}

/**
 * @brief AT+DEVADDR=? Get device address
 * 
 * @return int always 0
 */
static int at_query_devaddr(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%08lX\n", g_lorawan_settings.node_dev_addr);
	return 0;
}

/**
 * @brief AT+DEVADDR=<XXXXXXXX> Set device address
 * 
 * @return int 0 if device address has correct length and was valid HEX
 */
static int at_exec_devaddr(char *str)
{
	int i;
	uint8_t len;
	uint8_t buf[4];
	uint8_t swap_buf[4];

	len = hex2bin(str, buf, 4);
	if (len != 4)
	{
		return AT_ERRNO_PARA_VAL;
	}

	for (i = 0; i < 4; i++)
	{
		swap_buf[i] = buf[3 - i];
	}

	memcpy(&g_lorawan_settings.node_dev_addr, swap_buf, 4);
	save_settings();

	return 0;
}

/**
 * @brief AT+APPSKEY=? Get application session key
 * 
 * @return int always 0
 */
static int at_query_appskey(void)
{
	uint8_t i;
	uint8_t len = 0;

	for (i = 0; i < 16; i++)
	{
		len += snprintf(g_at_query_buf + len, ATQUERY_SIZE - len, "%02X", g_lorawan_settings.node_apps_key[i]);
		if (ATQUERY_SIZE <= len)
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief AT+APPSKEY=<XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX> Set application session key
 * 
 * @return int 0 if App session Key has correct length and was valid HEX
 */
static int at_exec_appskey(char *str)
{
	uint8_t len;
	uint8_t buf[16];

	len = hex2bin(str, buf, 16);
	if (len != 16)
	{
		return AT_ERRNO_PARA_VAL;
	}

	memcpy(g_lorawan_settings.node_apps_key, buf, 16);
	save_settings();

	return 0;
}

/**
 * @brief AT+NWSKEY=? Get network session key
 * 
 * @return int always 0
 */
static int at_query_nwkskey(void)
{
	uint8_t i;
	uint8_t len = 0;

	for (i = 0; i < 16; i++)
	{
		len += snprintf(g_at_query_buf + len, ATQUERY_SIZE - len, "%02X", g_lorawan_settings.node_nws_key[i]);
		if (ATQUERY_SIZE <= len)
		{
			return -1;
		}
	}

	return 0;
}

/**
 * @brief AT+NWSKEY=<XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX> Set network session key
 * 
 * @return int 0 if Network session key has correct length and was valid HEX
 */
static int at_exec_nwkskey(char *str)
{
	uint8_t len;
	uint8_t buf[16];

	len = hex2bin(str, buf, 16);
	if (len != 16)
	{
		return AT_ERRNO_PARA_VAL;
	}

	memcpy(g_lorawan_settings.node_nws_key, buf, 16);
	save_settings();

	return 0;
}

/**
 * @brief AT+CLASS=? Get device class
 * 
 * @return int always 0
 */
static int at_query_class(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%c", g_lorawan_settings.lora_class + 65);

	return 0;
}

/**
 * @brief AT+CLASS=X Set device class
 * 
 * @param str Class A or C, B not supported
 * @return int if class was valid
 */
static int at_exec_class(char *str)
{
	uint8_t cls;
	char *param;

	param = strtok(str, ",");
	cls = (uint8_t)param[0];
	// Class B is not supported
	// if (cls != 'A' && cls != 'B' && cls != 'C')
	if (cls != 'A' && cls != 'C')
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.lora_class = cls - 65;
	save_settings();

	return 0;
}

/**
 * @brief AT+NJM=? Get join mode
 * 
 * @return int always 0
 */
static int at_query_join(void)
{
	// Param1 = Join command: 1 for joining the network , 0 for stop joining
	// Param2 = Auto-Join config: 1 for Auto-join on power up) , 0 for no auto-join.
	// Param3 = Reattempt interval: 7 - 255 seconds
	// Param4 = No. of join attempts: 0 - 255
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d,%d,%d,%d", 0, g_lorawan_settings.auto_join, 8, g_lorawan_settings.join_trials);

	return 0;
}

/**
 * @brief AT+NJM=<Param1>,<Param2>,<Param3>,<Param4> Set join mode
 * Param1 = Join command: 1 for joining the network , 0 for stop joining (not supported)
 * Param2 = Auto-Join config: 1 for Auto-join on power up) , 0 for no auto-join.
 * Param3 = Reattempt interval: 7 - 255 seconds (ignored)
 * Param4 = No. of join attempts: 0 - 255
 * 
 * @param str parameters as string
 * @return int 0 if all parameters were valid
 */
static int at_exec_join(char *str)
{
	uint8_t bJoin;
	uint8_t autoJoin;
	uint8_t nbtrials;
	char *param;

	param = strtok(str, ":");

	/* check start or stop join parameter */
	bJoin = strtol(param, NULL, 0);
	if (bJoin != 1 && bJoin != 0)
	{
		return AT_ERRNO_PARA_VAL;
	}

	/* check auto join parameter */
	param = strtok(NULL, ":");
	if (param != NULL)
	{
		autoJoin = strtol(param, NULL, 0);
		if (autoJoin != 1 && autoJoin != 0)
		{
			return AT_ERRNO_PARA_VAL;
		}
		g_lorawan_settings.auto_join = (autoJoin == 1 ? true : false);

		param = strtok(NULL, ":");
		if (param != NULL)
		{
			// join interval, not support yet.

			// join attemps number
			param = strtok(NULL, ":");
			if (param != NULL)
			{
				nbtrials = strtol(param, NULL, 0);
				if (nbtrials == 0)
				{
					return AT_ERRNO_PARA_VAL;
				}
				g_lorawan_settings.join_trials = nbtrials;
			}
		}
		save_settings();

		if ((bJoin == 1) && !g_lorawan_initialized) // ==0 stop join, not support, yet
		{
			APP_LOG("AT", "Initialize LoRaWAN and start join");
			// Manual join only works if LoRaWAN was not initialized yet.
			// If LoRaWAN was already initialized, a restart is required
			// Wake up task to request join
			if (loop_thread != NULL)
			{
				APP_LOG("AT", "Request Join");
				osSignalSet(loop_thread, SIGNAL_JOIN);
			}
		}

		if ((bJoin == 1) && g_lorawan_initialized && (lmh_join_status_get() != LMH_SET))
		{
			// If if not yet joined, start join
			delay(100);
			lmh_join();
			return 0;
		}

		// if ((bJoin == 1) && g_lorawan_settings.auto_join)
		// {
		// 	// If auto join is set, restart the device to start join process
		// 	delay(100);
		// 	NVIC_SystemReset();
		// 	return 0;
		// }
	}

	return 0;
}

/**
 * @brief AT+NJS Get current join status
 * 
 * @return int always 0
 */
static int at_query_join_status()
{
	uint8_t join_status;

	join_status = (uint8_t)lmh_join_status_get();
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", join_status);

	return 0;
}

/**
 * @brief AT+CFM=? Get current confirm/unconfirmed packet status
 * 
 * @return int always 0
 */
static int at_query_confirm(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.confirmed_msg_enabled);
	return 0;
}

/**
 * @brief AT+CFM=X Set confirmed / unconfirmed packet sending
 * 
 * @param str 0 = unconfirmed 1 = confirmed packet
 * @return int 0 if correct parameter
 */
static int at_exec_confirm(char *str)
{
	int cfm;

	cfm = strtol(str, NULL, 0);
	if (cfm != 0 && cfm != 1)
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.confirmed_msg_enabled = (lmh_confirm)cfm;
	save_settings();

	return 0;
}

/**
 * @brief AT+DR=? Get current datarate
 * 
 * @return int always 0
 */
static int at_query_datarate(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.data_rate);
	return 0;
}

/**
 * @brief AT+DR=X Set datarate
 * 
 * @param str 0 to 15, depending on region
 * @return int 0 if correct parameter
 */
static int at_exec_datarate(char *str)
{
	uint8_t datarate;

	datarate = strtol(str, NULL, 0);

	if (datarate > 15)
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.data_rate = datarate;
	save_settings();

	return 0;
}

/**
 * @brief AT+ADR=? Get current adaptive datarate status
 * 
 * @return int always 0
 */
static int at_query_adr(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.adr_enabled ? 1 : 0);
	return 0;
}

/**
 * @brief AT+ADR=X Enable/disable adaptive datarate
 * 
 * @param str 0 = disable, 1 = enable ADR
 * @return int 0 if correct parameter
 */
static int at_exec_adr(char *str)
{
	int adr;

	adr = strtol(str, NULL, 0);
	if (adr != 0 && adr != 1)
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.adr_enabled = (adr == 1 ? true : false);

	save_settings();

	return 0;
}

/**
 * @brief AT+TXP=? Get current TX power setting
 * 
 * @return int always 0
 */
static int at_query_txpower(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_lorawan_settings.tx_power);
	return 0;
}

/**
 * @brief AT+TXP Set TX power
 * 
 * @param str TX power 0 to 10
 * @return int always 0
 */
static int at_exec_txpower(char *str)
{
	uint8_t tx_power;

	tx_power = strtol(str, NULL, 0);

	if (tx_power > 10)
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.tx_power = tx_power;

	save_settings();

	return 0;
}

/**
 * @brief AT+SENDFREQ=? Get current send frequency
 * 
 * @return int always 0
 */
static int at_query_sendfreq(void)
{
	// Return time in seconds, but it is saved in milli seconds
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", (g_lorawan_settings.send_repeat_time == 0) ? 0 : (int)(g_lorawan_settings.send_repeat_time / 1000));

	return 0;
}

/**
 * @brief AT+SENDFREQ=<value> Set current send frequency
 * 
 * @param str send frequency in seconds between 0 (disabled) and 3600
 * @return int 
 */
static int at_exec_sendfreq(char *str)
{
	long time = strtol(str, NULL, 0);

	if ((time < 0) || (time > 3600))
	{
		return AT_ERRNO_PARA_VAL;
	}

	g_lorawan_settings.send_repeat_time = time * 1000;
	save_settings();

	return 0;
}

static int at_exec_send(char *str)
{
	if (!g_lpwan_has_joined)
	{
		return AT_ERRNO_NOALLOW;
	}

	// Get fPort
	char *param;

	param = strtok(str, ":");
	uint16_t fPort = strtol(param, NULL, 0);
	if ((fPort == 0) || (fPort > 255))
	{
		return AT_ERRNO_PARA_VAL;
	}

	// Get data to send
	param = strtok(NULL,":");
	int data_size = strlen(param);
	if (!(data_size % 2 == 0) || (data_size > 254))
	{
		return AT_ERRNO_PARA_VAL;
	}

	int buff_idx = 0;
	char buff_parse[3];
	for (int idx = 0; idx <= data_size + 1; idx += 2)
	{
		buff_parse[0] = param[idx];
		buff_parse[1] = param[idx + 1];
		buff_parse[2] = 0;
		m_lora_app_data_buffer[buff_idx] = strtol(buff_parse, NULL, 16);
		buff_idx++;
	}
	send_lora_packet(m_lora_app_data_buffer, data_size / 2, fPort);
	return 0;
}

/**
 * @brief AT+BATT=? Get current battery value (0 to 255)
 * 
 * @return int always 0
 */
static int at_query_battery(void)
{
	// Battery level is from 1 to 254, 254 meaning fully charged.
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", get_lora_batt());

	return 0;
}

/**
 * @brief AT+RSSI=? Get RSSI of last received package
 * 
 * @return int always 0
 */
static int at_query_rssi(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_last_rssi);

	return 0;
}

/**
 * @brief AT+SNR=? Get SNR of last received package
 * 
 * @return int always 0
 */
static int at_query_snr(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d", g_last_snr);

	return 0;
}

/**
 * @brief AT+VER=? Get firmware version and build date
 * 
 * @return int always 0
 */
static int at_query_version(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%d.%d.%d %s %s", g_sw_ver_1, g_sw_ver_2, g_sw_ver_3, __DATE__, __TIME__);

	return 0;
}

static int at_query_status(void)
{
	log_settings();
	snprintf(g_at_query_buf, ATQUERY_SIZE, " ");

	return 0;
}

/**
 * @brief ATZ Initiate a system reset
 * 
 * @return int always 0
 */
static int at_exec_reboot(void)
{
	delay(100);
	NVIC_SystemReset();
	return 0;
}

/**
 * @brief ATR Restore flash defaults
 * 
 * @return int always 0
 */
static int at_exec_restore(void)
{
	flash_reset();
	return 0;
}

static int at_exec_list_all(void);

/**
 * @brief List of all available commands with short help and pointer to functions
 * 
 */
static atcmd_t g_at_cmd_list[] = {
	/*|    CMD    |     AT+CMD?      |    AT+CMD=?    |  AT+CMD=value |  AT+CMD  |*/
	// General commands
	{"?", "AT commands", NULL, NULL, at_exec_list_all},
	{"R", "Restore default", NULL, NULL, at_exec_restore},
	{"Z", "Trig a MCU reset", NULL, NULL, at_exec_reboot},
	// LoRaWAN keys, ID's EUI's
	{"+APPEUI", "Get or set the application EUI", at_query_appeui, at_exec_appeui, NULL},
	{"+APPKEY", "Get or set the application key", at_query_appkey, at_exec_appkey, NULL},
	{"+DEVEUI", "Get or set the device EUI", at_query_deveui, at_exec_deveui, NULL},
	{"+APPSKEY", "Get or set the application session key", at_query_appskey, at_exec_appskey, NULL},
	{"+NWKSKEY", "Get or Set the network session key", at_query_nwkskey, at_exec_nwkskey, NULL},
	{"+DEVADDR", "Get or set the device address", at_query_devaddr, at_exec_devaddr, NULL},
	// Joining and sending data on LoRa network
	{"+CFM", "Get or set the confirm mode", at_query_confirm, at_exec_confirm, NULL},
	{"+JOIN", "Join network", at_query_join, at_exec_join, NULL},
	{"+NJS", "Get the join status", at_query_join_status, NULL, NULL},
	{"+NJM", "Get or set the network join mode", at_query_joinmode, at_exec_joinmode, NULL},
	{"+SENDFREQ", "Get or Set the automatic send time", at_query_sendfreq, at_exec_sendfreq, NULL},
	{"+SEND", "Send data", NULL, at_exec_send, NULL},
	// LoRa network management
	{"+ADR", "Get or set the adaptive data rate setting", at_query_adr, at_exec_adr, NULL},
	{"+CLASS", "Get or set the device class", at_query_class, at_exec_class, NULL},
	{"+DR", "Get or Set the Tx DataRate=[0..7]", at_query_datarate, at_exec_datarate, NULL},
	{"+TXP", "Get or set the transmit power", at_query_txpower, at_exec_txpower, NULL},
	{"+BAND", "Get and Set number corresponding to active regions", at_query_region, at_exec_region, NULL},
	{"+MASK", "Get and Set channels mask", at_query_mask, at_exec_mask, NULL},
	// Status queries
	{"+BAT", "Get battery level", at_query_battery, NULL, NULL},
	{"+RSSI", "Last RX packet RSSI", at_query_rssi, NULL, NULL},
	{"+SNR", "Last RX packet SNR", at_query_snr, NULL, NULL},
	{"+VER", "Get SW version", at_query_version, NULL, NULL},
	{"+STATUS", "Show LoRaWAN status", at_query_status, NULL, NULL},
};

/**
 * @brief List all available commands with short help
 * 
 * @return int always 0
 */
static int at_exec_list_all(void)
{
	AT_PRINTF("\r\n+++++++++++++++\r\n");
	AT_PRINTF("AT command list\r\n");
	AT_PRINTF("+++++++++++++++\r\n");

	for (unsigned int idx = 0; idx < sizeof(g_at_cmd_list) / sizeof(atcmd_t); idx++)
	{
		if (idx < 4)
		{
			AT_PRINTF("AT%s\t\t%s\r\n", g_at_cmd_list[idx].cmd_name, g_at_cmd_list[idx].cmd_desc);
		}
		else
		{
			AT_PRINTF("AT%s\t%s\r\n", g_at_cmd_list[idx].cmd_name, g_at_cmd_list[idx].cmd_desc);
		}
	}
	AT_PRINTF("+++++++++++++++\r\n");
	return 0;
}

/**
 * @brief Handle received AT command
 * 
 */
static void at_cmd_handle(void)
{
	uint8_t i;
	int ret = 0;
	const char *cmd_name;
	char *rxcmd = atcmd + 2;
	int16_t tmp = atcmd_index - 2;
	uint16_t rxcmd_index;

	if (atcmd_index < 2 || rxcmd[tmp] != '\0')
	{
		atcmd_index = 0;
		memset(atcmd, 0xff, ATCMD_SIZE);
		return;
	}

	// Serial.printf("atcmd_index==%d=%s==\n", atcmd_index, atcmd);
	if (atcmd_index == 2 && strncmp(atcmd, "AT", atcmd_index) == 0)
	{
		atcmd_index = 0;
		memset(atcmd, 0xff, ATCMD_SIZE);
		AT_PRINTF("\r\nOK\r\n");
		return;
	}

	rxcmd_index = tmp;

	for (i = 0; i < sizeof(g_at_cmd_list) / sizeof(atcmd_t); i++)
	{
		cmd_name = g_at_cmd_list[i].cmd_name;
		// Serial.printf("===rxcmd========%s================cmd_name=====%s====%d===\n", rxcmd, cmd_name, strlen(cmd_name));
		if (strlen(cmd_name) && strncmp(rxcmd, cmd_name, strlen(cmd_name)) != 0)
		{
			continue;
		}

		// Serial.printf("===rxcmd_index========%d================strlen(cmd_name)=====%d=======\n", rxcmd_index, strlen(cmd_name));

		if (rxcmd_index == (strlen(cmd_name) + 1) &&
			rxcmd[strlen(cmd_name)] == '?')
		{
			/* test cmd */
			if (g_at_cmd_list[i].cmd_desc)
			{
				if (strncmp(g_at_cmd_list[i].cmd_desc, "OK", 2) == 0)
				{
					snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
				}
				else
				{
					snprintf(atcmd, ATCMD_SIZE, "\r\nAT%s: %s\r\nOK\r\n",
							 cmd_name, g_at_cmd_list[i].cmd_desc);
				}
			}
			else
			{
				snprintf(atcmd, ATCMD_SIZE, "\r\nAT%s\r\nOK\r\n", cmd_name);
			}
		}
		else if (rxcmd_index == (strlen(cmd_name) + 2) &&
				 strcmp(&rxcmd[strlen(cmd_name)], "=?") == 0)
		{
			/* query cmd */
			if (g_at_cmd_list[i].query_cmd != NULL)
			{
				ret = g_at_cmd_list[i].query_cmd();

				if (ret == 0)
				{
					snprintf(atcmd, ATCMD_SIZE, "\r\n%s:%s\r\nOK\r\n",
							 cmd_name, g_at_query_buf);
				}
			}
			else
			{
				ret = AT_ERRNO_NOALLOW;
			}
		}
		else if (rxcmd_index > (strlen(cmd_name) + 1) &&
				 rxcmd[strlen(cmd_name)] == '=')
		{
			/* exec cmd */
			if (g_at_cmd_list[i].exec_cmd != NULL)
			{
				ret = g_at_cmd_list[i].exec_cmd(rxcmd + strlen(cmd_name) + 1);
				if (ret == 0)
				{
					snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
				}
				else if (ret == -1)
				{
					ret = AT_ERRNO_SYS;
				}
			}
			else
			{
				ret = AT_ERRNO_NOALLOW;
			}
		}
		else if (rxcmd_index == strlen(cmd_name))
		{
			/* exec cmd without parameter*/
			if (g_at_cmd_list[i].exec_cmd_no_para != NULL)
			{
				ret = g_at_cmd_list[i].exec_cmd_no_para();
				if (ret == 0)
				{
					snprintf(atcmd, ATCMD_SIZE, "\r\nOK\r\n");
				}
				else if (ret == -1)
				{
					ret = AT_ERRNO_SYS;
				}
			}
			else
			{
				ret = AT_ERRNO_NOALLOW;
			}
		}
		else
		{
			continue;
		}
		break;
	}

	if (i == sizeof(g_at_cmd_list) / sizeof(atcmd_t))
	{

		ret = AT_ERRNO_NOSUPP;
	}

	if (ret != 0 && ret != AT_CB_PRINT)
	{
		snprintf(atcmd, ATCMD_SIZE, "\r\n%s%x\r\n", AT_ERROR, ret);
	}

	if (ret != AT_CB_PRINT)
	{
		AT_PRINTF(atcmd);
	}

	atcmd_index = 0;
	memset(atcmd, 0xff, ATCMD_SIZE);
	return;
}

/**
 * @brief Get Serial input and start parsing
 * 
 * @param cmd received character
 */
void at_serial_input(uint8_t cmd)
{
	DualSerial("%c", cmd);

	// Handle backspace
	if (cmd == '\b')
	{
		atcmd[atcmd_index--] = '\0';
		DualSerial(" \b");
	}

	if ((cmd >= '0' && cmd <= '9') || (cmd >= 'a' && cmd <= 'z') ||
		(cmd >= 'A' && cmd <= 'Z') || cmd == '?' || cmd == '+' || cmd == ':' ||
		cmd == '=' || cmd == ' ' || cmd == ',')
	{
		atcmd[atcmd_index++] = cmd;
	}
	else if (cmd == '\r' || cmd == '\n')
	{
		atcmd[atcmd_index] = '\0';
		at_cmd_handle();
	}

	if (atcmd_index >= ATCMD_SIZE)
	{
		atcmd_index = 0;
	}
}
