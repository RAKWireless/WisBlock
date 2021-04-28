/**
   @file ble.ino
   @author rakwireless.com
   @brief function of BLE
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <NimBLEDevice.h> //http://librarymanager/All#NimBLE

extern uint32_t wifi_config_get(char* wifi_config, uint32_t len);
extern void wifi_config_set(char *jsonStr, uint32_t len);

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

/** Device name for BLE */
char g_devName[32] = {0};

/**
   ServerCallbacks
   Callbacks for client connection and disconnection
*/
class ServerCallbacks : public NimBLEServerCallbacks
{
    // TODO this doesn't take into account several clients being connected
    void onConnect(NimBLEServer *pServer)
    {
      Serial.println("BLE client connected");

      // blue light on
      digitalWrite(WB_LED2, HIGH);
    };

    void onDisconnect(NimBLEServer *pServer)
    {
      // blue light off
      digitalWrite(WB_LED2, LOW);
      Serial.println("BLE client disconnected");
      pAdvertising->start();
    }
};

/**
   CharacteristicCallbacks
   Callbacks for BLE client read/write requests
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

      // set wifi ssid and psk
      wifi_config_set((char *)&value[0], value.length());
    };

    void onRead(NimBLECharacteristic *pCharacteristic)
    {
      Serial.println("BLE onRead request");

      char wifi_config[256] = {0};
      // get wifi ssid and psk
      uint32_t len = wifi_config_get(wifi_config, sizeof(wifi_config));

      pCharacteristic->setValue((uint8_t *)wifi_config, (size_t) len);
    }
};

static CharacteristicCallbacks chrCallbacks;

/**
   @brief update BLE characteristic
*/
void update_characteristic(void)
{
  char wifi_config[256] = {0};
  // get wifi ssid and psk
  uint32_t len = wifi_config_get(wifi_config, sizeof(wifi_config));

  pCharacteristic->setValue((uint8_t *)wifi_config, (size_t) len);
}

/**
   @brief  ble server set character and start advertise
*/
void ble_service_start()
{
  uint8_t devAddrArray[6] = {0};

  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("");
  // get ble mac
  memcpy(devAddrArray, BLEDevice::getAddress().getNative(), 6);
  snprintf(g_devName, 32, "RAK11200-%02X%02X%02X", devAddrArray[2], devAddrArray[1], devAddrArray[0]);

  BLEDevice::deinit();

  // set ble device name contain mac, like RAK11200-AABBCC
  BLEDevice::init(g_devName);

  Serial.printf("Len: %d BLE name: %s\n", strlen(g_devName), g_devName);
  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();

  // Set server callbacks
  pServer->setCallbacks(new ServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristic for WiFi settings
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         NIMBLE_PROPERTY::READ |
                                         NIMBLE_PROPERTY::WRITE
                                       );

  pCharacteristic->setCallbacks(&chrCallbacks);

  char wifi_config[256] = {0};
  // get wifi ssid and psk
  uint32_t len = wifi_config_get(wifi_config, sizeof(wifi_config));

  pCharacteristic->setValue((uint8_t *)wifi_config, (size_t) len);

  pService->start();

  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("Characteristic defined! Now you can read and write it in your phone!");
}
