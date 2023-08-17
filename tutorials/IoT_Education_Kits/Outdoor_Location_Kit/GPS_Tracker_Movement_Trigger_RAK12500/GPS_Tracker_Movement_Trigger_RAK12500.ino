/**
   @file GPS_Tracker.ino
   @author rakwireless.com
   @brief This sketch demonstrate a GPS tracker that collect location from a uBlox M7 GNSS sensor
      and send the data to lora gateway.
      It uses a 3-axis acceleration sensor to detect movement of the tracker
   @version 0.2
   @date 2021-04-30

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
#include <LoRaWan-RAK4630.h> //http://librarymanager/All#SX126x
#include <SPI.h>
#include "SparkFunLIS3DH.h" //http://librarymanager/All#SparkFun-LIS3DH
#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
#include "Wire.h"

#define INT1_PIN WB_IO5 // Set on Slot D to support all wisblock base.

LIS3DH acc_sensor(I2C_MODE, 0x18);

SFE_UBLOX_GNSS g_myGNSS;

String tmp_data = "";
int direction_S_N = 0;  //0--S, 1--N
int direction_E_W = 0;  //0--E, 1--W

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
DeviceClass_t gCurrentClass = CLASS_A;          /* class definition*/
LoRaMacRegion_t gCurrentRegion = LORAMAC_REGION_US915;    /* Region:EU868*/
lmh_confirm gCurrentConfirm = LMH_UNCONFIRMED_MSG;          /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;                      /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                        lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler
                                       };

//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x06, 0x03, 0xFB};
uint8_t nodeAppEUI[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0xFC, 0x07, 0x2B, 0x4B, 0x6A, 0xA7, 0xE5, 0x75, 0x89, 0x2A, 0x95, 0x80, 0x3F, 0xCB, 0xD3, 0xE3};

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = {0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23};
uint8_t nodeAppsKey[16] = {0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C};

// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64                     /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 10000                        /**< Defines for user timer, the application data transmission interval. 10s, value in [ms]. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];        //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

TimerEvent_t appTimer;
static uint32_t timers_init(void);
static uint32_t count = 0;
static uint32_t count_fail = 0;
volatile uint8_t tracker_trig = 0;
String data = "";
bool send_always;

void acc_int_callback(void)
{
  if (tracker_trig == 0)
  {
    Serial.println("Tracker Moved!");
    tracker_trig = 1;
    send_lora_frame();
  }
}

/**
   @brief Clear ACC interrupt register to enable next wakeup

*/
void clear_acc_int(void)
{
  uint8_t data_read;
  tracker_trig = 0;
  acc_sensor.readRegister(&data_read, LIS3DH_INT1_SRC);
}

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

  // Initialize LoRa chip.
  lora_rak4630_init();

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

  switch (gCurrentRegion)
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

  pinMode(INT1_PIN, INPUT);

  Wire.begin();

  acc_sensor.settings.accelSampleRate = 10; //Hz.  Can be: 0,1,10,25,50,100,200,400,1600,5000 Hz
  acc_sensor.settings.accelRange = 2;     //Max G force readable.  Can be: 2, 4, 8, 16

  acc_sensor.settings.adcEnabled = 0;
  acc_sensor.settings.tempEnabled = 0;
  acc_sensor.settings.xAccelEnabled = 1;
  acc_sensor.settings.yAccelEnabled = 1;
  acc_sensor.settings.zAccelEnabled = 1;

  //lis3dh init
  if (acc_sensor.begin() != 0)
  {
    Serial.println("Problem starting the sensor at 0x18.");
  }
  else
  {
    uint8_t data_to_write = 0;
    // Enable interrupts
    data_to_write |= 0x20;                    //Z high
    data_to_write |= 0x08;                    //Y high
    data_to_write |= 0x02;                    //X high
    acc_sensor.writeRegister(LIS3DH_INT1_CFG, data_to_write); // Enable interrupts on high tresholds for x, y and z

    // Set interrupt trigger range
    data_to_write = 0;

    data_to_write |= 0x10; // 1/8 range

    acc_sensor.writeRegister(LIS3DH_INT1_THS, data_to_write); // 1/8th range

    // Set interrupt signal length
    data_to_write = 0;
    data_to_write |= 0x01; // 1 * 1/50 s = 20ms
    acc_sensor.writeRegister(LIS3DH_INT1_DURATION, data_to_write);

    acc_sensor.readRegister(&data_to_write, LIS3DH_CTRL_REG5);
    data_to_write &= 0xF3;                     //Clear bits of interest
    data_to_write |= 0x08;                     //Latch interrupt (Cleared by reading int1_src)
    acc_sensor.writeRegister(LIS3DH_CTRL_REG5, data_to_write); // Set interrupt to latching

    // Select interrupt pin 1
    data_to_write = 0;
    data_to_write |= 0x40; //AOI1 event (Generator 1 interrupt on pin 1)
    data_to_write |= 0x20; //AOI2 event ()
    acc_sensor.writeRegister(LIS3DH_CTRL_REG3, data_to_write);

    // No interrupt on pin 2
    acc_sensor.writeRegister(LIS3DH_CTRL_REG6, 0x00);

    // Enable high pass filter
    acc_sensor.writeRegister(LIS3DH_CTRL_REG2, 0x01);

    // Set low power mode
    data_to_write = 0;
    acc_sensor.readRegister(&data_to_write, LIS3DH_CTRL_REG1);
    data_to_write |= 0x08;
    acc_sensor.writeRegister(LIS3DH_CTRL_REG1, data_to_write);
    delay(100);
    data_to_write = 0;
    acc_sensor.readRegister(&data_to_write, 0x1E);
    data_to_write |= 0x90;
    acc_sensor.writeRegister(0x1E, data_to_write);
    delay(100);

    clear_acc_int();

    // Set the interrupt callback function
    attachInterrupt(INT1_PIN, acc_int_callback, RISING);
  }
  //gps init

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 0);
  delay(1000);
  digitalWrite(WB_IO2, 1);
  delay(1000);

  int baud[7] = {9600, 14400, 19200, 38400, 56000, 57600, 115200};

  for (int i = 0 ; i < sizeof(baud) / sizeof(int) ; i++)
  {
    Serial1.begin(baud[i]);
    while (!Serial1);      // Wait for user to open terminal
    if (g_myGNSS.begin(Serial1) == true)
    {
      Serial.printf("GNSS baund rate: %d \n", baud[i]); //GNSS baund rate
      break;
    }
    Serial1.end();
    delay(200);
  }
  Serial.println("GNSS serial connected");

  g_myGNSS.setUART1Output(COM_TYPE_UBX);      //Set the UART port to output UBX only
  g_myGNSS.setI2COutput(COM_TYPE_UBX);        //Set the I2C port to output UBX only (turn off NMEA noise)
  g_myGNSS.saveConfiguration();               //Save the current settings to flash and BBR

  //creat a user timer to send data to server period
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
  err_code = lmh_init(&lora_callbacks, lora_param_init, doOTAA, gCurrentClass, gCurrentRegion);
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
  send_always = 1;                    // Make this value 1 if you want continuous transmission and 0 if only when triggered.
  float x = 0;
  float y = 0;
  float z = 0;

  Serial.println("\ncheck acc!");
  x = acc_sensor.readFloatAccelX() * 1000;
  y = acc_sensor.readFloatAccelY() * 1000;
  z = acc_sensor.readFloatAccelZ() * 1000;
  data = "X = " + String(x) + "mg" + " Y = " + String(y) + "mg" + " Z =" + String(z) + "mg";
  Serial.println(data);
  data = "";

  //  long latitude;
  //  long longitude;
  uint32_t latitude = 10000000;
  uint32_t longitude = 10000000;
  
  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    latitude = g_myGNSS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);

    longitude = g_myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));
  }

  //  float flat, flon;
  //  int32_t ilat, ilon;

  //    unsigned long age;
  //    gps.f_get_position(&flat, &flon, &age);
  //    flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat;
  //    ilat = flat * 100000;
  //    flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon;
  //    ilon = flon * 100000;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;
  m_lora_app_data.buffer[0] = 0x09;
  //lat data
  m_lora_app_data.buffer[1] = (latitude & 0xFF000000) >> 24;
  m_lora_app_data.buffer[2] = (latitude & 0x00FF0000) >> 16;
  m_lora_app_data.buffer[3] = (latitude & 0x0000FF00) >> 8;
  m_lora_app_data.buffer[4] =  latitude & 0x000000FF;
  if (direction_S_N == 0)
  {
    m_lora_app_data.buffer[5] = 'S';
  }
  else
  {
    m_lora_app_data.buffer[5] = 'N';
  }
  //lon data
  m_lora_app_data.buffer[6] = (longitude & 0xFF000000) >> 24;
  m_lora_app_data.buffer[7] = (longitude & 0x00FF0000) >> 16;
  m_lora_app_data.buffer[8] = (longitude & 0x0000FF00) >> 8;
  m_lora_app_data.buffer[9] =  longitude & 0x000000FF;
  if (direction_E_W == 0)
  {
    m_lora_app_data.buffer[10] = 'E';
  }
  else
  {
    m_lora_app_data.buffer[10] = 'W';
  }
  m_lora_app_data.buffsize = 11;

  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.
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

  lmh_error_status ret = lmh_class_request(gCurrentClass);
  if (ret == LMH_SUCCESS)
  {
    delay(1000);
    TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
    TimerStart(&appTimer);
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
  lmh_send(&m_lora_app_data, gCurrentConfirm);
}

void send_lora_frame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
    return;
  }

  if (tracker_trig == 1 || send_always == 1)
  {
    lmh_error_status error = lmh_send(&m_lora_app_data, gCurrentConfirm);
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
    TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
    TimerStart(&appTimer);
  }
  else
  {
    Serial.printf("Not sending!", count_fail);
    TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
    TimerStart(&appTimer);
  }
}

/**@brief Function for analytical direction.
*/
void direction_parse(String tmp)
{
  if (tmp.indexOf(",E,") != -1)
  {
    direction_E_W = 0;
  }
  else
  {
    direction_E_W = 1;
  }

  if (tmp.indexOf(",S,") != -1)
  {
    direction_S_N = 0;
  }
  else
  {
    direction_S_N = 1;
  }
}

/**@brief Function for handling a LoRa tx timer timeout event.
*/

void tx_lora_periodic_handler(void)
{
  if (send_always == 1)
  {
    send_lora_frame();
  }
  else
  {
    Serial.println("No Location Found");
    TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
    TimerStart(&appTimer);
  }
  tracker_trig = 0;
  send_always = 0;
  clear_acc_int();
}

/**@brief Function for the Timer initialization.

   @details Initializes the timer module. This creates and starts application timers.
*/
uint32_t timers_init(void)
{
  TimerInit(&appTimer, tx_lora_periodic_handler);
  return 0;
}
