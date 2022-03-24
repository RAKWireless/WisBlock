/**
   @file Rak11300_LoRaWAN_Sensor_Veml7700.ino
   @author rakwireless.com
   @brief Get the Light Sensor data and send it through LoRaWan
   @version 0.1
   @date 2021-07-015
   @copyright Copyright (c) 2021

*/
#include <Arduino.h>
#include <LoRaWan-Arduino.h>  // Click to install library: http://librarymanager/All#SX126x
#include "Light_VEML7700.h"  // Click to install library: http://librarymanager/All#RAK12010
#include <SPI.h>
#include <stdio.h>
#include "mbed.h"
#include "rtos.h"

using namespace std::chrono_literals;
using namespace std::chrono;

//veml770 Initialize
Light_VEML7700 VMEL = Light_VEML7700();
void Sensor_VEML7700_Print(void);
void Sensor_VEML7700_Init(void);
void LoRaWan_OTaa_Init(void);

bool doOTAA = true;   // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60                      /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0                   /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5             /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3                      /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;         /* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_EU868;    /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_UNCONFIRMED_MSG;         /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;                      /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t g_lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);
void lorawan_unconf_finished(void);
void lorawan_conf_finished(bool result);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t g_lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                          lorawan_rx_handler, lorawan_has_joined_handler,
                                          lorawan_confirm_class_handler, lorawan_join_failed_handler,
                                          lorawan_unconf_finished, lorawan_conf_finished
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
#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 20000                        /**< Defines for user timer, the application data transmission interval. 20s, value in [ms]. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];            //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

mbed::Ticker appTimer;
void tx_lora_periodic_handler(void);

static uint32_t count = 0;
static uint32_t count_fail = 0;
static float LUX_VEML = 0;
static float WHITE_VEML = 0;
static float ALS = 0;

bool send_now = false;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize Serial for debug output
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
  LoRaWan_OTaa_Init();
  Sensor_VEML7700_Init();
}

/**@brief Function for LoRaWan initialize
   time:2021/07/15
*/
void LoRaWan_OTaa_Init(void)
{
  Serial.println("=====================================");
  Serial.println("Welcome to RAK11300 LoRaWan!!!");
  if (doOTAA)
  {
    Serial.println("Type: OTAA");
  }
  else
  {
    Serial.println("Type: ABP");
  }
  // Initialize LoRa chip.
  lora_rak11300_init();
  switch (g_CurrentRegion)
  {
    case LORAMAC_REGION_AS923:
      Serial.println("Region: AS923-1");
      break;
    case LORAMAC_REGION_AU915:
      Serial.println("Region: AU915");
      break;
    case LORAMAC_REGION_CN470:
      Serial.println("Region: CN470");
      break;
    case LORAMAC_REGION_CN779:
      Serial.println("Region: CN779");
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
    case LORAMAC_REGION_RU864:
      Serial.println("Region: RU864");
      break;
    case LORAMAC_REGION_AS923_2:
      Serial.println("Region: AS923-2");
      break;
    case LORAMAC_REGION_AS923_3:
      Serial.println("Region: AS923-3");
      break;
    case LORAMAC_REGION_AS923_4:
      Serial.println("Region: AS923-4");
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


/**@brief Function for sensor initialize
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

void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
  LUX_VEML = VMEL.readLux();
  WHITE_VEML = VMEL.readWhite();
  ALS = VMEL.readALS();
  delay(1000);
  // Every LORAWAN_APP_INTERVAL milliseconds send_now will be set
  // true by the application timer and collects and sends the data
  if (send_now)
  {
    send_now = false;
    send_lora_frame();
  }
}

/**@brief LoRa function for handling HasJoined event.
*/
void lorawan_has_joined_handler(void)
{
  Serial.println("OTAA Mode, Network Joined!");

  lmh_error_status ret = lmh_class_request(g_CurrentClass);
  if (ret == LMH_SUCCESS)
  {
    delay(1000);
    // Start the application timer. Time has to be in microseconds
    appTimer.attach(tx_lora_periodic_handler, (std::chrono::microseconds)(LORAWAN_APP_INTERVAL * 1000));
  }
}
/**@brief LoRa function for handling OTAA join failed
*/
static void lorawan_join_failed_handler(void)
{
  Serial.println("OTAA join failed!");
  Serial.println("Check your EUI's and Keys's!");
  Serial.println("Check if a Gateway is in range!");
}
/**@brief Function for handling LoRaWan received data from Gateway

   @param[in] app_data  Pointer to rx data
*/

void lorawan_unconf_finished(void)
{
  Serial.println("TX finished");
}

void lorawan_conf_finished(bool result)
{
  Serial.printf("Confirmed TX %s\n", result ? "success" : "failed");
}

void lorawan_rx_handler(lmh_app_data_t *app_data)
{
  Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
                app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

void lorawan_confirm_class_handler(DeviceClass_t Class)
{
  Serial.printf("switch to class %c done\n", "ABC"[Class]);
  // Informs the server that switch has occurred ASAP
  m_lora_app_data.buffsize = 0;
  m_lora_app_data.port = gAppPort;
  lmh_send(&m_lora_app_data, g_CurrentConfirm);
}

//time:2021/7/8
String data = "";
void send_lora_frame(void)
{
  uint32_t Lux = 0;
  uint32_t White = 0;
  uint32_t Als = 0;
  uint8_t loradatacount = 0;
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    Serial.println("1Not joined");
    return;
  }
  Lux = LUX_VEML * 100; //(VMEL.readLux() * 100);
  White = WHITE_VEML * 100;
  Als = ALS;
  data = "Lux = " + String(Lux) + " " + " White = " + String(White) + " " + " Als=" + String(Als) + " ";
  Serial.println(data);
  data = "";
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;
  m_lora_app_data.buffer[loradatacount++] = 0x08;
  m_lora_app_data.buffer[loradatacount++] = ',';
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((Lux & 0xFF000000) >> 24);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((Lux & 0x00FF0000) >> 16);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((Lux & 0x0000FF00) >> 8);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)(Lux &  0x000000FF);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((White & 0xFF000000) >> 24);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((White & 0x00FF0000) >> 16);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((White & 0x0000FF00) >> 8);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)(White &  0x000000FF);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)((Als & 0x0000FF00) >> 8);
  m_lora_app_data.buffer[loradatacount++] = (uint8_t)(Als & 0xFF);
  m_lora_app_data.buffsize = loradatacount;
  lmh_error_status error = lmh_send(&m_lora_app_data, g_CurrentConfirm);
  if (error == LMH_SUCCESS)
  {
    count++;
    //Serial.printf("lmh_send ok count %d\n", count);
  }
  else
  {
    count_fail++;
    //Serial.printf("lmh_send fail count %d\n", count_fail);
  }
  Serial.printf("lmh_send ok count %d---------lmh_send fail count %d\n", count, count_fail);
}

/**@brief Function for handling user timerout event.
*/
void tx_lora_periodic_handler(void)
{
  appTimer.attach(tx_lora_periodic_handler, (std::chrono::microseconds)(LORAWAN_APP_INTERVAL * 1000));
  Serial.println("Sending frame now...");
  // This is a timer interrupt, do not do lengthy things here. Signal the loop() instead
  send_now = true;
}
