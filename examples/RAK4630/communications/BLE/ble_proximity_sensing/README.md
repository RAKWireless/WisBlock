# BLE proximity sensing examples
----
This example shows how to setup two WisBlock devices and use BLE to detect each other and give a visual signal if the two modules can see each other.     

Both devices are using BLE advertising and BLE device scanning functionality of BLE. 

![ble-proximity](../../../../../assets/Examples/ble-proximity.png)

----
## Hardware required

----
To build this system the WisBlock Core RAK4631 microcontroller and WisBlock Base RAK5005-O no additional modules are required. With just WisBlock Core RAK4631 plugged into the WisBlock Base RAK5005-O board the system is ready to be used.

- WisBlock Base RAK5005-O  \*    2 pcs
- WisBlock Core RAK4631  \*    2 pcs

The assembly is as follows:

![ble-rak4631-proxy](../../../../../assets/repo/ble-rak4631-proxy.png)

For stable connection, please use screws to tighten.

----
## Software required
----

- [ArduinoIDE](https://www.arduino.cc/en/Main/Software)    
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)    

----

We design a scene that device A and device B are both set as advertising role and scanner. Their device name are as "RAK4630-XXXXXX", and the "XXXXXX" is the last three bytes of device's MAC address. They both advertise their device name and RSSI value and also scan the two value. When device A scanned device B whose device name is like "RAK4630-XXXXXX" and the RSSI of device is larger than the threshold we set, green light of device A  will be on. The threshold value is an approximate, you can change it according the specific application scenarios.

The test example code is as follow：

```
/*
 * This sketch demonstrate how to run both Central and Peripheral roles
 * at the same time.
 *
 * node1 <--> DualRole <--> node2
 * When device A and device B are close, the green light on the board is on.
 * Otherwise, the green light is off.
 */
#include <bluefruit.h>

#define MAX_COUNT_NUM (5)
#define AVERAGE_RSSI_VAL (-40)
int32_t count = 0;
int32_t rssi = 0;
void startAdv(void);
void scan_callback(ble_gap_evt_adv_report_t* report);

void setup()
{
  Serial.begin(115200);
  // while ( !Serial ) delay(10);

  pinMode(PIN_LED1, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  uint8_t mac[6] = {0};
  char deviceName[16] = {0};

  Serial.println("RAK4630 Dual Role BLEUART Example");
  Serial.println("-------------------------------------\n");

  // Initialize Bluefruit with max concurrent connections as Peripheral = 1, Central = 1
  // SRAM usage required by SoftDevice will increase with number of connections
  Bluefruit.begin(1, 1);
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
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
   Bluefruit.Scanner.setRxCallback(scan_callback);
   Bluefruit.Scanner.restartOnDisconnect(true);
   Bluefruit.Scanner.filterRssi(-80);            // Only invoke callback for devices with RSSI >= -80 dBm
   Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
   Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
   Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
   Serial.println("Scanning ...");

   // Set up and start advertising
   startAdv();
}

void startAdv(void)
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
   Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
   Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
   Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop()
{
  // do nothing, all the work is done in callback
}

/**
 * when the Bluetooth node scans that there is a Bluetooth broadcast with the prefix of ‘rak4630-’ around
 * and the RSSI of the Bluetooth broadcast is less than the set threshold value,
 * the light is on. On the contrary, the light is off.
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  uint8_t len = 0;
  uint8_t buffer[32];
  memset(buffer, 0, sizeof(buffer));

  /* Check for UUID128 Complete List */
  len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, buffer, sizeof(buffer));
  if ( len == 14 && strncmp((char *)buffer, "RAK4630-", 8) == 0 )
  {
    count++;
    rssi += report->rssi;

    if(count == MAX_COUNT_NUM)
    {
        Serial.printf("===rssi: %d===\n", rssi / MAX_COUNT_NUM);
        if( (rssi / MAX_COUNT_NUM) >= AVERAGE_RSSI_VAL )
        {
          digitalWrite(LED_BUILTIN, HIGH);
        }
        else
        {
          digitalWrite(LED_BUILTIN, LOW);
        }
        count = 0;
        rssi = 0;
    }
  }

  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}
```



## Test Results

When we burn the above examples to device a and device B respectively through Arduino IDE, and connect them to the PC through USB, open the serial port tool on the PC, set the baud rate to 115200, and connect to the two devices respectively, we will see that the broadcast RSSI of the other device is printed on the serial port tool. 

The log is as follow:

```
[10:05:10.100]===rssi: -40===

[10:05:12.004]===rssi: -38===

[10:05:13.729]===rssi: -42===

[10:05:15.633]===rssi: -42===

[10:05:18.136]===rssi: -44===

[10:05:21.117]===rssi: -41===

[10:05:25.540]===rssi: -39===

[10:05:28.039]===rssi: -44===

[10:05:29.921]===rssi: -45===

[10:05:32.124]===rssi: -43===

[10:05:34.618]===rssi: -42===

[10:05:36.531]===rssi: -44===

[10:05:44.421]===rssi: -37===

[10:05:51.203]===rssi: -43===

[10:05:58.588]===rssi: -40===

[10:06:02.995]===rssi: -45===

[10:06:10.520]===rssi: -45===

[10:06:15.226]===rssi: -45===

[10:06:17.424]===rssi: -45===

[10:06:26.410]===rssi: -46===

[10:06:29.725]===rssi: -45===

[10:06:36.786]===rssi: -35===

[10:06:39.309]===rssi: -34===

[10:06:40.715]===rssi: -34===

[10:06:42.598]===rssi: -34===

[10:06:44.477]===rssi: -34===
```

If the value of RSSI in log of device A is larger than -40, green light of device B will be on. If the value of RSSI in log of device B is larger than -40, green light of device A will be on.







