#include <Arduino.h>
#include <LoRaWan-RAK4630.h>  //http://librarymanager/All#SX126x
#include <SPI.h>
#include <Wire.h>

#include "UVlight_LTR390.h"                // http://librarymanager/All#RAK12019_LTR390
UVlight_LTR390 ltr = UVlight_LTR390();

#include "Light_VEML7700.h"                // http://librarymanager/All#Light_veml7700
Light_VEML7700 VMEL = Light_VEML7700();

uint16_t uvi;
uint32_t uvs;
uint32_t lux;
uint32_t white;
uint32_t raw_als;

#include <Adafruit_MCP23X17.h>  //http://librarymanager/All#Adafruit_MCP23017
Adafruit_MCP23X17 mcp;

// Added definitions for the functionality of RAK14003
const uint8_t LED_0 = 0;      // PA0 (17) of MCP23017
const uint8_t LED_1 = 1;      // PA1 (18) of MCP23017
const uint8_t LED_2 = 2;      // PA2 (19) of MCP23017
const uint8_t LED_3 = 3;      // PA3 (20) of MCP23017
const uint8_t LED_4 = 4;      // PA4 (21) of MCP23017
const uint8_t LED_5 = 5;      // PA5 (22) of MCP23017
const uint8_t LED_6 = 6;      // PA6 (23) of MCP23017
const uint8_t LED_7 = 7;      // PA7 (24) of MCP23017
const uint8_t LED_8 = 8;      // PB0 (25) of MCP23017
const uint8_t LED_9 = 9;      // PB1 (26) of MCP23017

#ifdef RAK4630
#define BOARD "RAK4631 "
#define RAK4631_BOARD true
#else
#define RAK4631_BOARD false
#endif

bool doOTAA = true;                                               // OTAA is used by default.
#define SCHED_MAX_EVENT_DATA_SIZE APP_TIMER_SCHED_EVENT_DATA_SIZE /**< Maximum size of scheduler events. */
#define SCHED_QUEUE_SIZE 60                                       /**< Maximum number of events in the scheduler queue. */
#define LORAWAN_DATERATE DR_0                                     /*LoRaMac datarates definition, from DR_0 to DR_5*/
#define LORAWAN_TX_POWER TX_POWER_5                               /*LoRaMac tx power definition, from TX_POWER_0 to TX_POWER_15*/
#define JOINREQ_NBTRIALS 3                                        /**< Number of trials for the join request. */
DeviceClass_t g_CurrentClass = CLASS_A;                           /* class definition*/
LoRaMacRegion_t g_CurrentRegion = LORAMAC_REGION_US915;           /* Region:EU868*/
lmh_confirm g_CurrentConfirm = LMH_CONFIRMED_MSG;                 /* confirm/unconfirm packet definition*/
uint8_t gAppPort = LORAWAN_APP_PORT;                              /* data port*/

/**@brief Structure containing LoRaWan parameters, needed for lmh_init()
*/
static lmh_param_t g_lora_param_init = { LORAWAN_ADR_ON, LORAWAN_DATERATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF };

// Forward declaration
static void lorawan_has_joined_handler(void);
static void lorawan_join_failed_handler(void);
static void lorawan_rx_handler(lmh_app_data_t *app_data);
static void lorawan_confirm_class_handler(DeviceClass_t Class);
static void send_lora_frame(void);

/**@brief Structure containing LoRaWan callback functions, needed for lmh_init()
*/
static lmh_callback_t g_lora_callbacks = { BoardGetBatteryLevel, BoardGetUniqueId, BoardGetRandomSeed,
                                           lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, lorawan_join_failed_handler };
//OTAA keys !!!! KEYS ARE MSB !!!!
uint8_t nodeDeviceEUI[8] = { 0xAC, 0x1F, 0x09, 0xFF, 0xFE, 0x06, 0xD3, 0xE9 };
uint8_t nodeAppEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t nodeAppKey[16] = { 0xE9, 0x0D, 0x28, 0x1D, 0x92, 0x3D, 0xE4, 0x9F, 0xD9, 0xF7, 0xE0, 0x5F, 0x39, 0x10, 0x55, 0x71 };

// ABP keys
uint32_t nodeDevAddr = 0x260116F8;
uint8_t nodeNwsKey[16] = { 0x7E, 0xAC, 0xE2, 0x55, 0xB8, 0xA5, 0xE2, 0x69, 0x91, 0x51, 0x96, 0x06, 0x47, 0x56, 0x9D, 0x23 };
uint8_t nodeAppsKey[16] = { 0xFB, 0xAC, 0xB6, 0x47, 0xF3, 0x58, 0x45, 0xC7, 0x50, 0x7D, 0xBF, 0x16, 0x8B, 0xA8, 0xC1, 0x7C };

// Private definition
#define LORAWAN_APP_DATA_BUFF_SIZE 64                                            /**< buffer size of the data to be transmitted. */
static uint8_t m_lora_app_data_buffer[LORAWAN_APP_DATA_BUFF_SIZE];               //< Lora user application data buffer.
static lmh_app_data_t m_lora_app_data = { m_lora_app_data_buffer, 0, 0, 0, 0 };  //< Lora user application data structure.

static uint32_t count = 0;
static uint32_t count_fail = 0;

void setup() {

  // Initialize Serial for debug output
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, HIGH);

  // Sensor Power Switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  // Reset RAK14003 module
  pinMode(WB_IO4, OUTPUT);
  digitalWrite(WB_IO4, HIGH);
  delay(10);
  digitalWrite(WB_IO4, LOW);
  delay(10);
  digitalWrite(WB_IO4, HIGH);
  delay(10);

  // Initialize LoRa chip.
  lora_rak4630_init();

  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  Serial.println("=====================================");
  Serial.println("Welcome to RAK4630 LoRaWan!!!");
  if (doOTAA) {
    Serial.println("Type: OTAA");
  } else {
    Serial.println("Type: ABP");
  }

  switch (g_CurrentRegion) {
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
  if (doOTAA) {
    lmh_setDevEui(nodeDeviceEUI);
    lmh_setAppEui(nodeAppEUI);
    lmh_setAppKey(nodeAppKey);
  } else {
    lmh_setNwkSKey(nodeNwsKey);
    lmh_setAppSKey(nodeAppsKey);
    lmh_setDevAddr(nodeDevAddr);
  }

  uint32_t err_code;
  // Initialize LoRaWan
  err_code = lmh_init(&g_lora_callbacks, g_lora_param_init, doOTAA, g_CurrentClass, g_CurrentRegion);
  if (err_code != 0) {
    Serial.printf("lmh_init failed - %d\n", err_code);
    return;
  }

  // Start Join procedure
  lmh_join();

  Serial.println("================================");
  Serial.println("RAK12019 + RAK12010 LoRaWAN Code");
  Serial.println("================================");

  if (!VMEL.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }

  VMEL.setGain(VEML7700_GAIN_1);
  VMEL.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (VMEL.getGain())
  {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
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

	Serial.println("RAK12019 test");
	Wire.begin();
	if (!ltr.init())
	{
		Serial.println("Couldn't find LTR sensor!");
		while (1)
			delay(10);
	}
	Serial.println("Found LTR390 sensor!");

	//set to LTR390_MODE_UVS,get ultraviolet light data.
  ltr.setMode(LTR390_MODE_UVS);  // UVS Mode

	Serial.println("In UVS mode");

	ltr.setGain(LTR390_GAIN_3);
	Serial.print("Gain : ");
	switch (ltr.getGain())
	{
	case LTR390_GAIN_1:
		Serial.println(1);
		break;
	case LTR390_GAIN_3:
		Serial.println(3);
		break;
	case LTR390_GAIN_6:
		Serial.println(6);
		break;
	case LTR390_GAIN_9:
		Serial.println(9);
		break;
	case LTR390_GAIN_18:
		Serial.println(18);
		break;
	default:
		Serial.println("Failed to set gain");
		break;
	}
	ltr.setResolution(LTR390_RESOLUTION_16BIT);
	Serial.print("Integration Time (ms): ");
	switch (ltr.getResolution())
	{
	case LTR390_RESOLUTION_13BIT:
		Serial.println(13);
		break;
	case LTR390_RESOLUTION_16BIT:
		Serial.println(16);
		break;
	case LTR390_RESOLUTION_17BIT:
		Serial.println(17);
		break;
	case LTR390_RESOLUTION_18BIT:
		Serial.println(18);
		break;
	case LTR390_RESOLUTION_19BIT:
		Serial.println(19);
		break;
	case LTR390_RESOLUTION_20BIT:
		Serial.println(20);
		break;
	default:
		Serial.println("Failed to set Integration Time");
		break;
	}

	ltr.setThresholds(100, 1000); //Set the interrupt output threshold range for lower and upper.

  ltr.configInterrupt(true, LTR390_MODE_UVS);

  // For RAK14003
  mcp.begin_I2C(0x24);

  // Added for the functionality of RAK14003

  Wire.begin();    // initialize I2C serial bus

  // Configuration of MCP23017 I/O ports
  mcp.pinMode(LED_0, OUTPUT);     // Red LED; Top most
  mcp.pinMode(LED_1, OUTPUT);     // Red LED
  mcp.pinMode(LED_2, OUTPUT);     // Orange LED
  mcp.pinMode(LED_3, OUTPUT);     // Orange LED
  mcp.pinMode(LED_4, OUTPUT);     // Orange LED
  mcp.pinMode(LED_5, OUTPUT);     // Green LED
  mcp.pinMode(LED_6, OUTPUT);     // Green LED
  mcp.pinMode(LED_7, OUTPUT);     // Green LED
  mcp.pinMode(LED_8, OUTPUT);     // Green LED
  mcp.pinMode(LED_9, OUTPUT);     // Green LED; Bottom

  // Initially Turn OFF all LEDs
  mcp.digitalWrite(LED_0, HIGH);
  mcp.digitalWrite(LED_1, HIGH);
  mcp.digitalWrite(LED_2, HIGH);
  mcp.digitalWrite(LED_3, HIGH);
  mcp.digitalWrite(LED_4, HIGH);
  mcp.digitalWrite(LED_5, HIGH);
  mcp.digitalWrite(LED_6, HIGH);
  mcp.digitalWrite(LED_7, HIGH);
  mcp.digitalWrite(LED_8, HIGH);
  mcp.digitalWrite(LED_9, HIGH);

}

void loop() {
  // Put your application tasks here, like reading of sensors,
  // Controlling actuators and/or other functions.

  // Trial # 1
  // Data from RAK12019
  if (ltr.newDataAvailable())
  {
    uvi = ltr.getUVI();
    uvs = ltr.readUVS();
  }

  // Data from RAK12010
  lux = VMEL.readLux();
  white = VMEL.readWhite();
  raw_als = VMEL.readALS();

  Serial.println("Sending frame now...");
  send_lora_frame();

  // Showing only UV Index (uvi), UV Intensity (uvs) and Lux (lx)

  // From RAK12019 module
  Serial.print("UVI: ");
  Serial.println(uvi);          // Uvi data from RAK12019
  Serial.print("UVS: ");
  Serial.println(uvs);            // Uvs data from RAK12019

  // From RAK12010
  Serial.print("Lux: ");
  Serial.print(lux);
  Serial.println(" lx ");
  Serial.print("White: ");
  Serial.println(white);
  Serial.print("Raw ALS: ");
  Serial.println(raw_als);

  // Result of uvi to be displayed via RAK14003 module
  if (uvi <= 0.4)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, HIGH);
    mcp.digitalWrite(LED_6, HIGH);
    mcp.digitalWrite(LED_7, HIGH);
    mcp.digitalWrite(LED_8, HIGH);
    mcp.digitalWrite(LED_9, HIGH);
  }

  else if (uvi > 0.5 && uvi <= 1)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, HIGH);
    mcp.digitalWrite(LED_6, HIGH);
    mcp.digitalWrite(LED_7, HIGH);
    mcp.digitalWrite(LED_8, HIGH);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 1.5 && uvi <= 2)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, HIGH);
    mcp.digitalWrite(LED_6, HIGH);
    mcp.digitalWrite(LED_7, HIGH);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 2.5 && uvi <= 3)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, HIGH);
    mcp.digitalWrite(LED_6, HIGH);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 3.5 && uvi <= 4)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, HIGH);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 4.5 && uvi <= 5)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, HIGH);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 5.5 && uvi <= 6)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, HIGH);
    mcp.digitalWrite(LED_4, LOW);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 6.5 && uvi <= 7)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, HIGH);
    mcp.digitalWrite(LED_3, LOW);
    mcp.digitalWrite(LED_4, LOW);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 7.5 && uvi <= 8)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, HIGH);
    mcp.digitalWrite(LED_2, LOW);
    mcp.digitalWrite(LED_3, LOW);
    mcp.digitalWrite(LED_4, LOW);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi > 8.5 && uvi <= 9.5)
  {
    mcp.digitalWrite(LED_0, HIGH);
    mcp.digitalWrite(LED_1, LOW);
    mcp.digitalWrite(LED_2, LOW);
    mcp.digitalWrite(LED_3, LOW);
    mcp.digitalWrite(LED_4, LOW);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }

  else if (uvi >= 10)
  {
    mcp.digitalWrite(LED_0, LOW);
    mcp.digitalWrite(LED_1, LOW);
    mcp.digitalWrite(LED_2, LOW);
    mcp.digitalWrite(LED_3, LOW);
    mcp.digitalWrite(LED_4, LOW);
    mcp.digitalWrite(LED_5, LOW);
    mcp.digitalWrite(LED_6, LOW);
    mcp.digitalWrite(LED_7, LOW);
    mcp.digitalWrite(LED_8, LOW);
    mcp.digitalWrite(LED_9, LOW);
  }
  
  delay(10000);
}

/**@brief LoRa function for handling HasJoined event.
 */
void lorawan_has_joined_handler(void) {
  Serial.println("OTAA Mode, Network Joined!");
}

/**@brief LoRa function for handling OTAA join failed
*/
static void lorawan_join_failed_handler(void) {
  Serial.println("OTAA join failed!");
  Serial.println("Check your EUI's and Keys's!");
  Serial.println("Check if a Gateway is in range!");
}
/**@brief Function for handling LoRaWan received data from Gateway
 *
 * @param[in] app_data  Pointer to rx data
 */
void lorawan_rx_handler(lmh_app_data_t *app_data) {
  Serial.printf("LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s\n",
                app_data->port, app_data->buffsize, app_data->rssi, app_data->snr, app_data->buffer);
}

void lorawan_confirm_class_handler(DeviceClass_t Class) {
  Serial.printf("switch to class %c done\n", "ABC"[Class]);
  // Informs the server that switch has occurred ASAP
  m_lora_app_data.buffsize = 0;
  m_lora_app_data.port = gAppPort;
  lmh_send(&m_lora_app_data, g_CurrentConfirm);
}

String data = "";

void send_lora_frame(void) {
  if (lmh_join_status_get() != LMH_SET) {
    //Not joined, try again later
    return;
  }

  Serial.print("result: ");
  uint32_t i = 0;
  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  m_lora_app_data.port = gAppPort;

  // Showing only UV Index (uvi), UV Intensity (uvs) and Lux (lx)
  data = " Uvi Data: " + String(uvi) + " Uvs Data: " + String(uvs) + " Lux Data: " + String(lux) + " lx " ;

  Serial.println(data);

  // Showing only UV Index (uvi), UV Intensity (uvs) and Lux (lx)
  uint16_t uvi_data = uvi;            // Uvi data from RAK12019
  uint32_t uvs_data = uvs;            // Uvs data from RAK12019
  uint32_t lux_data = lux;            // Lux data from RAK12010
  
  m_lora_app_data.buffer[i++] = 0x01;  // byte[0]

  // Showing only UV Index (uvi), UV Intensity (uvs) and Lux (lx)
  // Data from RAK12019 module
  m_lora_app_data.buffer[i++] = (uint8_t)((uvi_data & 0x0000FF00) >> 8);     // byte[1]
  m_lora_app_data.buffer[i++] = (uint8_t)(uvi_data & 0x000000FF);            // byte[2]

  // Data from RAK12019 module
  m_lora_app_data.buffer[i++] = (uint8_t)((uvs_data & 0xFF000000) >> 24);    // byte[3]
  m_lora_app_data.buffer[i++] = (uint8_t)((uvs_data & 0x00FF0000) >> 16);    // byte[4]
  m_lora_app_data.buffer[i++] = (uint8_t)((uvs_data & 0x0000FF00) >> 8);     // byte[5]
  m_lora_app_data.buffer[i++] = (uint8_t)(uvs_data & 0x000000FF);            // byte[6]

  // Data from RAK12010 module
  m_lora_app_data.buffer[i++] = (uint8_t)((lux_data & 0xFF000000) >> 24);    // byte[7]
  m_lora_app_data.buffer[i++] = (uint8_t)((lux_data & 0x00FF0000) >> 16);    // byte[8]
  m_lora_app_data.buffer[i++] = (uint8_t)((lux_data & 0x0000FF00) >> 8);     // byte[9]
  m_lora_app_data.buffer[i++] = (uint8_t)(lux_data & 0x000000FF);            // byte[10]
  
  m_lora_app_data.buffsize = i;

  lmh_error_status error = lmh_send(&m_lora_app_data, g_CurrentConfirm);
  if (error == LMH_SUCCESS) {
    count++;
    Serial.printf("lmh_send ok count %d\n", count);
  } else {
    count_fail++;
    Serial.printf("lmh_send fail count %d\n", count_fail);
  }
}