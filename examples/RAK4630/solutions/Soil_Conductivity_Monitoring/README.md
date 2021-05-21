## Soil Conductivity Monitoring over LoRaWAN®
[TOC]

### 1. Introduction

![exa-par-solution](../../../../assets/Examples/exa-par-solution.png)

This solution shows how to create an agriculture soil conductivity sensor node. The sensor node measures the soil conductivity, soil temperature and soil humidity from a sensor with a RS485 interface. It then transmits this data frequently over LoRaWan® to a LoRaWan® server.    
At the LoRaWan® server the data arrives in the following format    

| Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 |
| -     | -     | -     | -     | -     | -     | -     |
| 11    | CON1  | CON2  | T1    | T2    | H1    | H2    |
where
  - Byte 1 is a marker for the data type, here always 11 (0x0B) 
  - Conductivity is sent as **`CON1CON2`** e.g. 1009 (would be 0x03 0xF1 in the data package) 
  - Temperature is sent as **`T1.T2`** e.g. 23.50 C
  - Humidity is sent as **`H1.H2`** e.g. 45.91 %

>### Attention
>If this examples is implemented for the Region US915, DR0 cannot be used because the package size is too large to fit into the allowed payload. 

#### 1.1 LoraWAN®

If you want to know how to use the LoRa® communication with WisBlock. You can refer to the following document.

[LoRaWAN® ](/examples/RAK4630/communications/LoRa/LoRaWAN/) 

#### 1.2 Modbus

[Modbus](https://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf) has become a standard communications protocol and is now the most commonly available means of communicating with industrial electronic devices. In this case, the wind speed sensor supports Modbus protocol over RS-485 physical layers.

All Modbus devices include a register map with the location and a description of the data stored in the registers. Modbus functions operate on register map to monitor, configure, and control the device’s inputs and outputs. You have to refer to the register map of your device to gain a better understanding of its operation. Modbus registers are organized into reference types identified by the leading number of the reference address. You can see below an example of how to read and write data in a Modbus device.

![ex-soil-regmap](../../../../assets/Examples/ex-soil-regmap.png)

In our example, we are going to read the Soil Humidity Soil Temperature and Soil Conductivity from our device. We can see in the register map on above, the default Device Address is 1, the humidity value is stored in the register 0002H, the temperature value is stored in the register 0003H, the conductivity value is stored in the register 0015H and is accessible with the function ModbusRTUClient.requestFrom(), and is stored in 16 bits format (2 bytes).

ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x0002, 2)

ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x0003, 2)

ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, 0x0015, 2)

> HOLDING_REGISTERS is 0x03. To get more detail, Please refer to [Modbus protocol specification](https://www.modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf) 
>

If result is 0x0110 0x00FA 0x001C.

0x0110(H) = 272 => Humidity is 27.2% RH

0x00FA(H) = 250 => Temp is 25 ℃

0x001C(H) = 28  => Conductivity is 28 us/cm

### 2. Hardware required

To build this system, the following hardware are required:

WisBlock Base RAK5005-O * 1pcs

WisBlock Core RAK4631 * 1pcs

WisBlock IO RAK5802 * 1pcs

[JXBS-3001-EC-RS-4](http://www.jxiotcity.com/zdcs/zdcs78.html) * 1pcs

>**Note:** To send the data of sensor node to LoRaWan® server,  a  LoRaWan® gateway is also needed. If you don't have, we recommend you use [RAK7243](https://store.rakwireless.com/products/rak7243c-pilot-gateway).

The assembly of sensor node is as follows:

![exa-par-assy](../../../../assets/repo/exa-par-assy.png)

![ex-soil-assy](../../../../assets/Examples/ex-soil-assy.png)



### 3. Software required

- [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)    
- [Arduino RS485 library](https://www.arduino.cc/en/Reference/ArduinoRS485)
- [Arduino Modbus library](https://www.arduino.cc/en/ArduinoModbus/ArduinoModbus)
- [SX126x-Arduino library](https://github.com/beegee-tokyo/SX126x-Arduino/)

#### 3.1 Install library

![lib-manager](../../../../assets/Arduino/lib-manager.png)

![lib-rs485-install](../../../../assets/Arduino/lib-rs485-install.png)

![lib-modbus-install](../../../../assets/Arduino/lib-modbus-install.png)

![lib-sx12x-install](../../../../assets/Arduino/lib-sx12x-install.png)

### 4. Example

The code for the agriculture soil conductivity sensor node can be found in the [sketch file](https://github.com/RAKWireless/WisBlock/blob/master/examples/RAK4630/solutions/Soil_Conductivity_Monitoring/Soil_Conductivity_Monitoring.ino)

<!--
The code for the agriculture soil conductivity sensor node is as follows:
```cpp
#include <Arduino.h>
#include <LoRaWan-RAK4630.h>

#include <ArduinoRS485.h> 
#include <ArduinoModbus.h> 
#include <SPI.h>

#define SOIL_EC_SENSOR



// Check if the board has an LED port defined
#ifndef LED_BUILTIN
#define LED_BUILTIN 35
#endif

#ifndef LED_BUILTIN2
#define LED_BUILTIN2 36
#endif

bool doOTAA = true;
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60  /**< Maximum number of events in the scheduler queue. */

#define LORAWAN_DATERATE DR_0
#define LORAWAN_TX_POWER TX_POWER_0
#define JOINREQ_NBTRIALS 3 /**< Number of trials for the join request. */
DeviceClass_t gCurrentClass = CLASS_A;
lmh_confirm gCurrentConfirm = LMH_CONFIRMED_MSG;
uint8_t gAppPort = LORAWAN_APP_PORT;

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
uint8_t nodeAppKey[16] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77,0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};


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

  if (!ModbusRTUClient.begin(9600)) {
    Serial.println("Failed to start Modbus RTU Client!");
    while (1);
  }

  Scheduler.startLoop(loop2);
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

void loop2()
{
  uint32_t i = 0;
  short raw_conductivity;
  short raw_temperature;
  short raw_humidity;
  
  /* RS485 Power On */
  pinMode(34, OUTPUT); 
  digitalWrite(34, HIGH);
  delay(100);
  /* RS485 Power On */
    
  raw_conductivity = get_soil_conductivity();
  raw_temperature = get_soil_temperature();
  raw_humidity = get_soil_humidity();

  /* RS485 Power Off */
  pinMode(34, OUTPUT); 
  digitalWrite(34, LOW);
  delay(100);
  /* RS485 Power Off */
    
  Serial.printf("-----raw_conductivity = %d-------\n", raw_conductivity);
  Serial.printf("-----raw_temperature = %d-------\n", raw_temperature);
  Serial.printf("-----raw_humidity = %d-------\n", raw_humidity);
  
  m_lora_app_data.port = gAppPort;
  m_lora_app_data.buffer[i++] = 0x08;
  m_lora_app_data.buffer[i++] = (raw_conductivity >> 8) & 0xFF;
  m_lora_app_data.buffer[i++] = raw_conductivity & 0x00FF;
  m_lora_app_data.buffer[i++] = (raw_temperature >> 8) & 0xFF;
  m_lora_app_data.buffer[i++] = raw_temperature & 0x00FF;
  m_lora_app_data.buffer[i++] = (raw_humidity >> 8) & 0xFF;
  m_lora_app_data.buffer[i++] = raw_humidity & 0x00FF;

  m_lora_app_data.buffsize = i;

  delay(5000);
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

static short read_reg(int reg_address)
{
  short reg_value;
  
  if (!ModbusRTUClient.requestFrom(1, HOLDING_REGISTERS, reg_address, 1)) {
    Serial.print("failed to read registers! ");
    Serial.println(ModbusRTUClient.lastError());
  } else {
    // If the request succeeds, the sensor sends the readings, that are
    // stored in the holding registers. The read() method can be used to
    // get the raw humidity temperature values.
    reg_value = ModbusRTUClient.read();
  }
  return reg_value;
}

static short get_soil_conductivity(void)
{
  return read_reg(0x0015);
}

static short get_soil_temperature(void)
{
  return read_reg(0x0003);
}

static short get_soil_humidity(void)
{
  return read_reg(0x0002);
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
```

-->

The logs of sensor node is as follows:

```
=====================================
Welcome to RAK4630 LoRaWan!!!
Type: OTAA
Region: EU868
=====================================
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 267-------
OTAA Mode, Network Joined!
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 270-------
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 272-------
Sending frame now...
lmh_send ok count 1
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 272-------
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 272-------
Sending frame now...
lmh_send ok count 2
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 270-------
-----raw_conductivity = 28-------
-----raw_temperature = 250-------
-----raw_humidity = 272-------
Sending frame now...
lmh_send ok count 3
```

The logs of LoraWan® Server is as follows:

![exa-soil-data](../../../../assets/Examples/exa-soil-data.png)

LoRa® is a registered trademark or service mark of Semtech Corporation or its affiliates. LoRaWAN® is a licensed mark.