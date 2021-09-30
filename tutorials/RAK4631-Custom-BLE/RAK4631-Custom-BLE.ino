/**
   @file RAK4631-Custom-BLE.ino
   @author rakwireless.com
   @brief RAK4631 Custom BLE peripheral example using Adafruit Bluefruit
   @version 0.1
   @date 2021-9-29
   @copyright Copyright (c) 2021
**/
#include <bluefruit.h>

// Custom UUID used to differentiate this device.
// Use any online UUID generator to generate a valid UUID.
// Note that the byte order is reversed ...
// E54B0001-67F5-479E-8711-B3B99198CE6C - led service
// E54B0002-67F5-479E-8711-B3B99198CE6C - led characteristic

const uint8_t LED_SERVICE_UUID[] =
{
    0x6C, 0xCE, 0x98, 0x91, 0xB9, 0xB3, 0x11, 0x87,
    0x9E, 0x47, 0xF5, 0x67, 0x01, 0x00, 0x4B, 0xE5
};

const uint8_t LED_CHR_UUID[] =
{
    0x6C, 0xCE, 0x98, 0x91, 0xB9, 0xB3, 0x11, 0x87,
    0x9E, 0x47, 0xF5, 0x67, 0x02, 0x00, 0x4B, 0xE5
};


// RAK4630 supply two LED
// Green LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

// Blue LED
#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif

BLEService        lbs(LED_SERVICE_UUID);
BLECharacteristic lsbLED(LED_CHR_UUID);
BLEConnection* connection;

void setupAdv(void)
{

  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
 // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(lbs);

  // There isn't enough room in the advertising packet for the
  // name so we'll place it on the secondary Scan Response packet
  Bluefruit.ScanResponse.addName();
  // For recommended advertising interval
  // https://developer.apple.com/library/content/qa/qa1931/_index.html  
  Bluefruit.Advertising.setInterval(320,360);    // in unit of 0.625 ms
  Bluefruit.Advertising.restartOnDisconnect(true);
}
void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
  Serial.println("BLE custom example"); 
  Bluefruit.begin();
  Bluefruit.setName("RAK4630");
  
  // Configure callbacks
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0); // led off

  lbs.begin();
  lsbLED.setProperties(CHR_PROPS_READ | CHR_PROPS_WRITE);
  lsbLED.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  lsbLED.setFixedLen(1);
  lsbLED.begin();
  lsbLED.write8(0x00); // led = off
  lsbLED.setWriteCallback(led_write_callback);

  // Set up Advertising Packet
  setupAdv();

  // Start Advertising
  Bluefruit.Advertising.start();
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); 
  Serial.println(reason, HEX);
  lsbLED.write8(0x0);

}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}


void led_write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  (void) conn_hdl;
  (void) chr;
  (void) len; // len should be 1

  // If least significant bit of data[0] is 0 then turn LED Off
  // If least significant bit of data[0] is 1 then turn LED On

  digitalWrite(LED_BUILTIN,data[0]&1);
  Serial.print("Data: ");
  Serial.println(data[0],HEX);
  lsbLED.write8(data[0]&1);
}

void loop() 
{
  
}
