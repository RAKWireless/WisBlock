/**
 * @file main.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Test RAK4631 QSPI flash
 * @version 0.1
 * @date 2020-07-30
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
 * IO1 <-> P0.17 (Arduino GPIO number 17)
 * IO2 <-> P1.02 (Arduino GPIO number 34)
 * IO3 <-> P0.21 (Arduino GPIO number 21)
 * IO4 <-> P0.04 (Arduino GPIO number 4)
 * IO5 <-> P0.09 (Arduino GPIO number 9)
 * IO6 <-> P0.10 (Arduino GPIO number 10)
 * SW1 <-> P0.01 (Arduino GPIO number 1)
 */

#include <Arduino.h>
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"

/** Adafruit QSPI Flash Transport */
Adafruit_FlashTransport_QSPI flashTransport;
/** Adafruit SPI Flash */
Adafruit_SPIFlash flash(&flashTransport);

/** Flash definition structure for ISSI IS25LP080D NOR Flash */
SPIFlash_Device_t rakFlash{
	.total_size = (1 << 23), /* 8 MiB */
	.start_up_time_us = 300,
	.manufacturer_id = 0x9D,
	.memory_type = 0x60,
	.capacity = 0x14,
	.max_clock_speed_mhz = 133,
	.quad_enable_bit_mask = 0x40,
	.has_sector_protection = false,
	.supports_fast_read = true,
	.supports_qspi = true,
	.supports_qspi_writes = true,
	.write_status_register_split = false,
	.single_status_byte = true,
};
/** Buffer to write to Flash */
uint8_t flashBuffer[512] = {0};
/** Buffer to read from Flash */
uint8_t buf[512];

/**
 * @brief Arduino setup function
 * @note Called once after power on or reset
 * 
 */
void setup(void)
{
	// Initialize builtin LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, HIGH);

	// Initialize Serial for debug output
	Serial.begin(115200);

	// Wait for USB Serial to be ready
	time_t timeout = millis();
	while (!Serial)
	{
		if ((millis() - timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		}
		else
		{
			break;
		}
	}

	digitalWrite(LED_BUILTIN, LOW);
	Serial.println("=====================================");
	Serial.println("RAK4631 Flash test");
	Serial.println("=====================================");

	// Fill the buffer with some values
	flashBuffer[0] = 0xAA;
	flashBuffer[1] = 0x55;
	flashBuffer[2] = 0x00;
	flashBuffer[3] = 0xFF;

	for (int i = 4; i < 512; i++)
	{
		flashBuffer[i] = i;
	}

	// Start access to the flash
	if (!flash.begin(&rakFlash))
	{
		Serial.println("Flash access failed, check the settings");
	}
	flash.waitUntilReady();

	// Get Flash information
	Serial.print("JEDEC ID: ");
	Serial.print(flash.getJEDECID(), HEX);
	Serial.print(" Flash size: ");
	Serial.print(flash.size());
	Serial.print(" # pages: ");
	Serial.print(flash.numPages());
	Serial.print(" page size: ");
	Serial.println(flash.pageSize());

	// Get content of the flash
	flash.readBuffer(0, buf, 256);
	Serial.println("First 16 bytes before erasing Flash");
	Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
				  buf[0], buf[1], buf[2], buf[3],
				  buf[4], buf[5], buf[6], buf[7],
				  buf[8], buf[9], buf[10], buf[11],
				  buf[12], buf[13], buf[14], buf[15]);

	// Erase the Flash content
	Serial.println("Erase Flash");
	flash.eraseChip();
	flash.waitUntilReady();

	// Read the content to see if erasing worked
	flash.readBuffer(0, buf, 256);
	Serial.println("First 16 bytes after erasing Flash");
	Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
				  buf[0], buf[1], buf[2], buf[3],
				  buf[4], buf[5], buf[6], buf[7],
				  buf[8], buf[9], buf[10], buf[11],
				  buf[12], buf[13], buf[14], buf[15]);

	// Write new data to the Flash
	Serial.println("Writing 0xAA, 0x55, 0x00, 0xFF");

	flash.writeBuffer(0, flashBuffer, 512);
	flash.waitUntilReady();

	Serial.println("Write done");

	// Read the content to see if write worked
	flash.readBuffer(0, buf, 16);
	Serial.println("First 16 bytes after writing new content");
	Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
				  buf[0], buf[1], buf[2], buf[3],
				  buf[4], buf[5], buf[6], buf[7],
				  buf[8], buf[9], buf[10], buf[11],
				  buf[12], buf[13], buf[14], buf[15]);

	// That's all folks, we loop here until next time
	while (1)
	{
		delay(300);
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
	}
}

/**
 * @brief Arduino loop function
 * @note We do nothing here in this example
 * 
 */
void loop(void)
{
}
