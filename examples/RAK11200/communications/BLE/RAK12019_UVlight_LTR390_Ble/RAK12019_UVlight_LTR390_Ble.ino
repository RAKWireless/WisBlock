/**
   @file ble_environment_node.ino
   @author rakwireless.com
   @brief This sketch demonstrate how to get environment data from LTR390
      and send the data to ble gateway with ble gatt.
      ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
   @version 0.1
   @date 2021-09-13
   @copyright Copyright (c) 2021
**/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "UVlight_LTR390.h" //Click here to get the library:

//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.

UVlight_LTR390 ltr = UVlight_LTR390();
void LTR390_Init(void);

static float ALS = 0;
static float LUX = 0;
static float UVS = 0;
static float UVI = 0;
static uint8_t lightdata[8] = {0};
int beatAvg;
int detect_flag = 0;
int len = 0;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

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

class MyCallbacks : public BLECharacteristicCallbacks
{
	void onWrite(BLECharacteristic *pCharacteristic)
	{
		std::string rxValue = pCharacteristic->getValue();

		if (rxValue.length() > 0)
		{
			Serial.println("*********");
			Serial.print("Received Value: ");
			for (int i = 0; i < rxValue.length(); i++)
				Serial.print(rxValue[i]);

			Serial.println();
			Serial.println("*********");
		}
	}
};

/*
  @brief LTP390 initialize
*/
void LTR390_Init(void)
{
	//Sensor power switch
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(300);

	Serial.println("RAK12019 Ble Test");
	Wire.begin();
	if (!ltr.init())
	{
		Serial.println("Couldn't find LTR sensor!");
		while (1)
			delay(10);
	}
	Serial.println("Found LTR390 sensor!");

	//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
	ltr.setMode(LTR390_MODE_ALS); //LTR390_MODE_UVS
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		Serial.println("In ALS mode");
	}
	else
	{
		Serial.println("In UVS mode");
	}

	ltr.setGain(LTR390_GAIN_3);
	Serial.print("Gain : ");
	switch (ltr.getGain())
	{
	case LTR390_GAIN_1:
		Serial.println(1);
		break;
	case LTR390_GAIN_3:
		Serial.println(3);
		break;
	case LTR390_GAIN_6:
		Serial.println(6);
		break;
	case LTR390_GAIN_9:
		Serial.println(9);
		break;
	case LTR390_GAIN_18:
		Serial.println(18);
		break;
	default:
		Serial.println("Failed to set gain");
		break;
	}
	ltr.setResolution(LTR390_RESOLUTION_16BIT);
	Serial.print("Integration Time (ms): ");
	switch (ltr.getResolution())
	{
	case LTR390_RESOLUTION_13BIT:
		Serial.println(13);
		break;
	case LTR390_RESOLUTION_16BIT:
		Serial.println(16);
		break;
	case LTR390_RESOLUTION_17BIT:
		Serial.println(17);
		break;
	case LTR390_RESOLUTION_18BIT:
		Serial.println(18);
		break;
	case LTR390_RESOLUTION_19BIT:
		Serial.println(19);
		break;
	case LTR390_RESOLUTION_20BIT:
		Serial.println(20);
		break;
	default:
		Serial.println("Failed to set Integration Time");
		break;
	}

	ltr.setThresholds(100, 1000); //Set the interrupt output threshold range for lower and upper.
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		ltr.configInterrupt(true, LTR390_MODE_ALS); //Configure the interrupt based on the thresholds in setThresholds()
	}
	else
	{
		ltr.configInterrupt(true, LTR390_MODE_UVS);
	}
}

/**@brief Function for the Sensor get data.
*/
String data = "";
void UVlight_Ltr390_GetData(void)
{
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		uint32_t sendlux = 0;
		uint32_t sendals = 0;
		uint8_t count = 0;
		ALS = ltr.readALS();
		LUX = ltr.getLUX(); //calculate the lux
		sendlux = LUX * 100;
		sendals = ALS;
		data = "Lux = " + String(LUX) + " " + "sendals=" + String(sendals) + " ";
		Serial.println(data);
		data = "";
		lightdata[count++] = (uint8_t)((sendlux & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((sendlux & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((sendlux & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(sendlux & 0x000000FF);
		lightdata[count++] = (uint8_t)((sendals & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((sendals & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((sendals & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(sendals & 0x000000FF);
		ALS = 0;
		LUX = 0;
	}
	else
	{
		uint32_t senduvs = 0;
		uint32_t senduvi = 0;
		uint8_t count = 0;
		UVS = ltr.readUVS();
		UVI = ltr.getUVI(); //calculate the UVI
		senduvi = UVI * 100;
		senduvs = UVS;
		data = "UVI = " + String(UVI) + " " + " UVS=" + String(senduvs) + " ";
		Serial.println(data);
		data = "";
		lightdata[count++] = (uint8_t)((senduvi & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((senduvi & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((senduvi & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(senduvi & 0x000000FF);
		lightdata[count++] = (uint8_t)((senduvs & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((senduvs & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((senduvs & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(senduvs & 0x000000FF);
		UVS = 0;
		LUX = 0;
	}
}

void setup()
{
	Serial.begin(115200);
	LTR390_Init();
	Serial.println("RAK11200 Ble Test");
	// Create the BLE Device
	BLEDevice::init("RAK11200 UVLIGHT");

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_TX,
		BLECharacteristic::PROPERTY_NOTIFY);

	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
		CHARACTERISTIC_UUID_RX,
		BLECharacteristic::PROPERTY_WRITE);

	pRxCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();
	Serial.println("Waiting a client connection to notify...");
}

void loop()
{

	if (deviceConnected)
	{
		UVlight_Ltr390_GetData();
		pTxCharacteristic->setValue(lightdata, sizeof(lightdata) / sizeof(uint8_t));
		pTxCharacteristic->notify();
		delay(1000); // bluetooth stack will go into congestion, if too many packets are sent
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
