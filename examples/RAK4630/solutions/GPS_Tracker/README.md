## GPS Tracker over LoRaWAN®
This solution shows how to create an GPS tracker node. After the node joins to server successfully, It will check acceleration of object. Once the value exceeds the threshold value (e.g. twist the board). It will transmits GPS information to a LoRaWan® server. 

### Data Format 

Data sent to LoRaWan® server will be like below:

**9,4546.40891,N,12639.65641,E**




  - First 9 is a marker for the data type, here always 9    
  - 4546.40891 means latitude
  - N means the Northern Hemisphere. Here maybe S, the Southern Hemisphere
  - 12639.65641 means longitude
  - E means the Eastern Hemisphere. Here maybe W, the Western Hemisphere

### Attention
If this examples is implemented for the Region US915, DR0 cannot be used because the package size is too large to fit into the allowed payload.    

### Hardware required
To build this system, the following hardware are required:

- WisBlock Base RAK5005-O * 1pcs   
- WisBlock Core RAK4631 * 1pcs   
- WisBlock Sensor RAK1904 * 1pcs    
- WisBlock Sensor RAK1910 * 1pcs   

![exa-tracker-assy](../../../../assets/repo/exa-tracker-assy.png)

### Software required



- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)    
- SparkFun LIS3DH Arduino library

![lib-lis3dh-install](../../../../assets/Arduino/lib-lis3dh-install.png)

- SX126x-Arduino library

![lib-sx12x-install](../../../../assets/Arduino/lib-sx12x-install.png)

The code for the GPS tracker node is as follows:
```cpp
#include <Arduino.h>
#include <LoRaWan-RAK4630.h>
#include <SPI.h>
#include "SparkFunLIS3DH.h" 
#include "Wire.h"
#include <SoftwareSerial.h>


SoftwareSerial Serial3(15, 16);  //GPS_UART_RX,GPS_UART_TX
LIS3DH SensorTwo( I2C_MODE, 0x18 );



// RAK4630 supply two LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif

bool doOTAA = true;
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60  /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0 /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5 /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3 /**< Number of trials for the join request. */
DeviceClass_t gCurrentClass = CLASS_A; /* class definition*/
lmh_confirm gCurrentConfirm = LMH_CONFIRMED_MSG; /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;   /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
 */
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON , LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

// Foward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t lora_callbacks = {BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
										lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler};


//OTAA keys
uint8_t nodeDeviceEUI[8] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x33, 0x33};
uint8_t nodeAppEUI[8] = {0xB8, 0x27, 0xEB, 0xFF, 0xFE, 0x39, 0x00, 0x00};
uint8_t nodeAppKey[16] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x33, 0x33};


// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64  /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 10000 /**< Defines for user timer, the application data transmission interval. 10s, value in [ms]. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE]; //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = {m_lora_app_data_buffer, 0, 0, 0, 0}; //< Lora user application data structure.

TimerEvent_t appTimer;
static uint32_t timers_init(void);
static uint32_t count = 0;
static uint32_t count_fail = 0;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

  // Initialize LoRa chip.
  lora_rak4630_init();

	// Initialize Serial for debug output
	Serial.begin(115200);
  while(!Serial){delay(10);}
	Serial.println("=====================================");
	Serial.println("Welcome to RAK4630 LoRaWan!!!");
  Serial.println("Type: OTAA");


#if defined(REGION_AS923)
    Serial.println("Region: AS923");
#elif defined(REGION_AU915)
    Serial.println("Region: AU915");
#elif defined(REGION_CN470)
    Serial.println("Region: CN470");
#elif defined(REGION_CN779)
    Serial.println("Region: CN779");
#elif defined(REGION_EU433)
    Serial.println("Region: EU433");
#elif defined(REGION_IN865)
    Serial.println("Region: IN865");
#elif defined(REGION_EU868)
    Serial.println("Region: EU868");
#elif defined(REGION_KR920)
    Serial.println("Region: KR920");
#elif defined(REGION_US915)
    Serial.println("Region: US915");
#elif defined(REGION_US915_HYBRID)
    Serial.println("Region: US915_HYBRID");
#else
    Serial.println("Please define a region in the compiler options.");
#endif
    Serial.println("=====================================");

  //lis3dh init 
  if( SensorTwo.begin() != 0 )
  {
    Serial.println("Problem starting the sensor at 0x18.");
  }
  else
  {
    Serial.println("Sensor at 0x18 started.");
  }
  //gps init 
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);
  pinMode(34,OUTPUT); 
  digitalWrite(34,0);
  delay(1000);
  digitalWrite(34,1);
  delay(1000);
  Serial3.begin(9600);
  while(!Serial3);
  Serial.println("gps uart init ok!");

	//creat a user timer to send data to server period
    uint32_t err_code;

  err_code = timers_init();
	if (err_code != 0)
	{
		Serial.printf("timers_init failed - %d\n", err_code);
	}

	// Setup the EUIs and Keys
	lmh_setDevEui(nodeDeviceEUI);
	lmh_setAppEui(nodeAppEUI);
	lmh_setAppKey(nodeAppKey);

	// Initialize LoRaWan
	err_code = lmh_init(&lora_callbacks, lora_param_init,doOTAA);
	if (err_code != 0)
	{
		Serial.printf("lmh_init failed - %d\n", err_code);
	}

	// Start Join procedure
	lmh_join();
}

void loop()
{
	 // Handle Radio events
	 Radio.IrqProcess();
}

/**@brief LoRa function for handling HasJoined event.
 */
void lorawan_has_joined_handler(void)
{
  Serial.println("OTAA Mode, Network Joined!");

  lmh_error_status ret = lmh_class_request(gCurrentClass);
  if(ret == LMH_SUCCESS)
  {
    delay(1000);
  	TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
  	TimerStart(&appTimer);
  }
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
    lmh_send(&m_lora_app_data, gCurrentConfirm);
}

void send_lora_frame(void)
{
	if (lmh_join_status_get() != LMH_SET)
	{
		//Not joined, try again later
		return;
	}


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

String comdata = "";
/* return 0 ok, 1 fail*/
/*$GPRMC,080655.00,A,4546.40891,N,12639.65641,E,1.045,328.42,170809,,,A*60*/
int parse_gps()
{
  if(comdata.indexOf(",V,")!= -1)
    return 1;

  return 0;
}
/**@brief Function for handling a LoRa tx timer timeout event.
 */
String data="";
void tx_lora_periodic_handler(void)
{
   uint32_t i = 0;
   uint32_t j = 0;
   uint32_t k = 0;
   int res = 1;
   float x = 0;
   float y = 0;
   float z = 0;
   float z1 = 0;
   Serial.println("check acc!");
   x = SensorTwo.readFloatAccelX()*1000;
   y = SensorTwo.readFloatAccelY()*1000;
   z = SensorTwo.readFloatAccelZ()*1000;
   data = "X = "+String(x)+"mg"+" Y = "+String(y)+"mg"+" Z ="+String(z)+"mg";
   Serial.println(data);
   if( abs(x-z) < 400)
   {
       while (Serial3.available() > 0){
       // get the byte data from the GPS
          comdata += char(Serial3.read());
          delay(2);
          if(comdata.indexOf("GPRMC")!= -1 && comdata.indexOf("GPVTG")!= -1)
            {
               break;
            }
       }

       Serial.println(comdata);
       res = parse_gps();
       if(res == 1)
       {
            TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
            TimerStart(&appTimer);
            return;
       }
       delay(1000);
       j = comdata.indexOf(",A,");
       j = j+3;
       if(comdata.indexOf(",E,") != -1)
       {
          k = comdata.indexOf(",E,");
          k = k+1;
       }
       else
       {
          k = comdata.indexOf(",W,");
          k = k+1;
       }

       memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
       m_lora_app_data.port = gAppPort;
       m_lora_app_data.buffer[i++] = 0x09;  
       m_lora_app_data.buffer[i++] = ','; 	   
       while(j!=k+1)
       {
         m_lora_app_data.buffer[i++] = comdata[j];
         j++;
       }
       m_lora_app_data.buffsize = i;
       comdata = "";
       
       send_lora_frame();
   }
   else
   {
      TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
      TimerStart(&appTimer);
   }
}

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
uint32_t timers_init(void)
{
    TimerInit(&appTimer, tx_lora_periodic_handler);
    return 0;
}

```

### Result

```js
11:17:33.167 -> =====================================
11:17:33.167 -> Welcome to RAK4630 LoRaWan!!!
11:17:33.167 -> Type: OTAA
11:17:33.167 -> Region: EU868
11:17:33.167 -> =====================================
11:17:33.167 -> Sensor at 0x18 started.
11:17:35.134 -> gps uart init ok!
11:18:30.529 -> OTAA Mode, Network Joined!
11:18:41.527 -> check acc!
11:18:41.527 -> X = 7.01mg Y = -38.03mg Z =-967.79mg
11:18:51.513 -> check acc!
11:18:51.513 -> X = 5.00mg Y = -35.03mg Z =-942.77mg
11:19:01.534 -> check acc!
11:19:01.534 -> X = 11.01mg Y = -327.27mg Z =-468.38mg
11:19:11.526 -> check acc!
11:19:11.526 -> X = 692.56mg Y = -613.50mg Z =422.34mg
11:19:11.630 -> $GPRMC,080655.00,A,4546.40891,N,12639.65641,E,1.045,328.42,170809,,,A*60
11:19:12.640 -> lmh_send ok count 1
11:19:22.642 -> check acc!
11:19:22.642 -> X = 15.01mg Y = -22.02mg Z =-972.79mg
11:19:32.643 -> check acc!
11:19:32.643 -> X = 16.01mg Y = -23.02mg Z =-968.79mg
11:19:42.669 -> check acc!
11:19:42.669 -> X = 18.01mg Y = -25.02mg Z =-972.79mg
11:19:52.647 -> check acc!
11:19:52.647 -> X = 20.02mg Y = -27.02mg Z =-969.79mg

```

Data arrives at LoRaWAN® server.

![exa-tracker-data](../../../../assets/Examples/exa-tracker-data.png)



### Notes

In this solution, acceleration period is 10s. Because the minimum send interval of LoRaWAN® protocol is limited. To keep success rate, the solution uses 10s.



LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates. LoRaWAN® is a licensed mark.

