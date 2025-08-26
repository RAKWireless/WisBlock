#include "main.h"

// Includes for BLE Arduino-NimBLE
#include <NimBLEUtils.h>
#include <NimBLEServer.h>
#include <NimBLEDevice.h>
#include <NimBLEAdvertising.h>

// List of Service and Characteristic UUIDs
#define SERVICE_UUID "0000AAAA-EAD2-11E7-80C1-9A214CF093AE"
#define WIFI_UUID "00005555-EAD2-11E7-80C1-9A214CF093AE"

/** SSIDs of local WiFi networks */
String ssidPrim = "EMPTY";
String ssidSec = "EMPTY";
/** Password for local WiFi network */
String pwPrim = "EMPTY";
String pwSec = "EMPTY";

/** Characteristic for digital output */
BLECharacteristic *pCharacteristicWiFi;
/** BLE Advertiser */
BLEAdvertising *pAdvertising;
/** BLE Service */
BLEService *pService;
/** BLE Server */
BLEServer *pServer;

/** Buffer for JSON string */
// Max size is 51 bytes for frame:
// {"ssidPrim":"","pwPrim":"","ssidSec":"","pwSec":""}
// + 4 x 32 bytes for 2 SSID's and 2 passwords
StaticJsonDocument<512> jsonBuffer;

/** Unique device name */
char apName[] = "MHC-Cl1-XXXXXXXXXXXXXXXX";
/** Selected network 
    true = use primary network
		false = use secondary network
*/
bool usePrimAP = true;
/** Flag if stored AP credentials are available */
bool hasCredentials = false;

/**
 * MyServerCallbacks
 * Callbacks for client connection and disconnection
 */
class MyServerCallbacks : public BLEServerCallbacks
{
	// TODO this doesn't take into account several clients being connected
	void onConnect(BLEServer *pServer)
	{
		myLog_d("BLE client connected");
	};

	void onDisconnect(BLEServer *pServer)
	{
		myLog_d("BLE client disconnected");
		pAdvertising->start();
	}
};

/**
 * MyCallbackHandler
 * Callbacks for BLE client read/write requests
 */
class MyCallbackHandler : public BLECharacteristicCallbacks
{
	void onWrite(BLECharacteristic *pCharacteristic)
	{
		std::string value = pCharacteristic->getValue();

		// uint8_t *data = pCharacteristic->getData();

		// for (int idx = 0; idx < value.length(); idx++)
		// {
		// 	Serial.printf("%02X", data[idx]);
		// }
		// Serial.println("");
		for (int idx = 0; idx < value.length(); idx++)
		{
			Serial.printf("%02X", value[idx]);
		}
		Serial.println("");

		if (value.length() == 0)
		{
			return;
		}
		myLog_d("Received over BLE: %s with length %d", (char *)&value[0], value.length());
		myLog_d("Using >>%s<< for decoding", apName);
		// Decode data
		int keyIndex = 0;
		for (int index = 0; index < value.length(); index++)
		{
			value[index] = (char)value[index] ^ (char)apName[keyIndex];
			keyIndex++;
			if (keyIndex >= strlen(apName))
				keyIndex = 0;
		}
		myLog_d("Decoded: >>%s<<", (char *)&value[0]);
		myLog_d("Decoded: >>%s<<", value.c_str());

		/** Json object for incoming data */
		auto error = deserializeJson(jsonBuffer, (char *)&value[0]);
		// JsonObject& jsonIn = jsonBuffer.parseObject((char *)&value[0]);
		if (error)
		{
			myLog_d("Received invalid JSON");
		}
		else
		{

			if (jsonBuffer.containsKey("ssidPrim") &&
				jsonBuffer.containsKey("pwPrim") &&
				jsonBuffer.containsKey("ssidSec") &&
				jsonBuffer.containsKey("pwSec"))
			{
				ssidPrim = jsonBuffer["ssidPrim"].as<String>();
				pwPrim = jsonBuffer["pwPrim"].as<String>();
				ssidSec = jsonBuffer["ssidSec"].as<String>();
				pwSec = jsonBuffer["pwSec"].as<String>();

				Preferences preferences;
				preferences.begin("WiFiCred", false);
				preferences.putString("ssidPrim", ssidPrim);
				preferences.putString("ssidSec", ssidSec);
				preferences.putString("pwPrim", pwPrim);
				preferences.putString("pwSec", pwSec);
				preferences.putBool("valid", true);
				preferences.end();

				myLog_d("\nReceived over bluetooth:\nprimary SSID: %s  password: %s\nsecondary SSID: %s password: %s",
						ssidPrim.c_str(), pwPrim.c_str(), ssidSec.c_str(), pwSec.c_str());
				connStatusChanged = true;
				hasCredentials = true;
			}
			else if (jsonBuffer.containsKey("erase"))
			{
				myLog_d("Received erase command");
				Preferences preferences;
				preferences.begin("WiFiCred", false);
				preferences.clear();
				preferences.end();
				connStatusChanged = true;
				hasCredentials = false;
				ssidPrim = "";
				pwPrim = "";
				ssidSec = "";
				pwSec = "";

				int err;
				err = nvs_flash_init();
				myLog_d("nvs_flash_init: %d", err);
				err = nvs_flash_erase();
				myLog_d("nvs_flash_erase: %d", err);
			}
			else if (jsonBuffer.containsKey("reset"))
			{
				WiFi.disconnect();
				esp_restart();
			}
		}
		jsonBuffer.clear();
	};

	void onRead(BLECharacteristic *pCharacteristic)
	{
		myLog_d("BLE onRead request");
		String wifiCredentials;

		/** Json object for outgoing data */
		// JsonObject &jsonOut = jsonBuffer.createObject();
		jsonBuffer.clear();
		jsonBuffer["ssidPrim"] = ssidPrim;
		jsonBuffer["pwPrim"] = pwPrim;
		jsonBuffer["ssidSec"] = ssidSec;
		jsonBuffer["pwSec"] = pwSec;
		// Convert JSON object into a string
		serializeJson(jsonBuffer, wifiCredentials);
		// jsonBuffer.printTo(wifiCredentials);

		// encode the data
		int keyIndex = 0;
		myLog_d("Stored settings: %s", wifiCredentials.c_str());
		for (int index = 0; index < wifiCredentials.length(); index++)
		{
			wifiCredentials[index] = (char)wifiCredentials[index] ^ (char)apName[keyIndex];
			keyIndex++;
			if (keyIndex >= strlen(apName))
				keyIndex = 0;
		}
		pCharacteristicWiFi->setValue((uint8_t *)&wifiCredentials[0], wifiCredentials.length());
		jsonBuffer.clear();
	}
};

/**
 * initBLE
 * Initialize BLE service and characteristic
 * Start BLE server and service advertising
 */
void initBLE()
{
	// Initialize BLE and set output power
	myLog_d("BLE init with %s\n", apName);
	BLEDevice::init(apName);
	BLEDevice::setPower(ESP_PWR_LVL_P9);

	BLEDevice::setMTU(527);

	myLog_d("BLE advertising using %s\n", apName);

	// Create BLE Server
	pServer = BLEDevice::createServer();

	// Set server callbacks
	pServer->setCallbacks(new MyServerCallbacks());

	// Create BLE Service
	// pService = pServer->createService(BLEUUID(SERVICE_UUID), 20);
	pService = pServer->createService(SERVICE_UUID);

	// Create BLE Characteristic for WiFi settings
	pCharacteristicWiFi = pService->createCharacteristic(
		// BLEUUID(WIFI_UUID),
		WIFI_UUID,
		// BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
		NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);
	pCharacteristicWiFi->setCallbacks(new MyCallbackHandler());

	// Start the service
	pService->start();

	// Start advertising
	pAdvertising = pServer->getAdvertising();
	pAdvertising->addServiceUUID(SERVICE_UUID);
	pAdvertising->start();
}
