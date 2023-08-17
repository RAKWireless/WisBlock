#include <Arduino.h>
#include <LoRaWan-RAK4630.h>  //http://librarymanager/All#SX126x
#include <SPI.h>
#include <Wire.h>

// For RAK12014 module
#include <vl53l0x_class.h>    // Click to install library: http://librarymanager/All#stm32duino_vl53l0x
VL53L0X sensor_vl53l0x(&Wire, WB_IO2);

uint32_t distance;
int status;

// For RAK13001 module 
#define RELAY_PIN WB_IO4
int object;

// For RAK12029 module
#include "RAK12029_LDC1614.h"

#define INDUCTANCE 13.000
#define CAPATANCE 100.0
#define GETCHANNEL LDC1614_CHANNEL_0     // LDC1614_CHANNEL_(0~3); selecting the coil channel from 0 - 3
RAK12029_LDC1614_Inductive ldc;

u16 value = 0;
u32 ChannelData = 0;

#ifdef RAK4630
#define BOARD "RAK4631 "
#define RAK4631_BOARD true
#else
#define RAK4631_BOARD false
#endif

bool doOTAA = true;                                               // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60                                       /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0                                     /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5                               /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3                                        /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;                           /* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_US915;           /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_CONFIRMED_MSG;                 /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;                              /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t g_lora_param_init = { LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF };

// Forward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t g_lora_callbacks = { BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                           lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler };
//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = { 0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x06, 0xD3, 0xE9 };
uint8_t nodeAppEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t nodeAppKey[16] = { 0xC0, 0x81, 0xBB, 0x65, 0x54, 0xCF, 0xF7, 0x65, 0x87, 0x70, 0xFD, 0x31, 0x7F, 0x01, 0x87, 0x51 };

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = { 0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23 };
uint8_t nodeAppsKey[16] = { 0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C };

// Private definition
#define LORAWAN_APP_DATA_BUFF_SIZE 64                                            /**< buffer size of the data to be transmitted. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];               //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = { m_lora_app_data_buffer, 0, 0, 0, 0 };  //< Lora user application data structure.

static uint32_t count = 0;
static uint32_t count_fail = 0;

void setup() {
  // Sensor Power Switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  // Control Chip Switch
  pinMode(WB_IO5, OUTPUT);
  digitalWrite(WB_IO5, LOW);

  // Control Relay Module
  pinMode(RELAY_PIN,OUTPUT);
  digitalWrite(RELAY_PIN, LOW);    // relay module initially LOW

  pinMode(LED_GREEN, OUTPUT);      // Output LED
  pinMode(LED_BLUE, OUTPUT);       // Output LED

  // Initialize LoRa chip.
  lora_rak4630_init();

  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  Serial.println("=====================================");
  Serial.println("Welcome to RAK4630 LoRaWan!!!");
  if (doOTAA) {
    Serial.println("Type: OTAA");
  } else {
    Serial.println("Type: ABP");
  }

  switch (g_CurrentRegion) {
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
  if (doOTAA) {
    lmh_setDevEui(nodeDeviceEUI);
    lmh_setAppEui(nodeAppEUI);
    lmh_setAppKey(nodeAppKey);
  } else {
    lmh_setNwkSKey(nodeNwsKey);
    lmh_setAppSKey(nodeAppsKey);
    lmh_setDevAddr(nodeDevAddr);
  }

  uint32_t err_code;
  // Initialize LoRaWan
  err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
  if (err_code != 0) {
    Serial.printf("lmh_init failed - %d\n", err_code);
    return;
  }

  // Start Join procedure
  lmh_join();

  Serial.println("===========================================");
  Serial.println("RAK12014 + RAK12029 + RAK13001 LoRaWAN Code");
  Serial.println("===========================================");

  Wire.begin();

  // Configure VL53L0X component.
  sensor_vl53l0x.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X component.
  status = sensor_vl53l0x.InitSensor(0x52);
  if (status) {
    Serial.println("Init sensor_vl53l0x failed...");
  }

  // From RAK12029 
  ldc.LDC1614_init();
  /*single channel use case configuration.*/
  if (ldc.LDC1614_single_channel_config(GETCHANNEL, INDUCTANCE, CAPATANCE)) {
    Serial.println("can't detect sensor!");
    while (1) delay(100);
  }
}

void loop() {
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.

  // Read data from RAK12014
  status = sensor_vl53l0x.GetDistance(&distance);

  // Read data from RAK12029
  ldc.IIC_read_16bit(LDC1614_READ_DEVICE_ID, &value);
  if (value == 0x3055) {
    //u32 ChannelData = 0;
    /*Get channel 0 result and parse it.*/
    delay(100);
    if (ldc.LDC1614_get_channel_result(GETCHANNEL, &ChannelData) == 0) {
      /*sensor result value.you can make a lot of application according to its changes.*/
      if (0 != ChannelData) {
        digitalWrite(LED_BLUE, HIGH);     // Indicator Blue LED 
        digitalWrite(LED_GREEN, HIGH);    // Indicator Green LED
      }
      delay(200);
      digitalWrite(LED_BLUE, LOW);        // Indicator Blue LED 
      digitalWrite(LED_GREEN, LOW);       // Indicator Green LED
    }
  }

  if (ChannelData >= 49900000)            // metallic material sensed nearby via inductive sensor; you can adjust this value to calibrate operation
  {
    digitalWrite(RELAY_PIN, HIGH);        // Turns ON the relay module
    object = 1;                           // "1" corresponds to metallic object
    Serial.println("Metal Detected");                     
  }

  else if (ChannelData <= 49800000)       // nothing metallic was sensed; default value sensed by the inductive sensor; you can adjust this value to calibrate operation
  {
    digitalWrite(RELAY_PIN, LOW);         // Turns OFF the relay module
    object = 0;                           // "0" corresponds to non-metallic object
    Serial.println("No Metal Detected");
  }

  Serial.println("Sending frame now...");
  send_lora_frame();

  Serial.print(distance);       // From RAK12014 module
  Serial.println("mm   ");
  Serial.println(object);       // From RAK13001 module
  Serial.println(ChannelData);  // From RAK12029 module

  delay(5000);
}

/**@brief LoRa function for handling HasJoined event.
 */
void lorawan_has_joined_handler(void) {
  Serial.println("OTAA Mode, Network Joined!");
}

/**@brief LoRa function for handling OTAA join failed
*/
static void lorawan_join_failed_handler(void) {
  Serial.println("OTAA join failed!");
  Serial.println("Check your EUI's and Keys's!");
  Serial.println("Check if a Gateway is in range!");
}
/**@brief Function for handling LoRaWan received data from Gateway
 *
 * @param[in] app_data  Pointer to rx data
 */
void lorawan_rx_handler(lmh_app_data_t *app_data) {
  Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
                app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

void lorawan_confirm_class_handler(DeviceClass_t Class) {
  Serial.printf("switch to class %c done\n", "ABC"[Class]);
  // Informs the server that switch has occurred ASAP
  m_lora_app_data.buffsize = 0;
  m_lora_app_data.port = gAppPort;
  lmh_send(&m_lora_app_data, g_CurrentConfirm);
}

String data = "";

void send_lora_frame(void) {
  if (lmh_join_status_get() != LMH_SET) {
    //Not joined, try again later
    return;
  }

  Serial.print("result: ");
  uint32_t i = 0;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;

  data = " Distance: " + String(distance) + " mm " + " Metallic: " + String(object) + " Inductive_Sense: " + String(ChannelData);
  Serial.println(data);

  uint32_t distance_data = distance;    // data from RAK12014 module
  uint32_t channel_data = ChannelData;  // data from RAK12029 module
  int metallic_data = object;           // data from RAK13001 module

  m_lora_app_data.buffer[i++] = 0x01;  // byte[0]

  // Data from RAK12014 module
  m_lora_app_data.buffer[i++] = (uint8_t)((distance_data & 0xFF000000) >> 24);    // byte[1]
  m_lora_app_data.buffer[i++] = (uint8_t)((distance_data & 0x00FF0000) >> 16);    // byte[2]
  m_lora_app_data.buffer[i++] = (uint8_t)((distance_data & 0x0000FF00) >> 8);     // byte[3]
  m_lora_app_data.buffer[i++] = (uint8_t)(distance_data & 0x000000FF);            // byte[4]

  // Data from RAK12029 module (inductive sensor)
  m_lora_app_data.buffer[i++] = (uint8_t)((channel_data & 0xFF000000) >> 24);    // byte[5]
  m_lora_app_data.buffer[i++] = (uint8_t)((channel_data & 0x00FF0000) >> 16);    // byte[6]
  m_lora_app_data.buffer[i++] = (uint8_t)((channel_data & 0x0000FF00) >> 8);     // byte[7]
  m_lora_app_data.buffer[i++] = (uint8_t)(channel_data & 0x000000FF);            // byte[8]

  // Data from RAK13001 module (relay module status)
  m_lora_app_data.buffer[i++] = metallic_data;                                   // byte[9]

  m_lora_app_data.buffsize = i;

  lmh_error_status error = lmh_send(&m_lora_app_data, g_CurrentConfirm);
  if (error == LMH_SUCCESS) {
    count++;
    Serial.printf("lmh_send ok count %d\n", count);
  } else {
    count_fail++;
    Serial.printf("lmh_send fail count %d\n", count_fail);
  }
}