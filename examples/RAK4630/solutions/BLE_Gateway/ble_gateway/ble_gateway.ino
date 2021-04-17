/**
 * @file ble_gateway.ino
 * @author rakwireless.com
 * @brief This sketch demonstrate how to get environment data from BME680
 *    and send the data to lora gateway.
 *    ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
 * @version 0.1
 * @date 2020-07-28
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

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //Click here to get the library: http://librarymanager/All#SX126x
#include <SPI.h>
#include <bluefruit.h>

// RAK4630 supply two LED
// #ifndef LED_BUILTIN
// #define LED_BUILTIN 35
// #endif

// #ifndef LED_BUILTIN2
// #define LED_BUILTIN2 36
// #endif

bool doOTAA = true;   // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE // < Maximum size of scheduler events.
#define SCHED_QUEUE_SIZE 60                                       // < Maximum number of events in the scheduler queue.
#define LORAWAN_DATERATE DR_0                                     // LoRaMac datarates definition, from DR_0 to DR_5
#define LORAWAN_TX_POWER TX_POWER_5                               // LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15
#define JOINREQ_NBTRIALS 3                                        // < Number of trials for the join request.
DeviceClass_t g_CurrentClass = CLASS_A;                          // class definition
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_EU868;    /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_UNCONFIRMED_MSG;                // confirm/unconfirm packet definition
uint8_t g_AppPort = LORAWAN_APP_PORT;                            // data port

// Structure containing LoRaWan parameters, needed for lmh_init()
static lmh_param_t g_lora_param_init = {
    LORAWAN_ADR_ON,
    LORAWAN_DATERATE,
    LORAWAN_PUBLIC_NETWORK,
    JOINREQ_NBTRIALS,
    LORAWAN_TX_POWER,
    LORAWAN_DUTYCYCLE_OFF
};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

// Structure containing LoRaWan callback functions, needed for lmh_init()
static lmh_callback_t g_lora_callbacks = {
    BoardGetBatteryLevel,
    BoardGetUniqueId,
    BoardGetRandomSeed,
    lorawan_rx_handler,
    lorawan_has_joined_handler,
    lorawan_confirm_class_handler,
    lorawan_join_failed_handler
};

//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x33, 0x33};
uint8_t nodeAppEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88};

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = {0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23};
uint8_t nodeAppsKey[16] = {0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C};

// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64                                    // buffer size of the data to be transmitted.
static uint8_t g_LoraAppDataBuff[LORAWAN_APP_DATA_BUFF_SIZE];            // Lora user application data buffer.
static lmh_app_data_t g_LoraAppData = {g_LoraAppDataBuff, 0, 0, 0, 0};   // Lora user application data structure.

static uint32_t g_Count = 0;
static uint32_t g_CountFail = 0;

/* Environment Service Definitions
 * Environment Monitor Service:  0x181A
 * Pressure Measurement Char: 0x2A6D
 * Temperature Measurement Char: 0x2A6E
 * Temperature Measurement Char: 0x2A6F
*/
BLEClientService g_EnvironmentClientService(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLEClientCharacteristic g_PressureCliCharacteristic(0x2A6D);                        // pressure
BLEClientCharacteristic g_TemperatureCliCharacteristic(UUID16_CHR_TEMPERATURE);     // temperature
BLEClientCharacteristic g_HumidityCliCharacteristic(0x2A6F);                        // humidity

uint8_t g_TemperatureFlag = 0;
uint8_t g_HumdityFlag = 0;
uint8_t g_PressureFlag = 0;
int16_t g_Temperature;
int32_t g_Pressure;
uint16_t g_Humidity;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    // Initialize LoRa chip.
    lora_rak4630_init();

    // Initialize Serial for debug output
    Serial.begin(115200);
    // Wait for USB Serial to be ready or terminal to be connected
    time_t timeout = millis(); // Timeout in case the system runs on its own
    // Waiting for Serial
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            delay(100);
        }
        else
        {
            // Timeout while waiting for USB Serial
            break;
        }
    }
    Serial.println("=====================================");
    Serial.println("Welcome to RAK4630 LoRaWan!!!");
    if (doOTAA)
    {
      Serial.println("Type: OTAA");
    }
    else
    {
      Serial.println("Type: ABP");
    }

    switch (g_CurrentRegion)
    {
      case LORAMAC_REGION_AS923:
        Serial.println("Region: AS923");
        break;
      case LORAMAC_REGION_AU915:
        Serial.println("Region: AU915");
        break;
      case LORAMAC_REGION_CN470:
        Serial.println("Region: CN470");
        break;
      case LORAMAC_REGION_EU433:
        Serial.println("Region: EU433");
        break;
      case LORAMAC_REGION_IN865:
        Serial.println("Region: IN865");
        break;
      case LORAMAC_REGION_EU868:
        Serial.println("Region: EU868");
        break;
      case LORAMAC_REGION_KR920:
        Serial.println("Region: KR920");
        break;
      case LORAMAC_REGION_US915:
        Serial.println("Region: US915");
        break;
    }
    Serial.println("=====================================");
  
    // Setup the EUIs and Keys
    if (doOTAA)
    {
      lmh_setDevEui(nodeDeviceEUI);
      lmh_setAppEui(nodeAppEUI);
      lmh_setAppKey(nodeAppKey);
    }
    else
    {
      lmh_setNwkSKey(nodeNwsKey);
      lmh_setAppSKey(nodeAppsKey);
      lmh_setDevAddr(nodeDevAddr);
    }

    // Initialize LoRaWan
    uint32_t err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
    if (err_code != 0)
    {
      Serial.printf("lmh_init failed - %d\n", err_code);
      return;
    }

    // Start Join procedure
    lmh_join();
}

void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
}

/**
   @brief LoRa function for handling HasJoined event.
*/
void lorawan_has_joined_handler(void)
{
    Serial.println("OTAA Mode, Network Joined!");

    lmh_error_status ret = lmh_class_request(g_CurrentClass);
    if (ret == LMH_SUCCESS)
    {
        delay(1000);
        start_ble();
    }
}
/**@brief LoRa function for handling OTAA join failed
*/
static void lorawan_join_failed_handler(void)
{
  Serial.println("OVER_THE_AIR_ACTIVATION failed!");
  Serial.println("Check your EUI's and Keys's!");
  Serial.println("Check if a Gateway is in range!");
}
/**
   @brief Function for handling LoRaWan received data from Gateway
   @param[in] app_data  Pointer to rx data
*/
void lorawan_rx_handler(lmh_app_data_t *app_data)
{
    Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
                    app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

/**
   @brief Function for handling LoRaWan received data from Gateway
   @param[in] app_data  Pointer to rx data
*/
void lorawan_confirm_class_handler(DeviceClass_t Class)
{
    Serial.printf("switch to class %c done\n", "ABC"[Class]);
    // Informs the server that switch has occurred ASAP
    g_LoraAppData.buffsize = 0;
    g_LoraAppData.port = g_AppPort;
    lmh_send(&g_LoraAppData, g_CurrentConfirm);
}

/**
   @brief start ble scanner
*/
void start_ble()
{
    // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
    // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 1);
    Bluefruit.setName("RAKwisnode Central");
    Bluefruit.setTxPower(4);

    // Initialize environment client
    g_EnvironmentClientService.begin();

    // Initialize client characteristics of environment.
    // Note: Client Char will be added to the last service that is begin()ed.
    // set up callback for receiving measurement
    g_PressureCliCharacteristic.setNotifyCallback(environment_notify_callback);
    g_PressureCliCharacteristic.begin();

    g_TemperatureCliCharacteristic.setNotifyCallback(environment_notify_callback);
    g_TemperatureCliCharacteristic.begin();

    g_HumidityCliCharacteristic.setNotifyCallback(environment_notify_callback);
    g_HumidityCliCharacteristic.begin();

    // Increase Blink rate to different from PrPh advertising mode
    Bluefruit.setConnLedInterval(250);

    // Callbacks for Central
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    Bluefruit.Central.setConnectCallback(connect_callback);

    /* Start Central Scanning
     - Enable auto scan if disconnected
     - Interval = 100 ms, window = 80 ms
     - Don't use active scan
     - Filter only accept g_EnvironmentClientService service
     - Start(timeout) with timeout = 0 will scan forever (until connected)
    */
    Bluefruit.Scanner.setRxCallback(scan_callback);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
    Bluefruit.Scanner.useActiveScan(true);
    Bluefruit.Scanner.filterUuid(g_EnvironmentClientService.uuid);
    Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds
}

/**
   @brief Callback invoked when scanner pick up an advertising data
   @param report Structural advertising data
*/
void scan_callback(ble_gap_evt_adv_report_t *report)
{
    // Since we configure the scanner with filterUuid()
    // Scan callback only invoked for device with g_EnvironmentClientService service advertised
    // Connect to device with g_EnvironmentClientService service in advertising
    Bluefruit.Central.connect(report);
}

/**
   @brief Callback invoked when an connection is established
   @param conn_handle
*/
void connect_callback(uint16_t conn_handle)
{
    Serial.println("Connected");
    Serial.print("Discovering g_EnvironmentClientService Service ... ");

    // If g_EnvironmentClientService is not found, disconnect and return
    if (!g_EnvironmentClientService.discover(conn_handle))
    {
        Serial.println("Found NONE");

        // disconnect since we couldn't find g_EnvironmentClientService service
        Bluefruit.disconnect(conn_handle);

        return;
    }

    // Once g_EnvironmentClientService service is found, we continue to discover its characteristic
    Serial.println("Found it");

    Serial.println("Discovering Measurement characteristic ... ");
    if (!g_PressureCliCharacteristic.discover() || !g_TemperatureCliCharacteristic.discover() || !g_HumidityCliCharacteristic.discover())
    {
        // Measurement chr is mandatory, if it is not found (valid), then disconnect
        Serial.println("Measurement characteristic is mandatory but not found");
        Bluefruit.disconnect(conn_handle);
        return;
    }
    else
    {
        Serial.println("Found measurement characteristic.");
    }

    // Reaching here means we are ready to go, let's enable notification on measurement chr
    if (g_PressureCliCharacteristic.enableNotify())
    {
        Serial.println("Ready to receive Pressure value.");
    }
    else
    {
        Serial.println("Couldn't enable notify for Pressure characteristic.");
    }
    if (g_TemperatureCliCharacteristic.enableNotify())
    {
        Serial.println("Ready to receive Temperature value.");
    }
    else
    {
        Serial.println("Couldn't enable notify for Temperature characteristic.");
    }
    if (g_HumidityCliCharacteristic.enableNotify())
    {
        Serial.println("Ready to receive Humidity value.");
    }
    else
    {
        Serial.println("Couldn't enable notify for Humidity characteristic.");
    }
}

/**
   @brief Callback invoked when a connection is dropped
   @param conn_handle
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}

/**
   @brief Hooked callback that triggered when a measurement value is sent from peripheral
   @param chr   Pointer client characteristic that even occurred,
                in this example it should be humidity, pressure or temperature
   @param data  Pointer to received data
   @param len   Length of received data
*/
void environment_notify_callback(BLEClientCharacteristic *chr, uint8_t *data, uint16_t len)
{
    //https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml

    switch (chr->uuid._uuid.uuid)
    {
        case 0x2A6D:
        {
            if (len == 4)
            {
                memcpy(&g_Pressure, data, len);
                Serial.printf("Pressure data: %.1f Pa\n", (float)g_Pressure * 0.1);
                g_PressureFlag = 1;
            }
            else
            {
                Serial.printf("Invalid pressure data\n");
            }
        }
        break;
        case 0x2A6E:
        {
            if (len == 2)
            {
                memcpy(&g_Temperature, data, len);
                Serial.printf("Temperature data: %.2f Cel\n", (float)g_Temperature * 0.01);
                g_TemperatureFlag = 1;
            }
            else
            {
                Serial.printf("Invalid temperature data\n");
            }
        }
        break;
        case 0x2A6F:
        {
            if (len == 2)
            {
                memcpy(&g_Humidity, data, len);
                Serial.printf("Humidity data: %.2f %%\n", (float)g_Humidity * 0.01);
                g_HumdityFlag = 1;
            }
            else
            {
                Serial.printf("Invalid humidity data\n");
            }
        }
        break;
        default:
            break;
    }

    if (g_TemperatureFlag == 1 && g_HumdityFlag == 1 && g_PressureFlag == 1)
    {
        uint32_t payloadLen = 0;
        memset(g_LoraAppData.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
        g_LoraAppData.port = g_AppPort;
        g_LoraAppData.buffer[payloadLen++] = 0x01; // temperature
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Temperature >> 8) & 0xff;
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Temperature)&0xff;

        g_LoraAppData.buffer[payloadLen++] = 0x03; // humidity
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Humidity >> 8) & 0xff;
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Humidity)&0xff;

        g_LoraAppData.buffer[payloadLen++] = 0x04; // pressure
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Pressure >> 24) & 0xff;
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Pressure >> 16) & 0xff;
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Pressure >> 8) & 0xff;
        g_LoraAppData.buffer[payloadLen++] = (uint8_t)(g_Pressure)&0xff;

        g_LoraAppData.buffsize = payloadLen;

        lmh_error_status error = lmh_send(&g_LoraAppData, g_CurrentConfirm);
        Serial.printf("lmh_send count %d\n", ++g_Count);
        if (error != LMH_SUCCESS)
        {
            Serial.printf("lmh_send fail count %d\n", ++g_CountFail);
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(100);
            digitalWrite(LED_BUILTIN, LOW);
        }
        g_TemperatureFlag = 0;
        g_HumdityFlag = 0;
        g_PressureFlag = 0;
    }
}
