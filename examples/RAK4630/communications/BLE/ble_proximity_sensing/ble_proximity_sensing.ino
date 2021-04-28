/**
 * @file ble_proximity_sensing.ino
 * @author rakwireless.com
 * @brief This sketch demonstrate how to run both Central and Peripheral roles
 *  at the same time.
 *  node1 <--> DualRole <--> node2
 *  When node 1 and node 2 are close, the green light on the board is on.
 *  Otherwise, the green light is off.
 * @version 0.1
 * @date 2020-08-21
 *
 * @copyright Copyright (c) 2020
 *
 * @note RAK4631 GPIO mapping to nRF52840 GPIO ports
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

#include <bluefruit.h>

#define MAX_COUNT_NUM (5)
#define AVERAGE_RSSI_VAL (-40)
int32_t g_count = 0;
int32_t g_rssi = 0;
void ble_start_advertisement(void);
void ble_scan_callback(ble_gap_evt_adv_report_t *report);

void setup()
{
    time_t timeout = millis();
    Serial.begin(115200);
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            delay(100);
        }
        else
        {
            break;
        }
    }

    pinMode(PIN_LED1, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    uint8_t mac[6] = {0};
    char deviceName[16] = {0};

    Serial.println("RAK4630 Dual Role BLEUART Example");
    Serial.println("-------------------------------------\n");

    // Initialize Bluefruit with max concurrent connections as Peripheral = 1, Central = 1
    // SRAM usage required by SoftDevice will increase with number of connections
    Bluefruit.begin(1, 1);
    Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
    Bluefruit.getAddr(mac);
    sprintf(deviceName, "RAK4630-%2X%2X%2X", mac[0], mac[1], mac[2]);
    Bluefruit.setName(deviceName);

    /* Start Central Scanning
    * - Enable auto scan if disconnected
    * - Interval = 100 ms, window = 80 ms
    * - Filter only accept bleuart service
    * - Don't use active scan
    * - Start(timeout) with timeout = 0 will scan forever (until connected)
    */
    Bluefruit.Scanner.setRxCallback(ble_scan_callback);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.filterRssi(-80);      // Only invoke callback for devices with RSSI >= -80 dBm
    Bluefruit.Scanner.setInterval(160, 80); // in units of 0.625 ms
    Bluefruit.Scanner.useActiveScan(true);  // Request scan response data
    Bluefruit.Scanner.start(0);             // 0 = Don't stop scanning after n seconds
    Serial.println("Scanning ...");

    // Set up and start advertising
    ble_start_advertisement();
}

/**
   @brief start BLE advertisement
*/
void ble_start_advertisement(void)
{
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addName();

    /* Start Advertising
    * - Enable auto advertising if disconnected
    * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
    * - Timeout for fast mode is 30 seconds
    * - Start(timeout) with timeout = 0 will advertise forever (until connected)
    *
    * For recommended advertising interval
    * https://developer.apple.com/library/content/qa/qa1931/_index.html
    */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void loop()
{
    // do nothing, all the work is done in callback
}

/**
 * @brief  when the Bluetooth node scans that there is a Bluetooth broadcast with the prefix of ‘rak4630-’ around
 * and the RSSI of the Bluetooth broadcast is less than the set threshold value,
 * the light is on. On the contrary, the light is off.
 */
void ble_scan_callback(ble_gap_evt_adv_report_t *report)
{
    uint8_t len = 0;
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));

    /* Check for UUID128 Complete List */
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer));
    if (len == 14 && strncmp((char *)buffer, "RAK4630-", 8) == 0)
    {
        g_count++;
        g_rssi += report->rssi;

        if (g_count == MAX_COUNT_NUM)
        {
            Serial.printf("===rssi: %d===\n", g_rssi / MAX_COUNT_NUM);
            if ((g_rssi / MAX_COUNT_NUM) >= AVERAGE_RSSI_VAL)
            {
                digitalWrite(LED_BUILTIN, HIGH);
            }
            else
            {
                digitalWrite(LED_BUILTIN, LOW);
            }
            g_count = 0;
            g_rssi = 0;
        }
    }

    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
}
