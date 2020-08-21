/**
 * @file Read_Battery_Level.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read charging level from a battery connected to the RAK5005-O base board
 * Setup and main loop
 * @version 0.1
 * @date 2020-08-21
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
#include <bluefruit.h>

#define PIN_VBAT A0

uint32_t vbat_pin = PIN_VBAT;

#define VBAT_MV_PER_LSB (0.73242188F) / / 3.0V ADC range and 12 - bit ADC resolution = 3000mV / 4096
#define VBAT_DIVIDER (0.4F)		 // 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER_COMP (1.73) // (1.403F) // Compensation factor for the VBAT divider

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

#include <bluefruit.h>
void initBLE();
extern bool bleUARTisConnected;
extern BLEUart bleuart;

char bleString[256] = {0};

void initDisplay(void);
void dispAddLine(char *line);
char dispText[256] = {0};

float readVBAT(void)
{
	float raw;

	// Get the raw 12-bit, 0..3000mV ADC value
	raw = analogRead(vbat_pin);

	// // Set the ADC back to the default settings
	// analogReference(AR_DEFAULT);
	// analogReadResolution(10);

	// Convert the raw value to compensated mv, taking the resistor-
	// divider into account (providing the actual LIPO voltage)
	// ADC range is 0..3000mV and resolution is 12-bit (0..4095)
	return raw * REAL_VBAT_MV_PER_LSB;
}

uint8_t mvToPercent(float mvolts)
{
	if (mvolts < 3300)
		return 0;

	if (mvolts < 3600)
	{
		mvolts -= 3300;
		return mvolts / 30;
	}

	mvolts -= 3600;
	return 10 + (mvolts * 0.15F); // thats mvolts /6.66666666
}

uint8_t mvToLoRaWanBattVal(float mvolts)
{ // * 2.55
	if (mvolts < 3300)
		return 0;

	if (mvolts < 3600)
	{
		mvolts -= 3300;
		return mvolts / 30 * 2.55;
	}

	mvolts -= 3600;
	return (10 + (mvolts * 0.15F)) * 2.55;
}

void setup()
{
	initDisplay();

	Serial.begin(115200);
	dispAddLine("Waiting for Serial");
	time_t serialTimeout = millis();
	while (!Serial)
	{
		delay(10); // for nrf52840 with native usb
		if ((millis() - serialTimeout) > 5000)
		{
			break;
		}
	}

	Serial.println("=====================================");
	Serial.println("RAK4631 Battery test");
	Serial.println("=====================================");

	// Start BLE
	dispAddLine("Init BLE");
	initBLE();

	// Set the analog reference to 3.0V (default = 3.6V)
	analogReference(AR_INTERNAL_3_0);

	// Set the resolution to 12-bit (0..4095)
	analogReadResolution(12); // Can be 8, 10, 12 or 14

	// Let the ADC settle
	delay(1);

	// Get a single ADC sample and throw it away
	readVBAT();
}

void loop()
{
	// Get a raw ADC reading
	float vbat_mv = readVBAT();

	// Convert from raw mv to percentage (based on LIPO chemistry)
	uint8_t vbat_per = mvToPercent(vbat_mv);

	// Display the results

	snprintf(dispText, 255, "%.3fV \t %d%%", vbat_mv / 1000, vbat_per);
	dispAddLine(dispText);

	Serial.print("LIPO = ");
	Serial.print(vbat_mv);
	Serial.print(" mV (");
	Serial.print(vbat_per);
	Serial.print("%) LoRaWan Batt ");
	Serial.println(mvToLoRaWanBattVal(vbat_mv));

	if (bleUARTisConnected)
	{
		if (bleUARTisConnected)
		{
			snprintf(bleString, 255, "%.0fmV %d%% %d/255\n", vbat_mv, vbat_per, mvToLoRaWanBattVal(vbat_mv));
			bleuart.print(bleString);
		}
	}
	delay(1000);
}
