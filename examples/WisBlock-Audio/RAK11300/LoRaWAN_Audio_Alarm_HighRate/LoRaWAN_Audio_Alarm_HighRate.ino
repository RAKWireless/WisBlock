/**
   @file LoRaWAN_Audio_Alarm.ino
   @author rakwireless.com
   @brief The microphone detects the noise threshold and sends an alarm through lora.
   When the ambient noise is greater than the set threshold, a warning will be generated.
   And the LED of WisBase will lights 2 seconds. If you connect the lora gateway,it will
   send a packet of alarm information to the gateway.
   @note  This example need use the microphones that can supports higher sampling rate.
   @version 0.1
   @date 2022-06-10

   @copyright Copyright (c) 2022
*/
#include <Arduino.h>
#include "LoRaWan-Arduino.h" //http://librarymanager/All#SX126x
#include <SPI.h>
#include <stdio.h>

#include "mbed.h"
#include "rtos.h"
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <PDM.h>

using namespace std::chrono_literals;
using namespace std::chrono;

Channel_Mode channels = left; // stereo or left or right
int frequency = 48000;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];
volatile uint8_t read_flag = 0;
//Alarm threshold
int audio_threshold = 8000;
int sendflag = 0;

bool doOTAA = true;   // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60										  /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0									  /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_15							/*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3										  /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;					/* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_EU868;    /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_CONFIRMED_MSG;				  /* confirm/unconfirm packet definition  LMH_UNCONFIRMED_MSG */
uint8_t gAppPort = LORAWAN_APP_PORT;							        /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t g_lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void lorawan_unconfirm_txdone_handler(void);
static void lorawan_confirm_txdone_handler(bool result);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t g_lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                          lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler,
                                          lorawan_unconfirm_txdone_handler, lorawan_confirm_txdone_handler
                                         };
//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = {0x12, 0x88, 0x12, 0x88, 0x44, 0x12, 0x33, 0x66};
uint8_t nodeAppEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0x12, 0x21, 0x12, 0x21, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x44, 0x44, 0x44, 0x44};

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = {0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23};
uint8_t nodeAppsKey[16] = {0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C};

// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 10000                        /**< Defines for user timer, the application data transmission interval. 10s, value in [ms]. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];            //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.


int join_flag = 0;
TimerEvent_t ledTimer;
void led_off_timer_handler(void);

mbed::Ticker appTimer;
void tx_lora_periodic_handler(void);

static uint32_t count = 0;
static uint32_t count_fail = 0;

int abs_int(short data);
void onPDMdata();

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

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

  // Initialize LoRa chip.
  lora_rak11300_init();

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
  uint32_t  err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
  if (err_code != 0)
  {
    Serial.printf("lmh_init failed - %d\n", err_code);
    return;
  }

  // Start Join procedure
  lmh_join();
  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  // initialize PDM with:
  // - one channel (mono mode)
  // - a 200 kHz sample rate
  delay(6000);  //wait loraWAN joined
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }
  TimerInit(&ledTimer, led_off_timer_handler);

}
void loop()
{
  // wait for samples to be read
  if (read_flag == 1) {
    read_flag = 0;
    uint32_t sum = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum = sum + abs(sampleBuffer[i]);
    }
    int aver = sum / BUFFER_SIZE;
    if (aver > audio_threshold)
    {
      Serial.println("Alarm");
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      TimerSetValue(&ledTimer, 2000);
      TimerStart(&ledTimer);
      if (sendflag == 0)
      {
        sendflag = 1;
        if (join_flag == 1)
        {
          send_lora_frame();
        }
      }
    }
  }
}
int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}
/**@brief LoRa function for handling HasJoined event.
*/
void lorawan_has_joined_handler(void)
{
  if (doOTAA == true)
  {
    Serial.println("OTAA Mode, Network Joined!");
  }
  else
  {
    Serial.println("ABP Mode");
  }

  lmh_error_status ret = lmh_class_request(g_CurrentClass);
  if (ret == LMH_SUCCESS)
  {
    join_flag = 1;
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
static void lorawan_unconfirm_txdone_handler(void)
{
  //  Serial.println("unconfirm tx done");
  sendflag = 0;
}
static void lorawan_confirm_txdone_handler(bool result)
{
  //   Serial.println("confirm tx done");
  sendflag = 0;
}
void send_lora_frame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    return;
  }

  uint32_t i = 0;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;
  m_lora_app_data.buffer[i++] = 'A';
  m_lora_app_data.buffer[i++] = 'L';
  m_lora_app_data.buffer[i++] = 'A';
  m_lora_app_data.buffer[i++] = 'R';
  m_lora_app_data.buffer[i++] = 'M';
  m_lora_app_data.buffer[i++] = '!';
  m_lora_app_data.buffsize = i;

  lmh_error_status error = lmh_send(&m_lora_app_data, g_CurrentConfirm);
  if (error == LMH_SUCCESS)
  {
    count++;
    Serial.printf("lmh_send ok count %d\n", count);
  }
  else
  {
    count_fail++;
    Serial.printf("lmh_send fail count %d\n", count_fail);
  }
}
void led_off_timer_handler(void)
{
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
}
/**@brief Function for handling user timerout event.
*/
void tx_lora_periodic_handler(void)
{
  appTimer.attach(tx_lora_periodic_handler, (std::chrono::microseconds)(LORAWAN_APP_INTERVAL * 1000));
  // This is a timer interrupt, do not do lengthy things here. Signal the loop() instead
  sendflag = 0;
}

void onPDMdata() {
  // query the number of bytes available
  // read into the sample buffer
  PDM.read((uint8_t *)sampleBuffer, BUFFER_SIZE * 2);
  read_flag = 1;
}
