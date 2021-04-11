/**
 * @file Read_Battery_Level.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read charging level from a battery connected to the RAK5005-O base board
 * Setup and main loop
 * @version 0.1
 * @date 2020-08-21
 * 
 * @copyright Copyright (c) 2020 
 */
#include <Arduino.h>
#include <U8g2lib.h>

#define PIN_VBAT WB_A0

uint32_t vbat_pin = PIN_VBAT;

#define VBAT_MV_PER_LSB (0.73242188F) // 3.0V ADC range and 12 - bit ADC resolution = 3000mV / 4096
#define VBAT_DIVIDER (0.6F)			  // 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER_COMP (1.45F)	  // Compensation factor for the VBAT divider, depend on the board

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

#define NO_OF_SAMPLES   256          //Multisampling

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

char data[32] = {0};

float readVBAT(void)
{
	int i;
	float raw;
	float mv;

	raw = 0;
	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		raw += analogRead(vbat_pin);
	}
	raw /= NO_OF_SAMPLES;

	mv = esp_adc_cal_raw_to_voltage(raw * REAL_VBAT_MV_PER_LSB);

	return mv * (1/VBAT_DIVIDER);
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


void setup()
{
	u8g2.begin();   //initDisplay

	//Serial.println("RAK4631 Battery test");
	u8g2.clearBuffer();					// clear the internal memory
	u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

	memset(data, 0, sizeof(data));
	sprintf(data, "Battery Test");
	u8g2.drawStr(3, 15, data);

	adcAttachPin(vbat_pin);
	analogSetAttenuation(ADC_11db);
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
	memset(data, 0, sizeof(data));
	sprintf(data, "%.3fV \t %d%%", vbat_mv / 1000, vbat_per);
	u8g2.drawStr(3, 60, data);

	u8g2.sendBuffer(); // transfer internal memory to the display

	delay(1000);
}
