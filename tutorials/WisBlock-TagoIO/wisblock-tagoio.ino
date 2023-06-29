#include "SparkFun_SHTC3.h" 
#include "rak1902.h"
#include "rak1903.h"
#include "rak1904.h"
#include "SparkFun_MLX90632_Arduino_Library.h"
#include "UVlight_LTR390.h"

#define SENSOR_PIN  WB_IO6
#define VIBRATION_PIN WB_A1
#define MLX90632_ADDRESS 0x3A

UVlight_LTR390 ltr = UVlight_LTR390();
MLX90632 RAK_TempSensor;

rak1904 rak1904;
rak1903 rak1903;
rak1902 rak1902;
SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class


#define OTAA_PERIOD   (10000)
/*************************************

   LoRaWAN band setting:
     RAK_REGION_EU433
     RAK_REGION_CN470
     RAK_REGION_RU864
     RAK_REGION_IN865
     RAK_REGION_EU868
     RAK_REGION_US915
     RAK_REGION_AU915
     RAK_REGION_KR920
     RAK_REGION_AS923

 *************************************/
#define OTAA_BAND     (RAK_REGION_US915)
#define OTAA_DEVEUI   {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0xE8, 0xB1}
#define OTAA_APPEUI   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define OTAA_APPKEY   {0xDF, 0x81, 0xA4, 0x61, 0xDD, 0x6E, 0x61, 0x02, 0x2F, 0x53, 0xB4, 0x3E, 0xB9, 0xC3, 0x7F, 0x11}

/** Packet buffer for sending */
uint8_t collected_data[64] = { 0 };
float uv_raw;
uint16_t uv_index;

void recvCallback(SERVICE_LORA_RECEIVE_T * data)
{
  if (data->BufferSize > 0) {
    Serial.println("Something received!");
    for (int i = 0; i < data->BufferSize; i++) {
      Serial.printf("%x", data->Buffer[i]);
    }
    Serial.print("\r\n");
  }
}

void joinCallback(int32_t status)
{
  Serial.printf("Join status: %d\r\n", status);
}

/*************************************
   enum type for LoRa Event
    RAK_LORAMAC_STATUS_OK = 0,
    RAK_LORAMAC_STATUS_ERROR,
    RAK_LORAMAC_STATUS_TX_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_TIMEOUT,
    RAK_LORAMAC_STATUS_RX2_TIMEOUT,
    RAK_LORAMAC_STATUS_RX1_ERROR,
    RAK_LORAMAC_STATUS_RX2_ERROR,
    RAK_LORAMAC_STATUS_JOIN_FAIL,
    RAK_LORAMAC_STATUS_DOWNLINK_REPEATED,
    RAK_LORAMAC_STATUS_TX_DR_PAYLOAD_SIZE_ERROR,
    RAK_LORAMAC_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS,
    RAK_LORAMAC_STATUS_ADDRESS_FAIL,
    RAK_LORAMAC_STATUS_MIC_FAIL,
    RAK_LORAMAC_STATUS_MULTICAST_FAIL,
    RAK_LORAMAC_STATUS_BEACON_LOCKED,
    RAK_LORAMAC_STATUS_BEACON_LOST,
    RAK_LORAMAC_STATUS_BEACON_NOT_FOUND,
 *************************************/

void sendCallback(int32_t status)
{
  if (status == RAK_LORAMAC_STATUS_OK) {
    Serial.println("Successfully sent");
  } else {
    Serial.println("Sending failed");
  }
}

void setup()
{
  pinMode(SENSOR_PIN, INPUT);
  pinMode(VIBRATION_PIN, INPUT);
  TwoWire &wirePort = Wire;
  MLX90632::status returnError;
  Serial.begin(115200, RAK_AT_MODE);
  delay(2000);
  Wire.begin();
  mySHTC3.begin();
  rak1902.init();

  Serial.printf("RAK1903 init %s\r\n", rak1903.init() ? "Success" : "Fail");
  Serial.printf("RAK1904 init %s\r\n", rak1904.init() ? "Success" : "Fail");

  if (RAK_TempSensor.begin(MLX90632_ADDRESS, wirePort, returnError) == true) //MLX90632 init
  {
    Serial.println("MLX90632 Init Succeed");
  }
  else
  {
    Serial.println("MLX90632 Init Failed");
  }

  if (!ltr.init())
  {
    Serial.println("Couldn't find LTR sensor!");
  }
  else
  {
    Serial.println("Found LTR390 sensor!");
  }
  //if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
  ltr.setMode(LTR390_MODE_UVS); //LTR390_MODE_UVS
  ltr.setGain(LTR390_GAIN_3);
  ltr.setResolution(LTR390_RESOLUTION_16BIT);
  ltr.setThresholds(100, 1000);
  ltr.configInterrupt(true, LTR390_MODE_UVS);

  Serial.println("WisBlock TagoIO");
  Serial.println("------------------------------------------------------");

  if (api.lorawan.nwm.get() != 1)
  {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lorawan.nwm.set(1) ? "Success" : "Fail");
    api.system.reboot();
  }

  // OTAA Device EUI MSB first
  uint8_t node_device_eui[8] = OTAA_DEVEUI;
  // OTAA Application EUI MSB first
  uint8_t node_app_eui[8] = OTAA_APPEUI;
  // OTAA Application Key MSB first
  uint8_t node_app_key[16] = OTAA_APPKEY;

  if (!api.lorawan.appeui.set(node_app_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set application EUI is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.appkey.set(node_app_key, 16)) {
    Serial.printf("LoRaWan OTAA - set application key is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deui.set(node_device_eui, 8)) {
    Serial.printf("LoRaWan OTAA - set device EUI is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.band.set(OTAA_BAND)) {
    Serial.printf("LoRaWan OTAA - set band is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.deviceClass.set(RAK_LORA_CLASS_A)) {
    Serial.printf("LoRaWan OTAA - set device class is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.adr.set(true)) {
    Serial.printf("LoRaWan OTAA - set adaptive data rate is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.rety.set(5)) {
    Serial.printf("LoRaWan OTAA - set retry times is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.cfm.set(0)) {
    Serial.printf("LoRaWan OTAA - set confirm mode is incorrect! \r\n");
    return;
  }

  if (!api.lorawan.njm.set(RAK_LORA_OTAA))  // Set the network join mode to OTAA
  {
    Serial.printf("LoRaWan OTAA - set network join mode is incorrect! \r\n");
    return;
  }
  if (!api.lorawan.join())  // Join to Gateway
  {
    Serial.printf("LoRaWan OTAA - join fail! \r\n");
    return;
  }

  /** Check LoRaWan Status*/
  Serial.printf("Duty cycle is %s\r\n", api.lorawan.dcs.get() ? "ON" : "OFF");	// Check Duty Cycle status
  Serial.printf("Packet is %s\r\n", api.lorawan.cfm.get() ? "CONFIRMED" : "UNCONFIRMED");	// Check Confirm status
  uint8_t assigned_dev_addr[4] = { 0 };
  api.lorawan.daddr.get(assigned_dev_addr, 4);
  Serial.printf("Device Address is %02X%02X%02X%02X\r\n", assigned_dev_addr[0], assigned_dev_addr[1], assigned_dev_addr[2], assigned_dev_addr[3]);	// Check Device Address
  Serial.printf("Uplink period is %ums\r\n", OTAA_PERIOD);
  Serial.println("");
  api.lorawan.registerRecvCallback(recvCallback);
  api.lorawan.registerJoinCallback(joinCallback);
  api.lorawan.registerSendCallback(sendCallback);
}

void uplink_routine()
{
  float humidity_raw;
  float temperature_raw;
  float pressure_raw;
  float lux_raw;
  float x_raw, y_raw, z_raw;
  float ir_temp_raw;

  humidity_raw = mySHTC3.toPercent();
  temperature_raw = mySHTC3.toDegF();
  pressure_raw = rak1902.pressure();
  lux_raw = rak1903.lux();
  x_raw = abs(rak1904.x());
  y_raw = abs(rak1904.y());
  z_raw = abs(rak1904.z());
  ir_temp_raw = RAK_TempSensor.getObjectTempF();

  uint16_t humidity_data = humidity_raw * 100;
  uint16_t temperature_data = temperature_raw * 100;
  uint32_t pressure_data = pressure_raw * 100;
  uint16_t lux_data = lux_raw * 100;
  uint16_t x_data = x_raw * 100;
  uint16_t y_data = y_raw * 100;
  uint16_t z_data = z_raw * 100;
  uint16_t ir_temp_data = ir_temp_raw * 100;
  uint8_t motion_data = digitalRead(SENSOR_PIN);
  uint8_t vibration_data = digitalRead(VIBRATION_PIN);
  uint16_t uv_data = uv_raw * 100;

  Serial.print("RH = ");
  Serial.print(humidity_raw);                        
  Serial.print("%, T = ");
  Serial.print(temperature_raw);                           
  Serial.print(" deg F, Pressure = ");
  Serial.print(pressure_raw);                           
  Serial.print("hPa, Light = ");
  Serial.print(lux_raw);                           
  Serial.print("lux, X-axis = ");
  Serial.print(x_data);                           
  Serial.print(", Y-axis = ");
  Serial.print(y_data);                           
  Serial.print(", Z-axis = ");
  Serial.print(z_data);                           
  Serial.print(", IR temp = ");
  Serial.print(ir_temp_data);
  Serial.print(" deg F, Motion = ");
  Serial.print(motion_data);
  Serial.print(", Vibration = ");
  Serial.print(vibration_data);
  Serial.print(", UV = ");
  Serial.println(uv_data);

  /** Payload of Uplink */

  uint8_t data_len = 0;
  collected_data[data_len++] = humidity_data >> 8;
  collected_data[data_len++] = humidity_raw;
  collected_data[data_len++] = temperature_data >> 8;
  collected_data[data_len++] = temperature_data;
  collected_data[data_len++] = pressure_data >> 24;
  collected_data[data_len++] = pressure_data >> 16 ;
  collected_data[data_len++] = pressure_data >> 8;
  collected_data[data_len++] = pressure_data;
  collected_data[data_len++] = lux_data >> 8;
  collected_data[data_len++] = lux_data;
  collected_data[data_len++] = x_data >> 8;
  collected_data[data_len++] = x_data;
  collected_data[data_len++] = y_data >> 8;
  collected_data[data_len++] = y_data;
  collected_data[data_len++] = z_data >> 8;
  collected_data[data_len++] = z_data;
  collected_data[data_len++] = ir_temp_data >> 8;
  collected_data[data_len++] = ir_temp_data;
  collected_data[data_len++] = motion_data;
  collected_data[data_len++] = vibration_data;
  collected_data[data_len++] = uv_data >> 8;
  collected_data[data_len++] = uv_data;

  Serial.println("Data Packet:");
  for (int i = 0; i < data_len; i++) {
    Serial.printf("0x%02X ", collected_data[i]);
  }
  Serial.println("");

  /** Send the data package */
  if (api.lorawan.send(data_len, (uint8_t *) & collected_data, 1)) {
    Serial.println("Sending is requested");
  } else {
    Serial.println("Sending failed");
  }
}

void loop()
{
  static uint64_t last = 0;
  static uint64_t elapsed;

  SHTC3_Status_TypeDef result = mySHTC3.update();
  rak1903.update();
  rak1904.update();

  if (ltr.newDataAvailable())
  {
    uv_raw = ltr.readUVS();
  }
  if ((elapsed = millis() - last) > OTAA_PERIOD) {
    uplink_routine();

    last = millis();
  }
  //Serial.printf("Try sleep %ums..", OTAA_PERIOD);
  api.system.sleep.all(OTAA_PERIOD);
  //Serial.println("Wakeup..");
}
