#include "main.h"
#include "chars.h"

NeoPixelBus<NeoGrbFeature, NeoEsp32Rmt1Ws2812xMethod> strip(256, WB_IO5);

uint8_t colorSaturation = 16;
uint8_t lightSaturation = 3;

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);
RgbColor lightblue(0, 0, lightSaturation);
RgbColor lightwhite(lightSaturation);
RgbColor lightred(lightSaturation, 0, 0);

int8_t evenCorr[4] = {4, 12, 22, 30};
int8_t oddCorr[4] = {28, 20, 10, 2};
uint16_t pixelStart[4] = {128, 132, 137, 141};

Ticker colonTicker;

Ticker motionTicker;

// SemaphoreHandle_t accessStrip;

boolean colonVisible = false;

boolean motionVisible = false;

void initClock(void)
{
	// accessStrip = xSemaphoreCreateMutex();

	strip.Begin();
	strip.Show();
	colonTicker.attach(0.5, setColon);
	motionTicker.attach(0.25, showMotion);
}

void stopClock()
{
	colonTicker.detach();
	motionTicker.detach();
	clearStrip();
}

void updateClock(void)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		strip.Show();
		// xSemaphoreGive(accessStrip);
	}
}

void setNumber(uint8_t value, uint8_t digit)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		uint8_t *arrPtr;
		if ((digit == 0) && (value == 0))
		{
			arrPtr = blank;
		}
		else
		{
			arrPtr = number[value];
		}

		uint8_t pixel = pixelStart[digit];
		int16_t evenCorrVal = evenCorr[digit];
		int16_t oddCorrVal = oddCorr[digit];

		for (uint8_t row = 0; row < 9; row++)
		{
			for (uint8_t col = 0; col < 3; col++)
			{
				if (arrPtr[(row * 3) + col] == 1)
				{
					strip.SetPixelColor(pixel, green);
				}
				else
				{
					strip.SetPixelColor(pixel, black);
				}
				if (row % 2 == 0)
				{
					pixel++;
				}
				else
				{
					pixel--;
				}
			}
			if (row % 2 == 0)
			{
				pixel -= evenCorrVal;
			}
			else
			{
				pixel -= oddCorrVal;
			}
		}
		// xSemaphoreGive(accessStrip);
	}
}

void setDay(uint8_t value)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		uint8_t *day = daysOfWeek[value];
		for (int pixel = 160; pixel < 256; pixel++)
		{
			if (day[pixel - 160] == 1)
			{
				strip.SetPixelColor(pixel, blue);
			}
			else
			{
				strip.SetPixelColor(pixel, black);
			}
		}
		// xSemaphoreGive(accessStrip);
		setWiFiStatus();
	}
}

void setColon(void)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		RgbColor colonColor = black;
		if (colonVisible)
		{
			colonColor = red;
		}
		colonVisible = !colonVisible;

		strip.SetPixelColor(39, colonColor);
		strip.SetPixelColor(40, colonColor);
		strip.SetPixelColor(55, colonColor);
		strip.SetPixelColor(56, colonColor);
		strip.SetPixelColor(87, colonColor);
		strip.SetPixelColor(88, colonColor);
		strip.SetPixelColor(103, colonColor);
		strip.SetPixelColor(104, colonColor);
		strip.Show();
		// xSemaphoreGive(accessStrip);
	}
}

void setBrightness(bool lightsOn)
{
	if (lightsOn)
	{
		red = RgbColor(colorSaturation, 0, 0);
		green = RgbColor(0, colorSaturation, 0);
		blue = RgbColor(0, 0, colorSaturation);
		lightblue = RgbColor(0, 0, lightSaturation);
		lightwhite = RgbColor(lightSaturation);
		lightred = RgbColor(lightSaturation, 0, 0);
	}
	else
	{
		red = RgbColor(0);
		green = RgbColor(0);
		blue = RgbColor(0);
		lightblue = RgbColor(0);
		lightwhite = RgbColor(0);
		lightred = RgbColor(0);
	}
}

void setWiFiStatus(void)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		if (isConnected)
		{
			strip.SetPixelColor(159, lightwhite);
		}
		else
		{
			strip.SetPixelColor(159, lightred);
		}
		// xSemaphoreGive(accessStrip);
	}
}

void setWiFiError(bool error)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		if (error)
		{
			strip.SetPixelColor(157, lightred);
		}
		else
		{
			strip.SetPixelColor(157, black);
		}
		// xSemaphoreGive(accessStrip);
	}
}

void showMotion(void)
{
	// if ((accessStrip != NULL) && (xSemaphoreTake(accessStrip, (TickType_t)10) == pdTRUE))
	{
		RgbColor motionColor = black;
		if (cdsStatus)
		{
			if (motionVisible)
			{
				motionColor = lightblue;
			}
			motionVisible = !motionVisible;
		}
		strip.SetPixelColor(71, motionColor);
		strip.SetPixelColor(72, motionColor);

		if (ntpOk)
		{
			strip.SetPixelColor(155, lightwhite);
		}
		else
		{
			strip.SetPixelColor(155, lightred);
		}

		// xSemaphoreGive(accessStrip);
		updateClock();
	}
}

void clearStrip(void)
{
	strip.ClearTo(black);
	strip.Show();
}

void showOTA(uint16_t pixel)
{
	strip.SetPixelColor(pixel, green);
	strip.Show();
}