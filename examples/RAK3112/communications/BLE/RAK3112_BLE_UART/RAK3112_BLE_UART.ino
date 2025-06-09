/**
 * @file RAK3112_BLE_UART.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief BLE_UART example
 *	Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
 *	Ported to Arduino ESP32 by Evandro Copercini
 *
 *   Create a BLE server that, once we receive a connection, will send periodic notifications.
 *   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
 *   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
 *   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"
 *
 *   The design of creating the BLE server is:
 *   1. Create a BLE Server
 *   2. Create a BLE Service
 *   3. Create a BLE Characteristic on the Service
 *   4. Create a BLE Descriptor on the characteristic
 *   5. Start the service.
 *   6. Start advertising.
 *
 *   In this example rxValue is the data received (only accessible inside that function).
 *   And txValue is the data to be sent, in this example whatever was received over the USB Serial.
 * @version 0.1
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0x30;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"			  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // UART RX characteristic UUID
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // UART TX characteristic UUID

/**
 * @brief BLE connection/disconnection callback
 *
 */
class MyServerCallbacks : public BLEServerCallbacks
{
	void onConnect(BLEServer *pServer)
	{
		deviceConnected = true;
	};

	void onDisconnect(BLEServer *pServer)
	{
		deviceConnected = false;
	}
};

/**
 * @brief BLE UART write callback (data received)
 *
 */
class MyCallbacks : public BLECharacteristicCallbacks
{
	void onWrite(BLECharacteristic *pCharacteristic)
	{
		String rxValue = pCharacteristic->getValue();

		if (rxValue.length() > 0)
		{
			Serial.println("*********");
			Serial.print("Received Value: ");
			for (int i = 0; i < rxValue.length(); i++)
			{
				Serial.print(rxValue[i]);
			}

			Serial.println();
			Serial.println("*********");
		}
	}
};

/**
 * @brief Arduino setup function, called once
 *
 */
void setup(void)
{
	// Prepare LED's BLUE ==> TX, GREEN ==> Received a packet
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);

	// Initialize Serial for debug output
	Serial.begin(115200);
	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	// Create the BLE Device
	BLEDevice::init("UART Service");

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);

	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);

	pRxCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
}

/**
 * @brief Arduino loop, runs forever
 *
 */
void loop(void)
{
	if (deviceConnected)
	{
		if (Serial.available())
		{
			while (Serial.available())
			{
				txValue = Serial.read();
				pTxCharacteristic->setValue(&txValue, 1);
				pTxCharacteristic->notify();
				delay(10); // bluetooth stack will go into congestion, if too many packets are sent
			}
		}
	}

	// disconnecting
	if (!deviceConnected && oldDeviceConnected)
	{
		delay(500);					 // give the bluetooth stack the chance to get things ready
		pServer->startAdvertising(); // restart advertising
		Serial.println("start advertising");
		oldDeviceConnected = deviceConnected;
	}
	// connecting
	if (deviceConnected && !oldDeviceConnected)
	{
		// do stuff here on connecting
		oldDeviceConnected = deviceConnected;
	}
}