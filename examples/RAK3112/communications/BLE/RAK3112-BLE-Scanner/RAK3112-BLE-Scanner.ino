/**
   @file api-test.ino
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief BLE Scanner example
   @version 0.1
   @date 2021-09-10

   @copyright Copyright (c) 2021

*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; // In seconds
BLEScan *pBLEScan;
BLEScanResults foundDevices;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
	void onResult(BLEAdvertisedDevice advertisedDevice)
	{
		Serial.printf("RSSI %d Name: %s\n", advertisedDevice.getRSSI(), advertisedDevice.getName().c_str());
	}
};

void setup()
{
	Serial.begin(115200);
	Serial.println("Scanning...");

	BLEDevice::init("");
	pBLEScan = BLEDevice::getScan(); // create new scan
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
	pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster
	pBLEScan->setInterval(100);
	pBLEScan->setWindow(99); // less or equal setInterval value
}

void loop()
{
	// Start the scan
	pBLEScan->start(scanTime, false);

	Serial.println("Scan done!");
	pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
	delay(2000);
}