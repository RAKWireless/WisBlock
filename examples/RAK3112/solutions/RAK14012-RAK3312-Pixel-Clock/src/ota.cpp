#include "main.h"

/** Flag if OTA is enabled */
boolean otaStarted = false;

/** Limit the progress output on serial */
unsigned int lastProgress = 0;

/**
 * Initialize OTA server
 * and start waiting for OTA requests
 */
void startOTA(void)
{
	ArduinoOTA
		// OTA request received
		.onStart([]() {
			stopClock();
			stopRadar();

			String type;
			if (ArduinoOTA.getCommand() == U_FLASH)
				type = "sketch";
			else // U_SPIFFS
				type = "filesystem";

			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			myLog_d("Start updating %s", type);
			lastProgress = 0;
			otaStarted = true;
		})
		.onEnd([]() {
			// OTA is finished
			myLog_d("\nEnd");
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			// Status report during OTA
			if ((lastProgress == 0) || ((progress / (total / 100)) >= lastProgress + 5))
			{
				myLog_d("Progress: %u%%\r", (progress / (total / 100)));
				lastProgress = (progress / (total / 100));
				if (lastProgress == 0)
				{
					lastProgress = 1;
				}
			}
			showOTA(progress / (total / 100));
		})
		.onError([](ota_error_t error) {
			// Error occured during OTA, report it
			myLog_d("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
				myLog_d("Auth Failed");
			else if (error == OTA_BEGIN_ERROR)
				myLog_d("Begin Failed");
			else if (error == OTA_CONNECT_ERROR)
				myLog_d("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR)
				myLog_d("Receive Failed");
			else if (error == OTA_END_ERROR)
				myLog_d("End Failed");
		});

	// Enable MDNS so device can be seen
	ArduinoOTA.setMdnsEnabled(true);

	// Create a unique name
	uint8_t uniqueID[6];
	char hostApName[17];
	WiFi.macAddress(uniqueID);
	sprintf(hostApName, "MHC-Cl1-%02X%02X%02X%02X", uniqueID[0], uniqueID[3], uniqueID[4], uniqueID[5]);

	// Set the MDNS advertising name
	ArduinoOTA.setHostname(hostApName);
	// Start the OTA server
	ArduinoOTA.begin();

	MDNS.addServiceTxt("arduino", "tcp", "board", "ESP8266");
	MDNS.addServiceTxt("arduino", "tcp", "type", "Lanai Clock");
	MDNS.addServiceTxt("arduino", "tcp", "id", String(hostApName));
	MDNS.addServiceTxt("arduino", "tcp", "service", "MHC");
	MDNS.addServiceTxt("arduino", "tcp", "loc", "Lanai");
}
