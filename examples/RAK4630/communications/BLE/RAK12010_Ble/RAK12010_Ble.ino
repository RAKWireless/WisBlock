/**
   @file ble_environment_node.ino
   @author rakwireless.com
   @brief This sketch demonstrate how to get environment data from VEML7700
      and send the data to ble gateway with ble gatt.
      ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
   @version 0.1
   @date 2021-07-14
   @copyright Copyright (c) 2020
**/

#include <bluefruit.h>
#include <Wire.h>

#include "Light_VEML7700.h"

Light_VEML7700 VMEL = Light_VEML7700();
void Sensor_VEML7700_Init(void);
void Sensor_VEML7700_GetData(void);
void Sensor_VEML7700_Print(void);
uint8_t lightdata[10] = {0};


/* Environment Service Definitions
   Environment Monitor Service:  0x181A
   Pressure Measurement Char: 0x2A6D
   Temperature Measurement Char: 0x2A6E
   humidity Measurement Char: 0x2A6F
*/
#define  UUID16_CHR_LIGHT    0x2AD0
BLEService envms = BLEService(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLECharacteristic environment_light = BLECharacteristic(UUID16_CHR_LIGHT);

BLEDis bledis; // DIS (Device Information Service) helper class instance
BLEBas blebas; // BAS (Battery Service) helper class instance


/**@brief Function for sensor initialize
   time:2021/07/15
*/
void Sensor_VEML7700_Init(void)
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  if (!VMEL.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }
  Serial.println("Sensor found");
  VMEL.setGain(VEML7700_GAIN_1_8);
  VMEL.setIntegrationTime(VEML7700_IT_25MS);
  Sensor_VEML7700_Print();
  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  VMEL.setLowThreshold(10);
  VMEL.setHighThreshold(30000);
  VMEL.interruptEnable(true);
}

/**@brief Function for sensor gain and IntegrationTime print
   time:2021/07/15
*/
void Sensor_VEML7700_Print(void)
{
  Serial.print(F("Gain: "));
  switch (VMEL.getGain())
  {
    case VEML7700_GAIN_1: Serial.print("1  "); break;
    case VEML7700_GAIN_2: Serial.print("2  "); break;
    case VEML7700_GAIN_1_4: Serial.print("1/4  "); break;
    case VEML7700_GAIN_1_8: Serial.print("1/8  "); break;
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
}

/**@brief Function for the Sensor get data.
*/
String data="";
void Sensor_VEML7700_GetData(void)
{
  uint32_t Lux = 0;
  uint32_t White = 0;
  uint32_t Als = 0;
  uint8_t count = 0;
  Lux = VMEL.readLux()  * 100;
  White = VMEL.readWhite() * 100;
  Als = VMEL.readALS();
  data = "Lux = " + String(Lux) + " " + " White = " + String(White) + " " + " Als=" + String(Als) + " ";
  Serial.println(data);
  data = "";
  lightdata[count++] = (uint8_t)((Lux & 0xFF000000) >> 24);
  lightdata[count++] = (uint8_t)((Lux & 0x00FF0000) >> 16);
  lightdata[count++] = (uint8_t)((Lux & 0x0000FF00) >> 8);
  lightdata[count++] = (uint8_t)(Lux &  0x000000FF);
  lightdata[count++] = (uint8_t)((White & 0xFF000000) >> 24);
  lightdata[count++] = (uint8_t)((White & 0x00FF0000) >> 16);
  lightdata[count++] = (uint8_t)((White & 0x0000FF00) >> 8);
  lightdata[count++] = (uint8_t)(White &  0x000000FF);
  lightdata[count++] = (uint8_t)((Als & 0x0000FF00) >> 8);
  lightdata[count++] = (uint8_t)(Als & 0xFF);
  uint16_t irq = VMEL.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW) {
    Serial.println("** Low threshold");
  }
  if (irq & VEML7700_INTERRUPT_HIGH) {
    Serial.println("** High threshold");
  }
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include Environment Service UUID
  Bluefruit.Advertising.addService(envms);

  // Include Name
  Bluefruit.Advertising.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)
     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 244); // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);	 // number of seconds in fast mode
  Bluefruit.Advertising.start(0);				 // 0 = Don't stop advertising after n seconds
}

void setupEnvm(void)
{
  // Configure the Environment service
  // See: https://www.bluetooth.com/specifications/assigned-numbers/environmental-sensing-service-characteristics/
  // Supported Characteristics:
  // Name                         UUID    Requirement Properties
  // ---------------------------- ------  ----------- ----------
  // Pressure Measurement: 0x2A6D
  // Temperature Measurement: 0x2A6E
  // humidity Measurement: 0x2A6F
  envms.begin();

  // Note: You must call .begin() on the BLEService before calling .begin() on
  // any characteristic(s) within that service definition.. Calling .begin() on
  // a BLECharacteristic will cause it to be added to the last BLEService that
  // was 'begin()'ed!

  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pressure.xml
  // Properties = Notify

  environment_light.setProperties(CHR_PROPS_NOTIFY);
  environment_light.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  environment_light.setFixedLen(10);
  environment_light.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
  environment_light.begin();
  uint8_t environment_light_data[10] = {0x00};
  environment_light.write(environment_light_data, 10);
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection *connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = {0};
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
   Callback invoked when a connection is dropped
   @param conn_handle connection where this event happens
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void)conn_handle;
  (void)reason;

  Serial.print("Disconnected, reason = 0x");
  Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic *chr, uint16_t cccd_value)
{
  // Display the raw request packet
  Serial.print("CCCD Updated: ");
  //Serial.printBuffer(request->data, request->len);
  Serial.print(cccd_value);
  Serial.println("");

  // Check the characteristic this CCCD update is associated with in case
  // this handler is used for multiple CCCD records.
  if (chr->uuid == environment_light.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Light  Measurement 'Notify' enabled");
    } else {
      Serial.println("Light  Measurement 'Notify' disabled");
    }
  }
}

void setup()
{
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
  /* VEML7700 init */
  Sensor_VEML7700_Init();
  Serial.println("RAK4631 LIGHT Example");
  Serial.println("-----------------------\n");

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit module");
  Bluefruit.begin();

  // Set the advertised device name (keep it short!)
  Serial.println("Setting Device Name to 'RAK4631 LIGHT'");
  Bluefruit.setName("RAK4631 LIGHT");

  Bluefruit.setTxPower(4);

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure and Start the Device Information Service
  Serial.println("Configuring the Device Information Service");
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Start the BLE Battery Service and set it to 100%
  Serial.println("Configuring the Battery Service");
  blebas.begin();
  blebas.write(100);

  // Setup the Environment service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Environment Monitor Service");
  setupEnvm();

  // Setup the advertising packet(s)
  Serial.println("Setting up the advertising payload(s)");
  startAdv();

  Serial.println("\nAdvertising");
}

void loop()
{
  digitalToggle(LED_GREEN);
  if (Bluefruit.connected())
  {
    Sensor_VEML7700_GetData();
    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent
    if (environment_light.notify(lightdata, sizeof(lightdata)))
    {
      Serial.print("Light Measurement updated to: ");
      //Serial.println(LUX_VEML);
    }
  }
  delay(2000);
}
