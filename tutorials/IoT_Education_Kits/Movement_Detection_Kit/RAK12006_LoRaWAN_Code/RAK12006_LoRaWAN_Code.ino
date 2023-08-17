#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //http://librarymanager/All#SX126x
#include <SPI.h>
#include <Wire.h>

// Section to add include libraries, declarations and variables.

#define SENSOR_PIN  WB_IO6   // Attach AM312 sensor to Arduino Digital Pin WB_IO6

int gCurrentStatus = 0;         // variable for reading the pin current status
//int gCurrentStatus;  
int gLastStatus = 0;            // variable for reading the pin last status
//int gLastStatus; 

int sensor_val;
/*
// RAK4630 supply two LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif
*/

bool doOTAA = true;   // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60                     /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0                   /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5             /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3                      /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;         /* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_US915;    /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_CONFIRMED_MSG;         /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;                      /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t g_lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Forward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t g_lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                        lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler
                                       };
//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = {0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x06, 0xD3, 0xE9};
uint8_t nodeAppEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0x3F, 0xC4, 0x80, 0x0D, 0x4A, 0xE7, 0xAD, 0x9F, 0xDC, 0x27, 0x5D, 0x37, 0x2C, 0x4C, 0xD3, 0xF8};

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = {0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23};
uint8_t nodeAppsKey[16] = {0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C};

// Private definition
#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];            //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

static uint32_t count = 0;
static uint32_t count_fail = 0;

void setup()
{
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  pinMode(SENSOR_PIN, INPUT);        // PIR Sensor as an INPUT
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

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

  uint32_t err_code;
  // Initialize LoRaWan
  err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
  if (err_code != 0)
  {
    Serial.printf("lmh_init failed - %d\n", err_code);
    return;
  }

  // Start Join procedure
  lmh_join();

   Serial.println("========================");
   Serial.println("    RAK12006 LoRaWAN test");
   Serial.println("========================");

}

// Default void loop

void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
  gCurrentStatus = digitalRead(SENSOR_PIN);
  //if(gLastStatus != gCurrentStatus)
  //{
    if (gCurrentStatus == 0)
    // "0" for Detected; "1" for Undetected
    {
      digitalWrite(LED_GREEN, HIGH); // Turn ON LED
      digitalWrite(LED_BLUE, HIGH);
      sensor_val = 1;
      send_lora_frame();
      delay(2000);
    }
  
    else if (gCurrentStatus == 1)
    {
      // do nothing
      digitalWrite(LED_GREEN, LOW); // Turn OFF LED
      digitalWrite(LED_BLUE, LOW);
      sensor_val = 0;
      send_lora_frame();
      delay(2000);
    }
    
    //gLastStatus = gCurrentStatus;  // uncomment this line when using ThingSpeak application  
  //}

}


// void loop for RAK12006 Application
/*
void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
  
  //int sensor_val;
  sensor_val = digitalRead(SENSOR_PIN);

  if(sensor_val == 0)
  {
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    sensor_val = 1; // make it one to indicate there is detection
  }

  if(sensor_val == 1)
  {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    sensor_val = 0; // make it zero to indicate no person
  }
  send_lora_frame();
  delay(5000);
}
*/

// Default void loop
/*
void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
  gCurrentStatus = digitalRead(SENSOR_PIN);
  //if (sensor_val != gLastStatus)
  if(gLastStatus == gCurrentStatus)
  {
    if (gCurrentStatus == 0)
    // "0" for Detected; "1" for Undetected
    {
      digitalWrite(LED_GREEN, HIGH); // Turn ON LED
      digitalWrite(LED_BLUE, HIGH);
      sensor_val = 1; // make it one to indicate there is detection
      send_lora_frame();
      delay(5000);
    }
  
    else if (gCurrentStatus == 1)
    {
      // do nothing
      digitalWrite(LED_GREEN, LOW); // Turn OFF LED
      digitalWrite(LED_BLUE, LOW);
      sensor_val = 0; // make it zero to indicate no person
      send_lora_frame();
      delay(5000);
    }
    
    gLastStatus = gCurrentStatus;  // uncomment this line when using ThingSpeak application  

    //send_lora_frame();
    //delay(5000);
  }
}
*/



/**@brief LoRa function for handling HasJoined event.
 */
void lorawan_has_joined_handler(void)
{
  Serial.println("OTAA Mode, Network Joined!");
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
  m_lora_app_data.buffsize = 0;
  m_lora_app_data.port = gAppPort;
  lmh_send(&m_lora_app_data, g_CurrentConfirm);
}


// default void send_lora_frame
/*
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
  m_lora_app_data.buffer[i++] = 1;
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
*/

// void send_lora_frame for RAK12006 Application
void send_lora_frame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    return;
  }
  
  //int sensor_val;
  uint32_t i = 0;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;
  m_lora_app_data.buffer[i++] = sensor_val;
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
