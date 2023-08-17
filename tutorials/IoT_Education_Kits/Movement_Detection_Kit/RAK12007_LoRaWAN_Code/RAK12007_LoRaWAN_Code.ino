#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //http://librarymanager/All#SX126x
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>   //Click here to get the library: http://librarymanager/All#Adafruit_GFX
#include <Adafruit_SSD1306.h>   //Click here to get the library: http://librarymanager/All#Adafruit_SSD1306

#ifdef RAK4630
  #define BOARD "RAK4631 "
  #define  RAK4631_BOARD true  
#else    
  #define  RAK4631_BOARD false             
#endif

// Section to add include libraries, declarations and variables.

#define Enable WB_IO2
#define TRIG WB_IO6
#define ECHO WB_IO4
#define PD   WB_IO5   //power done control （=1 power done，=0 power on） 

#define TIME_OUT  24125 //max measure distance is 4m,the velocity of sound is 331.6m/s in 0℃,TIME_OUT=4*2/331.6*1000000=24215us

float ratio = 346.6/1000/2;   //velocity of sound =331.6+0.6*25℃(m/s),(Indoor temperature about 25℃)

long int duration_time();  //measure high level time

long int duration, mm;



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
uint8_t nodeAppKey[16] = {0x8B, 0x0E, 0x96, 0x86, 0xE8, 0x4E, 0xA3, 0xB5, 0x31, 0x42, 0xB3, 0x65, 0x75, 0xB1, 0xD3, 0xED};

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
  pinMode(Enable, OUTPUT);
  digitalWrite(Enable, HIGH);
  // pinMode(WB_IO4, INPUT);        // IR Sensor as an INPUT
  pinMode(ECHO, INPUT);   // Echo Pin of Ultrasonic Sensor is an Input
  pinMode(TRIG, OUTPUT);  // Trigger Pin of Ultrasonic Sensor is an Output
  pinMode(PD, OUTPUT);    // power done control pin is an Output 
  digitalWrite(TRIG,LOW);
  digitalWrite(PD,LOW);

  pinMode(LED_GREEN, OUTPUT);  // Output LED
  pinMode(LED_BLUE, OUTPUT);   // Output LED

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
   Serial.println("  RAK12007 LoRaWAN test ");
   Serial.println("========================");

}

void loop()
{
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
  

  // long int duration, mm;
   digitalWrite(LED_BLUE,HIGH);
   digitalWrite(LED_GREEN, HIGH);
   duration = duration_time();
   if(duration > 0)
   {
     mm = duration*ratio; //Test distance = (high level time×velocity of sound (340M/S) / 2,
     digitalWrite(LED_BLUE,LOW);
     digitalWrite(LED_GREEN, LOW);  
     Serial.println("Sending frame now...");
     send_lora_frame();   
     Serial.print(mm);
     Serial.print("mm");
     Serial.println();  
     delay(15000); 
   }
   else
   {
     Serial.println("Out of range");     
   } 
  delay(50);

}

long int duration_time()
{
   long int respondTime;
   pinMode(TRIG, OUTPUT);
   digitalWrite(TRIG, HIGH);
   delayMicroseconds(12);   //pull high time need over 10us 
   digitalWrite(TRIG, LOW);  
   pinMode(ECHO, INPUT);
   respondTime = pulseIn(ECHO, HIGH); // microseconds 
   delay(33);
   if(RAK4631_BOARD)
   {
     respondTime = respondTime*0.7726; // Time calibration factor is 0.7726,to get real time microseconds for 4631board
   }
   Serial.printf("respond time is %d\r\n",respondTime);

   if((respondTime>0)&&(respondTime < TIME_OUT))  //ECHO pin max timeout is 33000us according it's datasheet 
   {
    return respondTime;
   }
   else
   {
     return -1;  
   }   
}

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

String data = "";

void send_lora_frame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    return;
  }
  
  Serial.print("result: ");
  uint32_t i = 0;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;

  data = "Distance:" + String(mm) + "mm";
  Serial.println(data);

  uint32_t mm_data = mm;

  m_lora_app_data.buffer[i++] = 0x0A;
  m_lora_app_data.buffer[i++] = (uint8_t)((mm_data & 0xFF000000) >> 24);
  m_lora_app_data.buffer[i++] = (uint8_t)((mm_data & 0x00FF0000) >> 16);
  m_lora_app_data.buffer[i++] = (uint8_t)((mm_data & 0x0000FF00) >> 8);
  m_lora_app_data.buffer[i++] = (uint8_t)(mm_data & 0x000000FF);
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
