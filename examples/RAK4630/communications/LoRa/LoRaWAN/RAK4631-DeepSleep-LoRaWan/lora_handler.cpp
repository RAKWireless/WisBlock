/**
   @file lora_handler.cpp
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief Initialization, event handlers and task for LoRaWan
   @version 0.1
   @date 2020-08-15

   @copyright Copyright (c) 2020

*/
#include "main.h"

/** DIO1 GPIO pin for RAK4631 */
#define PIN_LORA_DIO_1 47

/** Max size of the data to be transmitted. */
#define LORAWAN_APP_DATA_BUFF_SIZE 64
/** Number of trials for the join request. */
#define JOINREQ_NBTRIALS 8

/** Lora application data buffer. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];
/** Lora application data structure. */
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0};

// LoRaWan event handlers
/** LoRaWan callback when join network finished */
static void lorawan_has_joined_handler(void);
/** LoRaWan callback when join failed */
static void lorawan_join_failed_handler(void);
/** LoRaWan callback when data arrived */
static void lorawan_rx_handler(lmh_app_data_t *app_data);
/** LoRaWan callback after class change request finished */
static void lorawan_confirm_class_handler(DeviceClass_t Class);
/** LoRaWan Function to send a package */
bool sendLoRaFrame(void);

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()

   Set structure members to
   LORAWAN_ADR_ON or LORAWAN_ADR_OFF to enable or disable adaptive data rate
   LORAWAN_DEFAULT_DATARATE OR DR_0 ... DR_5 for default data rate or specific data rate selection
   LORAWAN_PUBLIC_NETWORK or LORAWAN_PRIVATE_NETWORK to select the use of a public or private network
   JOINREQ_NBTRIALS or a specific number to set the number of trials to join the network
   LORAWAN_DEFAULT_TX_POWER or a specific number to set the TX power used
   LORAWAN_DUTYCYCLE_ON or LORAWAN_DUTYCYCLE_OFF to enable or disable duty cycles
                     Please note that ETSI mandates duty cycled transmissions.
*/
static lmh_param_t lora_param_init = {LORAWAN_ADR_OFF, DR_3, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_DEFAULT_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

/** Structure containing LoRaWan callback functions, needed for lmh_init() */
static lmh_callback_t lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                        lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler
                                       };
//  !!!! KEYS ARE MSB !!!!
/** Device EUI required for OTAA network join */
uint8_t nodeDeviceEUI[8] = {0x00, 0x0D, 0x75, 0xE6, 0x56, 0x4D, 0xC1, 0xF6};
/** Application EUI required for network join */
uint8_t nodeAppEUI[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1};
/** Application key required for network join */
uint8_t nodeAppKey[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};
/** Device address required for ABP network join */
uint32_t nodeDevAddr = 0x26021FB6;
/** Network session key required for ABP network join */
uint8_t nodeNwsKey[16] = {0x32, 0x3D, 0x15, 0x5A, 0x00, 0x0D, 0xF3, 0x35, 0x30, 0x7A, 0x16, 0xDA, 0x0C, 0x9D, 0xF5, 0x3F};
/** Application session key required for ABP network join */
uint8_t nodeAppsKey[16] = {0x3F, 0x6A, 0x66, 0x45, 0x9D, 0x5E, 0xDC, 0xA6, 0x3C, 0xBC, 0x46, 0x19, 0xCD, 0x61, 0xA1, 0x1E};

/** Flag whether to use OTAA or ABP network join method */
bool doOTAA = true;

DeviceClass_t gCurrentClass = CLASS_A;                  /* class definition*/
LoRaMacRegion_t gCurrentRegion = LORAMAC_REGION_EU868;  /* Region:EU868*/

/**
   @brief Initialize LoRa HW and LoRaWan MAC layer

   @return int8_t result
    0 => OK
   -1 => SX126x HW init failure
   -2 => LoRaWan MAC initialization failure
   -3 => Subband selection failure
*/
int8_t initLoRaWan(void)
{
  // Initialize LoRa chip.
  if (lora_rak4630_init() != 0)
  {
    return -1;
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
  if (lmh_init(&lora_callbacks, lora_param_init, doOTAA, gCurrentClass, gCurrentRegion) != 0)
  {
    return -2;
  }

  // For some regions we might need to define the sub band the gateway is listening to
  // This must be called AFTER lmh_init()
  if (!lmh_setSubBandChannels(1))
  {
    return -3;
  }

  // Start Join procedure
#ifndef MAX_SAVE
  Serial.println("Start network join request");
#endif
  lmh_join();

  return 0;
}

/**
   @brief LoRa function for handling HasJoined event.
*/
static void lorawan_has_joined_handler(void)
{
  if (doOTAA)
  {
    uint32_t otaaDevAddr = lmh_getDevAddr();
#ifndef MAX_SAVE
    Serial.printf("OTAA joined and got dev address %08X\n", otaaDevAddr);
#endif
  }
  else
  {
#ifndef MAX_SAVE
    Serial.println("ABP joined");
#endif
  }

  // Default is Class A, where the SX1262 transceiver is in sleep mode unless a package is sent
  // If switched to Class C the power consumption is higher because the SX1262 chip remains in RX mode

  // lmh_class_request(CLASS_C);

  digitalWrite(LED_CONN, LOW);
  
  // Now we are connected, start the timer that will wakeup the loop frequently
  taskWakeupTimer.begin(SLEEP_TIME, periodicWakeup);
  taskWakeupTimer.start();
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

   @param app_data  Pointer to rx data
*/
static void lorawan_rx_handler(lmh_app_data_t *app_data)
{
#ifndef MAX_SAVE
  Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d\n",
                app_data->port, app_data->buffsize, app_data->rssi, app_data->snr);
#endif
  switch (app_data->port)
  {
    case 3:
      // Port 3 switches the class
      if (app_data->buffsize == 1)
      {
        switch (app_data->buffer[0])
        {
          case 0:
            lmh_class_request(CLASS_A);
#ifndef MAX_SAVE
            Serial.println("Request to switch to class A");
#endif
            break;

          case 1:
            lmh_class_request(CLASS_B);
#ifndef MAX_SAVE
            Serial.println("Request to switch to class B");
#endif
            break;

          case 2:
            lmh_class_request(CLASS_C);
#ifndef MAX_SAVE
            Serial.println("Request to switch to class C");
#endif
            break;

          default:
            break;
        }
      }
      break;
    case LORAWAN_APP_PORT:
      // Copy the data into loop data buffer
      memcpy(rcvdLoRaData, app_data->buffer, app_data->buffsize);
      rcvdDataLen = app_data->buffsize;
      eventType = 0;
      // Notify task about the event
      if (taskEvent != NULL)
      {
#ifndef MAX_SAVE
        Serial.println("Waking up loop task");
#endif
        xSemaphoreGive(taskEvent);
      }
  }
}

/**
   @brief Callback for class switch confirmation

   @param Class The new class
*/
static void lorawan_confirm_class_handler(DeviceClass_t Class)
{
#ifndef MAX_SAVE
  Serial.printf("switch to class %c done\n", "ABC"[Class]);
#endif

  // Informs the server that switch has occurred ASAP
  m_lora_app_data.buffsize = 0;
  m_lora_app_data.port = LORAWAN_APP_PORT;
  lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG);
}

/**
   @brief Send a LoRaWan package

   @return result of send request
*/
bool sendLoRaFrame(void)
{
  if (lmh_join_status_get() != LMH_SET)
  {
    //Not joined, try again later
#ifndef MAX_SAVE
    Serial.println("Did not join network, skip sending frame");
#endif
    return false;
  }

  m_lora_app_data.port = LORAWAN_APP_PORT;

  //******************************************************************
  /// \todo here some more usefull data should be put into the package
  //******************************************************************

  uint8_t buffSize = 0;
  m_lora_app_data_buffer[buffSize++] = 'H';
  m_lora_app_data_buffer[buffSize++] = 'e';
  m_lora_app_data_buffer[buffSize++] = 'l';
  m_lora_app_data_buffer[buffSize++] = 'l';
  m_lora_app_data_buffer[buffSize++] = 'o';

  m_lora_app_data.buffsize = buffSize;

  lmh_error_status error = lmh_send(&m_lora_app_data, LMH_UNCONFIRMED_MSG);

  return (error == 0);
}
