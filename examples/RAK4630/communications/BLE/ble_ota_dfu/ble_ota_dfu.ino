/**
   @file ble_ota_dfu.ino
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief BLE example shows how to upgrade firmware over BLE (OTA DFU)
   @version 0.1
   @date 2020-07-27

   @copyright Copyright (c) 2020

   @note RAK4631 GPIO mapping to nRF52840 GPIO ports
   RAK4631    <->  nRF52840
   WB_IO1     <->  P0.17 (GPIO 17)
   WB_IO2     <->  P1.02 (GPIO 34)
   WB_IO3     <->  P0.21 (GPIO 21)
   WB_IO4     <->  P0.04 (GPIO 4)
   WB_IO5     <->  P0.09 (GPIO 9)
   WB_IO6     <->  P0.10 (GPIO 10)
   WB_SW1     <->  P0.01 (GPIO 1)
   WB_A0      <->  P0.04/AIN2 (AnalogIn A2)
   WB_A1      <->  P0.31/AIN7 (AnalogIn A7)
 */
#include <Arduino.h>
#include <bluefruit.h>

// Forward declarations for functions
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);

/**
   @brief  BLE DFU service
   @note   Used for DFU OTA upgrade
*/
BLEDfu bledfu;

/**
   @brief Arduino setup function. Called once after power on or reset

*/
void setup()
{
    // Initialize built in green LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Initialize Serial for debug output
    Serial.begin(115200);

    // Wait for USB Serial to be ready or terminal to be connected
    time_t timeout = millis(); // Timeout in case the system runs on its own
    // Waiting for Serial
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            // Blink the LED to show that we are alive
            delay(100);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        else
        {
            // Timeout while waiting for USB Serial
            break;
        }
    }

    Serial.println("================================");
    Serial.println("RAK4631 BLE OTA example");
    Serial.println("================================");

    // Config the peripheral connection with maximum bandwidth
    // more SRAM required by SoftDevice
    // Note: All config***() function must be called before begin()
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
    Bluefruit.configPrphConn(92, BLE_GAP_EVENT_LENGTH_MIN, 16, 16);

    Bluefruit.begin(1, 0);
    // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
    Bluefruit.setTxPower(4);
    // Set the BLE device name
    Bluefruit.setName("RAK4631_OTA");

    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

    // To be consistent OTA DFU should be added first if it exists
    bledfu.begin();

    // Set up and start advertising
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addName();

    /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
    */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

/**
   @brief  Callback when client connects
   @param  conn_handle: Connection handle id
*/
void connect_callback(uint16_t conn_handle)
{
    (void)conn_handle;
    Serial.println("BLE client connected");
}

/**
   @brief  Callback invoked when a connection is dropped
   @param  conn_handle: connection handle id
   @param  reason: disconnect reason
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.println("BLE client disconnected");
}

/**
   @brief Arduino loop. Runs forever until power off or reset

*/
void loop()
{
}
