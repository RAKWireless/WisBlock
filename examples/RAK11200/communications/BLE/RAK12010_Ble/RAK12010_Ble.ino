/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E
   Has a characteristic of: 6E400002-B5A3-F393-E0A9-E50E24DCCA9E - used for receiving data with "WRITE"
   Has a characteristic of: 6E400003-B5A3-F393-E0A9-E50E24DCCA9E - used to send data with  "NOTIFY"

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   In this example rxValue is the data received (only accessible inside that function).
   And txValue is the data to be sent, in this example just a byte incremented every second.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Light_VEML7700.h"

//veml770 Initialize
Light_VEML7700 VMEL = Light_VEML7700();
void Sensor_VEML7700_Print(void);
void Sensor_VEML7700_Init(void);
void Sensor_VEML7700_GetData(void);

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

int beatAvg;
int detect_flag = 0;
uint8_t tmp[10];
int len = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

/**@brief Function for sensor gain and IntegrationTime print
   time:2021/07/15
*/
void Sensor_VEML7700_Init(void)
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  if (!VMEL.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("Sensor found");
  VMEL.setGain(VEML7700_GAIN_1_8);
  VMEL.setIntegrationTime(VEML7700_IT_25MS);
  Sensor_VEML7700_Print();
  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  VMEL.setLowThreshold(10);
  VMEL.setHighThreshold(30000);
  VMEL.interruptEnable(true);
}

/**@brief Function for sensor gain and IntegrationTime print
   time:2021/07/15
*/
void Sensor_VEML7700_Print(void)
{
  Serial.print(F("Gain: "));
  switch (VMEL.getGain())
  {
    case VEML7700_GAIN_1: Serial.print("1  "); break;
    case VEML7700_GAIN_2: Serial.print("2  "); break;
    case VEML7700_GAIN_1_4: Serial.print("1/4  "); break;
    case VEML7700_GAIN_1_8: Serial.print("1/8  "); break;
  }
  Serial.print(F("Integration Time (ms): "));
  switch (VMEL.getIntegrationTime())
  {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }
}

/**@brief Function for get the sensor data
   time:2021/07/15
*/
String data = "";
void Sensor_VEML7700_GetData(void)
{
  uint32_t Lux = 0;
  uint32_t White = 0;
  uint32_t Als = 0;
  uint8_t count = 0;
  Lux = VMEL.readLux()  * 100;
  White = VMEL.readWhite() * 100;
  Als = VMEL.readALS();
  data = "Lux = " + String(Lux) + " " + " White = " + String(White) + " " + " Als=" + String(Als) + " ";
  Serial.println(data);
  data = "";
  tmp[count++] = (uint8_t)((Lux & 0xFF000000) >> 24);
  tmp[count++] = (uint8_t)((Lux & 0x00FF0000) >> 16);
  tmp[count++] = (uint8_t)((Lux & 0x0000FF00) >> 8);
  tmp[count++] = (uint8_t)(Lux &  0x000000FF);
  tmp[count++] = (uint8_t)((White & 0xFF000000) >> 24);
  tmp[count++] = (uint8_t)((White & 0x00FF0000) >> 16);
  tmp[count++] = (uint8_t)((White & 0x0000FF00) >> 8);
  tmp[count++] = (uint8_t)(White &  0x000000FF);
  tmp[count++] = (uint8_t)((Als & 0x0000FF00) >> 8);
  tmp[count++] = (uint8_t)(Als & 0xFF);
  
  uint16_t irq = VMEL.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold");
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold");
  }
}

void setup() {
  Serial.begin(115200);
  Sensor_VEML7700_Init();
  // Create the BLE Device
  BLEDevice::init("RAK11200 HRM");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  if (deviceConnected)
  {
    Sensor_VEML7700_GetData();
    pTxCharacteristic->setValue(tmp, sizeof(tmp) / sizeof(uint8_t));
    pTxCharacteristic->notify();
    delay(1000); // bluetooth stack will go into congestion, if too many packets are sent
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
