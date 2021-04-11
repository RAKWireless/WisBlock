/**
   @file ble_server.ino
   @author rakwireless.com
   @brief ble server
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <NimBLEDevice.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
#define SERVICE_UUID        "0000AAAA-EAD2-11E7-80C1-9A214CF093AE"
#define CHARACTERISTIC_UUID "00005555-EAD2-11E7-80C1-9A214CF093AE"

/** Characteristic */
NimBLECharacteristic *pCharacteristic;
/** BLE Advertiser */
BLEAdvertising *pAdvertising;
/** BLE Service */
NimBLEService *pService;
/** BLE Server */
NimBLEServer *pServer;

/**
 * ServerCallbacks
 * Callbacks for client connection and disconnection
 */
class ServerCallbacks : public NimBLEServerCallbacks
{
	// TODO this doesn't take into account several clients being connected
	void onConnect(NimBLEServer *pServer)
	{
		Serial.println("BLE client connected");
	};

	void onDisconnect(NimBLEServer *pServer)
	{
		Serial.println("BLE client disconnected");
		pAdvertising->start();
	}
};

/**
 * CharacteristicCallbacks
 * Callbacks for BLE client read/write requests
 */
class CharacteristicCallbacks : public NimBLECharacteristicCallbacks
{
	void onWrite(NimBLECharacteristic *pCharacteristic)
	{
		std::string value = pCharacteristic->getValue();

		if (value.length() == 0)
		{
			return;
		}
		Serial.printf("Received over BLE: %s with length %d\r\n", (char *)&value[0], value.length());
	};

	void onRead(NimBLECharacteristic *pCharacteristic)
	{
		Serial.println("BLE onRead request");
		uint8_t str[] = "Hello, I can see you";
		
		pCharacteristic->setValue(str);
	}
};

static CharacteristicCallbacks chrCallbacks;

/**
   @brief  ble server set character and start advertise
*/
void ble_service_start()
{
  char devName[32] = {0};
  uint8_t devAddrArray[6] = {0};
  
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("");
  // get ble mac
  memcpy(devAddrArray, BLEDevice::getAddress().getNative(), 6);
  snprintf(devName, 32, "RAK12000-%02X%02X%02X\n", devAddrArray[2], devAddrArray[1], devAddrArray[0]);

  BLEDevice::deinit();

  // set ble device name contain mac, like RAK12000-AABBCC
  BLEDevice::init(devName);

  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  
  // Set server callbacks
  pServer->setCallbacks(new ServerCallbacks());
  
  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);
  
  // Create BLE Characteristic
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID,
                                        NIMBLE_PROPERTY::READ |
                                        NIMBLE_PROPERTY::WRITE 
                                       );

  pCharacteristic->setCallbacks(&chrCallbacks);

  pService->start();

  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Characteristic defined! Now you can read and write it in your phone!");
}

void setup()
{
	ble_service_start();
}

void loop()
{
  // put your main code here, to run repeatedly:
  delay(20000);
}
