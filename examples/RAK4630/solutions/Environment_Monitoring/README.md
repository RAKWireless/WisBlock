## Environment Monitoring over LoRaWAN®
This solution shows how to create an environmental data sensor node. The sensor node measures air temperature, air humidity, barometric pressure and gas levels. After the node joins to server successfully, It then transmits this data periodically to a LoRaWan® server. 

### Data Format 

| Byte1 | Byte2        | Byte3        | Byte4     | Byte5     |
| ----- | ------------ | ------------ | --------- | --------- |
| 1     | Temperature1 | Temperature2 | Humidity1 | Humidity2 |

| Byte6       | Byte7       | Byte8       | Byte9       | Byte10 | Byte11 | Byte12 | Byte13 |
| ----------- | ----------- | ----------- | ----------- | ------ | ------ | ------ | ------ |
| Barometric1 | Barometric2 | Barometric3 | Barometric4 | Gas1   | Gas2   | Gas3   | Gas4   |


  - Byte 1 is a marker for the data type, here always 1    
  - Every value is MSB first
  - Temperature is sent as two bytes, enlarged 100 times for accuracy. For example, 2510 means 25.01 C, sent as  0x09 0xCE
  - Humidity is sent as two bytes, enlarged 100 times for accuracy. For example, 4173 means 41.73 %, sent as  0x10 0x4D
  - Barometric pressure is sent as four bytes, enlarged 100 times for accuracy. For example, 100945 means 1009.45 hPa, sent as 0x00 0x01 0x8A 0x51
  - Gas resistance is sent as as four bytes,  For example, 63560 Ohm, sent as 0x00 0x00 0xF8 0x48

### Attention
If this examples is implemented for the Region US915, DR0 cannot be used because the package size is too large to fit into the allowed payload.    

### Hardware required
To build this system, the following hardware are required:

- WisBlock Base RAK5005-O * 1pcs   
- WisBlock Core RAK4631 * 1pcs   
- WisBlock Sensor RAK1906 * 1pcs    

![exa-rak1906-assy](../../../../assets/repo/exa-rak1906-assy.png)

### Software required



- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)    
- ClosedCube BME680 library

![lib-bme680-install](../../../../assets/Arduino/lib-bme680-install.png)

- SX126x-Arduino library

![lib-sx12x-install](../../../../assets/Arduino/lib-sx12x-install.png)

The code for the environment sensor node can be found in the [sketch file](https://github.com/RAKWireless/WisBlock/blob/master/examples/RAK4630/solutions/Environment_Monitoring/Environment_Monitoring.ino)

<!--
The code for the environment sensor node is as follows:
```cpp
#include <Arduino.h>
#include <LoRaWan-RAK4630.h> //http://librarymanager/ALL#SX126x-Arduino
#include <SPI.h>

#include <Wire.h>
#include "ClosedCube_BME680.h" //http://librarymanager/ALL#ClosedCube_BME680_Arduino
#include <U8g2lib.h>       //http://librarymanager/ALL#u8g2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
ClosedCube_BME680 bme680;

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
static lmh_param_t lora_param_init = {LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF};

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
uint8_t nodeAppKey[16] = {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22};

// Private defination
#define LORAWAN_APP_DATA_BUFF_SIZE 64  /**< buffer size of the data to be transmitted. */
#define LORAWAN_APP_INTERVAL 20000 /**< Defines for user timer, the application data transmission interval. 20s, value in [ms]. */
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

  /* bme680 init */
  bme680_init();

  u8g2.begin();

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
	err_code = lmh_init(&lora_callbacks, lora_param_init, doOTAA);
	if (err_code != 0)
	{
		Serial.printf("lmh_init failed - %d\n", err_code);
	}

	// Start Join procedure
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

  u8g2.drawStr(20, 39, "Joining ...");
  u8g2.sendBuffer(); // transfer internal memory to the display 
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
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

  u8g2.drawStr(20, 39, "Joined");
  u8g2.sendBuffer(); // transfer internal memory to the display
  //delay(2000);

  lmh_error_status ret = lmh_class_request(gCurrentClass);
	if (ret == LMH_SUCCESS)
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
  bme680_get();

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
}

/**@brief Function for handling user timerout event.
 */
void tx_lora_periodic_handler(void)
{
   TimerSetValue(&appTimer, LORAWAN_APP_INTERVAL);
   TimerStart(&appTimer);
   Serial.println("Sending frame now...");
   send_lora_frame();
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

void bme680_init()
{
  Wire.begin();
  bme680.init(0x76); // I2C address: 0x76 or 0x77
  bme680.reset();

  Serial.print("Chip ID=0x");
  Serial.println(bme680.getChipID(), HEX);

  // oversampling: humidity = x1, temperature = x2, pressure = x16
  bme680.setOversampling(BME680_OVERSAMPLING_X1, BME680_OVERSAMPLING_X2, BME680_OVERSAMPLING_X16);
  bme680.setIIRFilter(BME680_FILTER_3);
  bme680.setGasOn(300, 100); // 300 degree Celsius and 100 milliseconds 

  bme680.setForcedMode();
}
String data = "";
void bme680_get()
{
  char oled_data[32] = {0};
    Serial.print("result: ");
    uint32_t i = 0;
    memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
    m_lora_app_data.port = gAppPort;

    double temp = bme680.readTemperature();
    double pres = bme680.readPressure();
    double hum = bme680.readHumidity();
      
    uint32_t gas = bme680.readGasResistance();

	data = "Tem:" + String(temp) + "C " + "Hum:" + String(hum) + "% " + "Pres:" + String(pres) + "KPa " + "Gas:" + String(gas) + "Ohms";
    Serial.println(data);

  // display bme680 sensor data on OLED
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

  memset(oled_data, 0, sizeof(oled_data));
  sprintf(oled_data, "T=%.2fC", temp);
  u8g2.drawStr(3, 15, oled_data);

  memset(oled_data, 0, sizeof(oled_data));
  snprintf(oled_data, 64, "RH=%.2f%%", hum);
  u8g2.drawStr(3, 30, oled_data);

  memset(oled_data, 0, sizeof(oled_data));
  sprintf(oled_data, "P=%.2fhPa", pres);
  u8g2.drawStr(3, 45, oled_data);

  memset(oled_data, 0, sizeof(oled_data));
  sprintf(oled_data, "G=%dOhms", gas);
  u8g2.drawStr(3, 60, oled_data);

  u8g2.sendBuffer(); // transfer internal memory to the display 
    
	uint16_t t = temp * 100;
	uint16_t h = hum * 100;
	uint32_t pre = pres * 100;
    
    //result: T=28.25C, RH=50.00%, P=958.57hPa, G=100406 Ohms
    m_lora_app_data.buffer[i++] = 0x01;
	m_lora_app_data.buffer[i++] = (uint8_t)(t >> 8);
    m_lora_app_data.buffer[i++] = (uint8_t)t;
	m_lora_app_data.buffer[i++] = (uint8_t)(h >> 8);
    m_lora_app_data.buffer[i++] = (uint8_t)h;
	m_lora_app_data.buffer[i++] = (uint8_t)((pre & 0xFF000000) >> 24);
	m_lora_app_data.buffer[i++] = (uint8_t)((pre & 0x00FF0000) >> 16);
	m_lora_app_data.buffer[i++] = (uint8_t)((pre & 0x0000FF00) >> 8);
    m_lora_app_data.buffer[i++] = (uint8_t)(pre & 0x000000FF);
	m_lora_app_data.buffer[i++] = (uint8_t)((gas & 0xFF000000) >> 24);
	m_lora_app_data.buffer[i++] = (uint8_t)((gas & 0x00FF0000) >> 16);
	m_lora_app_data.buffer[i++] = (uint8_t)((gas & 0x0000FF00) >> 8);
    m_lora_app_data.buffer[i++] = (uint8_t)(gas & 0x000000FF);
    m_lora_app_data.buffsize = i;
    bme680.setForcedMode();
}
```
-->
### Result

```js
10:30:07.777 -> =====================================
10:30:07.777 -> Welcome to RAK4630 LoRaWan!!!
10:30:07.777 -> Type: OTAA
10:30:07.777 -> Region: EU868
10:30:07.777 -> =====================================
10:30:07.777 -> Chip ID=0x61
10:30:12.974 -> OTAA Mode, Network Joined!
10:30:33.999 -> Sending frame now...
10:30:33.999 -> result: Tem:28.70C Hum:67.00% Pres:948.50KPa Gas:27646Ohms
10:30:33.999 -> lmh_send ok count 1
10:30:53.979 -> Sending frame now...
10:30:53.979 -> result: Tem:28.70C Hum:67.00% Pres:948.50KPa Gas:242488Ohms
10:30:54.012 -> lmh_send ok count 2
10:31:14.006 -> Sending frame now...
10:31:14.006 -> result: Tem:28.70C Hum:67.00% Pres:948.50KPa Gas:457174Ohms
10:31:14.006 -> lmh_send ok count 3
10:31:33.997 -> Sending frame now...
10:31:33.997 -> result: Tem:28.69C Hum:67.00% Pres:948.50KPa Gas:74102Ohms
10:31:34.032 -> lmh_send ok count 4

```

Data arrives at LoRaWAN® server.

![exa-env-mon-data](../../../../assets/Examples/exa-env-mon-data.png)





LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates. LoRaWAN® is a licensed mark.

