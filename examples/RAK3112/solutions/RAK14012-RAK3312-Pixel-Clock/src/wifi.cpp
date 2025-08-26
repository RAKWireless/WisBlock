#include "main.h"

WiFiMulti wifiMulti;

/** Connection change status */
bool connStatusChanged = false;
/** Connection status */
bool isConnected = false;

bool checkPreferences(void);

/**
 * @briefCallback for WiFi events
 */
void wifiEventCb(WiFiEvent_t event)
{
	myLog_d("[WiFi-event] event: %d", event);
	IPAddress localIP;
	switch (event)
	{
	case SYSTEM_EVENT_STA_GOT_IP:
		connStatusChanged = true;
		isConnected = true;

		localIP = WiFi.localIP();
		myLog_d("Connected to AP: %s with IP: %d.%d.%d.%d RSSI: %d",
				WiFi.SSID().c_str(),
				localIP[0], localIP[1], localIP[2], localIP[3],
				WiFi.RSSI());
		setWiFiStatus();
		break;
	case SYSTEM_EVENT_STA_LOST_IP:
	case SYSTEM_EVENT_STA_DISCONNECTED:
	case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
		connStatusChanged = true;
		isConnected = false;
		myLog_e("WiFi lost connection");
		setWiFiStatus();
		break;
	case SYSTEM_EVENT_SCAN_DONE:
		if (!WiFi.isConnected())
		{
			myLog_d("[WiFi-event] Scan finished, no connection");
			wifiMulti.addAP("Your-NetWork-1", "Password");
			wifiMulti.addAP("Your-NetWork-2", "Password");
			wifiMulti.addAP("Your-NetWork-3", "Password");
		}
	default:
		break;
	}
}

/**
 * Initialize WiFi
 * - Check if WiFi credentials are stored in the preferences
 * - Create unique device name
 * - Register WiFi event callback function
 * - Try to connect to WiFi if credentials are available
 */
void initWiFi(void)
{
	createName();

	if (!checkPreferences())
	{
		// initBLE();
	}
	initBLE();

	// WiFi credentials found, lets try to connect

	WiFi.disconnect(true);
	delay(100);
	WiFi.enableSTA(true);
	delay(100);
	WiFi.mode(WIFI_STA);
	delay(100);
	WiFi.onEvent(wifiEventCb);

	// Using WiFiMulti to connect to best AP
	wifiMulti.addAP(ssidPrim.c_str(), pwPrim.c_str());
	wifiMulti.addAP(ssidSec.c_str(), pwSec.c_str());
	// wifiMulti.addAP("THIRD_AP_NAME", "THIRD_AP_PASSWORD");

	setWiFiStatus();
	setWiFiError(true);
}

bool checkPreferences(void)
{
	Preferences preferences;
	bool hasCredentials = false;
	preferences.begin("WiFiCred", false);
	bool hasPref = preferences.getBool("valid", false);
	if (hasPref)
	{
		ssidPrim = preferences.getString("ssidPrim", "");
		ssidSec = preferences.getString("ssidSec", "");
		pwPrim = preferences.getString("pwPrim", "");
		pwSec = preferences.getString("pwSec", "");

		if (ssidPrim.equals("") || pwPrim.equals("") || ssidSec.equals("") || pwPrim.equals(""))
		{
			myLog_d("Found preferences but credentials are invalid");
		}
		else
		{
			myLog_d("\nRead from preferences:\nprimary SSID: %s  password: %s\nsecondary SSID: %s password: %s",
					ssidPrim.c_str(), pwPrim.c_str(), ssidSec.c_str(), pwSec.c_str());
			hasCredentials = true;
		}
	}
	else
	{
		myLog_d("Could not find preferences, need send data over BLE");
	}
	preferences.end();
	return hasCredentials;
}

/**
 * Create unique device name from MAC address
 **/
void createName()
{
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	// Write unique name into apName
	sprintf(apName, "MHC-Cl1-%02X%02X%02X%02X%02X%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
}
