#include "main.h"

uint8_t noAPfound = 0;

void setup()
{
	Serial.begin(115200);

	delay(5000);

	Serial.println("NeoPixelClock");

	initWiFi();

	initClock();

	// initRadar();
}

void loop()
{
	uint8_t multiResult = wifiMulti.run();
	if (multiResult != WL_CONNECTED)
	{
		if (multiResult == WL_NO_SSID_AVAIL)
		{
			noAPfound++;
			if (noAPfound >= 50)
			{
				myLog_d("WiFi not connected! Error %d", multiResult);
				myLog_e("No WiFi AP found");
				noAPfound = 0;
				setWiFiError(true);
				wifiMulti.addAP("MyHomeLan-C", "Bernabe@1700");
			}
		}
	}
	else
	{
		setWiFiError(false);
		if (connStatusChanged)
		{
			startOTA();
			initNTP();
			connStatusChanged = false;
		}

		ArduinoOTA.handle();

		if (otaStarted)
		{
			return;
		}
	}

	if (isConnected && !ntpOk)
	{
		tryGetTime();
	}
	
	// Update the time
	time_t now;
	time(&now);					  // get time (as epoch)
	localtime_r(&now, &timeinfo); // update tm struct with current time

	// DateTime nowTime = rtc.now();

	setDay(timeinfo.tm_wday == 0 ? 7 : timeinfo.tm_wday);

	uint8_t nowHour = timeinfo.tm_hour;
	uint8_t nowMinute = timeinfo.tm_min;
	setNumber(nowHour / 10, 0);
	setNumber(nowHour - (nowHour / 10 * 10), 1);
	setNumber(nowMinute / 10, 2);
	setNumber(nowMinute - (nowMinute / 10 * 10), 3);

	/// \todo only for debug
	if (radarTriggered)
	{
		radarTriggered = false;
		if (cdsStatus)
		{
			// digitalWrite(LED_BUILTIN, HIGH);
			myLog_d("Motion detected");
			setBrightness(true);
		}
		else
		{
			// digitalWrite(LED_BUILTIN, LOW);
			myLog_d("Nobody here");
		}
	}
	delay(1000);
}
