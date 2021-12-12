/**
 * @file flash.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Fake flash functions. /// \todo find option to use flash to store credentials
 * @version 0.1
 * @date 2021-10-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"
#include <hardware/flash.h>

uint16_t g_sw_ver_1 = 1; // major version increase on API change / not backwards compatible
uint16_t g_sw_ver_2 = 0; // minor version increase on API change / backward compatible
uint16_t g_sw_ver_3 = 0; // patch version increase on bugfix, no affect on API

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

void make_credentials(void)
{
	uint8_t pico_id[8];

	// Get flash ID
	getUniqueDeviceID(pico_id);

#if APP_DEBUG > 0
	Serial.print("RP2040 Flash ID: ");
	for (int idx = 0; idx < 8; idx++)
	{
		Serial.printf("%02X", pico_id[idx]);
	}
	Serial.println("");
#endif

	// Create LoRaWAN credentials from flash ID
	int idx_1 = 0;
	int idx_2 = 7;
	for (int idx = 0; idx < 8; idx++)
	{
		g_lorawan_settings.node_device_eui[idx_1] = pico_id[idx];
		g_lorawan_settings.node_app_eui[idx_2] = pico_id[idx];
		idx_1++;
		idx_2--;
	}

	idx_1 = 0;
	idx_2 = 15;
	for (int idx = 0; idx < 8; idx++)
	{
		g_lorawan_settings.node_app_key[idx_1] = pico_id[idx];
		g_lorawan_settings.node_app_key[idx_2] = pico_id[idx];
		g_lorawan_settings.node_nws_key[idx_1] = pico_id[idx];
		g_lorawan_settings.node_nws_key[idx_2] = pico_id[idx];
		g_lorawan_settings.node_apps_key[idx_2] = pico_id[idx];
		g_lorawan_settings.node_apps_key[idx_1] = pico_id[idx];
		idx_1++;
		idx_2--;
	}

	g_lorawan_settings.node_dev_addr = ((pico_id[0] + pico_id[1]) << 24) | ((pico_id[2] + pico_id[3]) << 18) | ((pico_id[4] + pico_id[5]) << 8) | ((pico_id[6] + pico_id[7]));

#if APP_DEBUG > 0
	Serial.print("DevEUI = ");
	for (int idx = 0; idx < 8; idx++)
	{
		Serial.printf("%02X", g_lorawan_settings.node_device_eui[idx]);
	}
	Serial.print("\n\nAppEUI = ");
	for (int idx = 0; idx < 8; idx++)
	{
		Serial.printf("%02X", g_lorawan_settings.node_app_eui[idx]);
	}
	Serial.print("\n\nAppKey = ");
	for (int idx = 0; idx < 16; idx++)
	{
		Serial.printf("%02X", g_lorawan_settings.node_app_key[idx]);
	}
	Serial.print("\n\nNwsKey = ");
	for (int idx = 0; idx < 16; idx++)
	{
		Serial.printf("%02X", g_lorawan_settings.node_nws_key[idx]);
	}
	Serial.print("\n\nAppsKey = ");
	for (int idx = 0; idx < 16; idx++)
	{
		Serial.printf("%02X", g_lorawan_settings.node_apps_key[idx]);
	}
	Serial.printf("\n\nDevice Addr = %08lX\n", g_lorawan_settings.node_dev_addr);
#endif
}

void init_flash(void)
{
	// Check if valid data is in the flash
	s_lorawan_settings flash_settings;

	APP_LOG("FLASH", "Flash size %lX - Flash sector size %lX", PICO_FLASH_SIZE_BYTES, FLASH_SECTOR_SIZE);
	APP_LOG("FLASH", "Trying to read from Flash address %lX", XIP_BASE + FLASH_TARGET_OFFSET);
	APP_LOG("FLASH", "Data size %ld", sizeof(s_lorawan_settings));
	APP_LOG("FLASH", "Data size %ld", sizeof(flash_settings));

	uint32_t ints = save_and_disable_interrupts();
	memcpy((void *)&flash_settings, (void *)(XIP_BASE + FLASH_TARGET_OFFSET), sizeof(flash_settings));
	restore_interrupts(ints);

	APP_LOG("FLASH", "Mark1: %0X Mark2: %0X", flash_settings.valid_mark_1, flash_settings.valid_mark_2);
	APP_LOG("FLASH", "Region: %d", flash_settings.lora_region);

	if ((flash_settings.valid_mark_1 != 0xAA) || (flash_settings.valid_mark_2 != LORAWAN_DATA_MARKER))
	{
		APP_LOG("FLASH", "No valid data found");

		// Create default settings from devices flash ID
		make_credentials();

		// Write new data to the flash
		eraseDataFlash();
		writeDataToFlash((uint8_t *)&g_lorawan_settings);
	}
	else
	{
		APP_LOG("FLASH", "Found valid data in flash");
		memcpy((void *)&g_lorawan_settings, (void *)&flash_settings, sizeof(flash_settings));
	}
}

bool save_settings(void)
{
	// Get settings from flash
	s_lorawan_settings flash_settings;
	uint32_t ints = save_and_disable_interrupts();
	memcpy((void *)&flash_settings, (void *)(XIP_BASE + FLASH_TARGET_OFFSET), sizeof(flash_settings));
	restore_interrupts(ints);

	// Compare flash content with current settings
	if (memcmp((void *)&flash_settings, (void *)&g_lorawan_settings, sizeof(s_lorawan_settings)) != 0)
	{
		APP_LOG("FLASH", "Flash content changed, writing new data");
		// Write new data to the flash
		eraseDataFlash();
		writeDataToFlash((uint8_t *)&g_lorawan_settings);
	}
	else
	{
		APP_LOG("FLASH", "Flash content identical no need to write");
	}
	return true;
}

void flash_reset(void)
{
	// Get a fresh structure
	s_lorawan_settings new_settings;
	memcpy((void *)&g_lorawan_settings, (void *)&new_settings, sizeof(new_settings));
	// Create default settings from devices flash ID
	make_credentials();
	// Erase the flash, then call init_flash to restore defaults
	eraseDataFlash();
	writeDataToFlash((uint8_t *)&g_lorawan_settings);
}

/**
 * @brief Printout of all settings
 * 
 */
void log_settings(void)
{
	DualSerial("LoRaWAN status:\n");
	DualSerial("   Auto join %s\n", g_lorawan_settings.auto_join ? "enabled" : "disabled");
	DualSerial("   OTAA %s\n", g_lorawan_settings.otaa_enabled ? "enabled" : "disabled");
	DualSerial("   Dev EUI %02X%02X%02X%02X%02X%02X%02X%02X\n", g_lorawan_settings.node_device_eui[0], g_lorawan_settings.node_device_eui[1],
			   g_lorawan_settings.node_device_eui[2], g_lorawan_settings.node_device_eui[3],
			   g_lorawan_settings.node_device_eui[4], g_lorawan_settings.node_device_eui[5],
			   g_lorawan_settings.node_device_eui[6], g_lorawan_settings.node_device_eui[7]);
	DualSerial("   App EUI %02X%02X%02X%02X%02X%02X%02X%02X\n", g_lorawan_settings.node_app_eui[0], g_lorawan_settings.node_app_eui[1],
			   g_lorawan_settings.node_app_eui[2], g_lorawan_settings.node_app_eui[3],
			   g_lorawan_settings.node_app_eui[4], g_lorawan_settings.node_app_eui[5],
			   g_lorawan_settings.node_app_eui[6], g_lorawan_settings.node_app_eui[7]);
	DualSerial("   App Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
			   g_lorawan_settings.node_app_key[0], g_lorawan_settings.node_app_key[1],
			   g_lorawan_settings.node_app_key[2], g_lorawan_settings.node_app_key[3],
			   g_lorawan_settings.node_app_key[4], g_lorawan_settings.node_app_key[5],
			   g_lorawan_settings.node_app_key[6], g_lorawan_settings.node_app_key[7],
			   g_lorawan_settings.node_app_key[8], g_lorawan_settings.node_app_key[9],
			   g_lorawan_settings.node_app_key[10], g_lorawan_settings.node_app_key[11],
			   g_lorawan_settings.node_app_key[12], g_lorawan_settings.node_app_key[13],
			   g_lorawan_settings.node_app_key[14], g_lorawan_settings.node_app_key[15]);
	DualSerial("   NWS Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
			   g_lorawan_settings.node_nws_key[0], g_lorawan_settings.node_nws_key[1],
			   g_lorawan_settings.node_nws_key[2], g_lorawan_settings.node_nws_key[3],
			   g_lorawan_settings.node_nws_key[4], g_lorawan_settings.node_nws_key[5],
			   g_lorawan_settings.node_nws_key[6], g_lorawan_settings.node_nws_key[7],
			   g_lorawan_settings.node_nws_key[8], g_lorawan_settings.node_nws_key[9],
			   g_lorawan_settings.node_nws_key[10], g_lorawan_settings.node_nws_key[11],
			   g_lorawan_settings.node_nws_key[12], g_lorawan_settings.node_nws_key[13],
			   g_lorawan_settings.node_nws_key[14], g_lorawan_settings.node_nws_key[15]);
	DualSerial("   Apps Key %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n",
			   g_lorawan_settings.node_apps_key[0], g_lorawan_settings.node_apps_key[1],
			   g_lorawan_settings.node_apps_key[2], g_lorawan_settings.node_apps_key[3],
			   g_lorawan_settings.node_apps_key[4], g_lorawan_settings.node_apps_key[5],
			   g_lorawan_settings.node_apps_key[6], g_lorawan_settings.node_apps_key[7],
			   g_lorawan_settings.node_apps_key[8], g_lorawan_settings.node_apps_key[9],
			   g_lorawan_settings.node_apps_key[10], g_lorawan_settings.node_apps_key[11],
			   g_lorawan_settings.node_apps_key[12], g_lorawan_settings.node_apps_key[13],
			   g_lorawan_settings.node_apps_key[14], g_lorawan_settings.node_apps_key[15]);
	DualSerial("   Dev Addr %08lX\n", g_lorawan_settings.node_dev_addr);
	DualSerial("   Repeat time %ld\n", g_lorawan_settings.send_repeat_time);
	DualSerial("   ADR %s\n", g_lorawan_settings.adr_enabled ? "enabled" : "disabled");
	DualSerial("   %s Network\n", g_lorawan_settings.public_network ? "Public" : "Private");
	DualSerial("   Dutycycle %s\n", g_lorawan_settings.duty_cycle_enabled ? "enabled" : "disabled");
	DualSerial("   Join trials %d\n", g_lorawan_settings.join_trials);
	DualSerial("   TX Power %d\n", g_lorawan_settings.tx_power);
	DualSerial("   DR %d\n", g_lorawan_settings.data_rate);
	DualSerial("   Class %d\n", g_lorawan_settings.lora_class);
	DualSerial("   Subband %d\n", g_lorawan_settings.subband_channels);
	DualSerial("   Fport %d\n", g_lorawan_settings.app_port);
	DualSerial("   %s Message\n", g_lorawan_settings.confirmed_msg_enabled ? "Confirmed" : "Unconfirmed");
	DualSerial("   Region %d\n", g_lorawan_settings.lora_region);
	DualSerial("   Network %s\n", g_lpwan_has_joined ? "joined" : "not joined");
}
