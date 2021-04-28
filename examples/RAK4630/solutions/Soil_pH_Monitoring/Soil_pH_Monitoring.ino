/**
 * @file Soil_pH_Monitoring.ino
 * @author rakwireless.com
 * @brief This sketch demonstrate reading soil pH values
 *    and send the data to lora gateway.
 * @version 0.1
 * @date 2020-07-28 
 * @copyright Copyright (c) 2020
 */
#include <ArduinoModbus.h> //Click here to get the library: http://librarymanager/All#ArduinoModbus

#include <ArduinoRS485.h> //Click here to get the library: http://librarymanager/All#ArduinoRS485

#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //Click here to get the library: http://librarymanager/All#SX126x
#include <SPI.h>

// RAK4630 supply two LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif

bool doOTAA = true;   // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60                     /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0                   /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5             /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3                      /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;         /* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_EU868;    /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_UNCONFIRMED_MSG;         /* confirm/unconfirm packet definition*/
uint8_t g_AppPort = LORAWAN_APP_PORT;                     /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
 */
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

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
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
#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 20000                        /**< Defines for user timer, the application data transmission interval. 20s, value in [ms]. */
static uint8_t g_m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];        //< Lora user application data buffer.
static lmh_app_data_t g_m_lora_app_data = {g_m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

TimerEvent_t g_appTimer;
static uint32_t timers_init(void);
static uint32_t g_count = 0;
static uint32_t g_count_fail = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize LoRa chip.
  lora_rak4630_init();

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

  Serial.println("=====================================");
  Serial.println("Welcome to RAK4630 LoRaWan!!!");

  if (!ModbusRTUClient.begin(9600))
  {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1)
      ;
  }
  
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

  Scheduler.startLoop(loop2);

  // Initialize Scheduler and timer
  uint32_t err_code;

  err_code = timers_init();
  if (err_code != 0)
  {
    Serial.printf("timers_init failed - %d\n", err_code);
    return;
  }

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
  err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
  if (err_code != 0)
  {
    Serial.printf("lmh_init failed - %d\n", err_code);
    return;
  }

  // Start Join procedure
  lmh_join();
}

void loop2()
{
  unsigned short raw_ph;
  uint32_t i = 0;

  raw_ph = get_ph();
  Serial.printf("-----raw_ph = 0x%x-----\n", raw_ph);
  g_m_lora_app_data.port = g_AppPort;
  g_m_lora_app_data.buffer[i++] = 0x05;
  g_m_lora_app_data.buffer[i++] = (raw_ph >> 8) & 0x00FF;
  g_m_lora_app_data.buffer[i++] = raw_ph & 0x00FF;
  g_m_lora_app_data.buffsize = i;
  delay(10000);
}

void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions. 
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
    TimerSetValue(&g_appTimer, LORAWAN_APP_INTERVAL);
    TimerStart(&g_appTimer);
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
 *
 * @param[in] app_data  Pointer to rx data
 */
void lorawan_rx_handler(lmh_app_data_t *app_data)
{
  Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
          app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

void lorawan_confirm_class_handler(DeviceClass_t Class)
{
  Serial.printf("switch to class %c done\n", "ABC"[Class]);
  // Informs the server that switch has occurred ASAP
  g_m_lora_app_data.buffsize = 0;
  g_m_lora_app_data.port = g_AppPort;
  lmh_send(&g_m_lora_app_data, g_CurrentConfirm);
}

void send_lora_frame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    return;
  }

  lmh_error_status error = lmh_send(&g_m_lora_app_data, g_CurrentConfirm);
  if (error == LMH_SUCCESS)
  {
    g_count++;
    Serial.printf("lmh_send ok count %d\n", g_count);
  }
  else
  {
    g_count_fail++;
    Serial.printf("lmh_send fail count %d\n", g_count_fail);
  }
  TimerSetValue(&g_appTimer, LORAWAN_APP_INTERVAL);
  TimerStart(&g_appTimer);
}

unsigned short get_ph(void)
{
  unsigned short rawph;
  float ph;

  /* RS485 Power On */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(100);
  /* RS485 Power On */

  if (!ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x0006, 1))
  {
    Serial.print("failed to read registers! ");
    Serial.println(ModbusRTUClient.lastError());
  }
  else
  {
    // If the request succeeds, the sensor sends the readings, that are
    // stored in the holding registers. The read() method can be used to
    // get the par values.
    rawph = ModbusRTUClient.read();

    // To get the ph reading as a percentage, divide the raw value by 100.00.
    ph = rawph / 100.00;
    ph = (int)(ph / 0.01) * 0.01;
    Serial.printf("-------ph------ = %02f\n", ph);
  }

  /* RS485 Power Off */
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, LOW);
  delay(100);
  /* RS485 Power Off */

  return rawph;
}

/**@brief Function for handling user timerout event.
 */
void tx_lora_periodic_handler(void)
{
  send_lora_frame();
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
uint32_t timers_init(void)
{
  TimerInit(&g_appTimer, tx_lora_periodic_handler);
  return 0;
}
