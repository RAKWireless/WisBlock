/*
   This sketch demonstrate how to get environment data from BME680
   and send the data to ble gateway with ble gatt.

   ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server

*/
#include <bluefruit.h>
#include <Wire.h>
#include "BlueDot_BME680.h" // Click here to get the library: http://librarymanager/All#BlueDot_BME680

BlueDot_BME680 bme680 = BlueDot_BME680();
float temperature = 0;
float humidity = 0;
float pressure = 0;


/* Environment Service Definitions
   Environment Monitor Service:  0x181A
   Pressure Measurement Char: 0x2A6D
   Temperature Measurement Char: 0x2A6E
   humidity Measurement Char: 0x2A6F
*/
BLEService        envms = BLEService(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLECharacteristic pressuremc = BLECharacteristic(0x2A6D);
BLECharacteristic temprtmc = BLECharacteristic(UUID16_CHR_TEMPERATURE);
BLECharacteristic humidmc = BLECharacteristic(0x2A6F);// humidity

BLEDis bledis;    // DIS (Device Information Service) helper class instance
BLEBas blebas;    // BAS (Battery Service) helper class instance

void bme680_init() {
  //*********************************************************************
  //*************BASIC SETUP - READ BEFORE GOING ON!*********************

  //Set the I2C address of your breakout board

  //0x76:       Alternative I2C Address (SDO pin connected to GND)
  //0x77:       Default I2C Address (SDO pin unconnected)
  Wire.begin();
  bme680.parameter.I2CAddress = 0x76;                  //Choose I2C Address


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Now the device will be set to forced mode
  //This is the default setup
  //Please note that unlike the BME280, BME680 does not have a normal mode

  //0b01:     In forced mode a single measured is performed and the device returns automatically to sleep mode

  bme680.parameter.sensorMode = 0b01;                   //Default sensor mode


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Great! Now set up the internal IIR Filter
  //The IIR (Infinite Impulse Response) filter suppresses high frequency fluctuations
  //In short, a high factor value means less noise, but measurements are also less responsive
  //You can play with these values and check the results!
  //In doubt just leave on default

  //0b000:      factor 0 (filter off)
  //0b001:      factor 1
  //0b010:      factor 3
  //0b011:      factor 7
  //0b100:      factor 15 (default value)
  //0b101:      factor 31
  //0b110:      factor 63
  //0b111:      factor 127 (maximum value)

  bme680.parameter.IIRfilter = 0b100;                                   //Setting IIR Filter coefficient to 15 (default)


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Next you'll define the oversampling factor for the humidity measurements
  //Again, higher values mean less noise, but slower responses
  //If you don't want to measure humidity, set the oversampling to zero

  //0b000:      factor 0 (Disable humidity measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme680.parameter.humidOversampling = 0b101;                           //Setting Humidity Oversampling to factor 16 (default)


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Now define the oversampling factor for the temperature measurements
  //You know now, higher values lead to less noise but slower measurements

  //0b000:      factor 0 (Disable temperature measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme680.parameter.tempOversampling = 0b101; //Setting Temperature Oversampling factor to 16 (default)


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Finally, define the oversampling factor for the pressure measurements
  //For altitude measurements a higher factor provides more stable values
  //On doubt, just leave it on default

  //0b000:      factor 0 (Disable pressure measurement)
  //0b001:      factor 1
  //0b010:      factor 2
  //0b011:      factor 4
  //0b100:      factor 8
  //0b101:      factor 16 (default value)

  bme680.parameter.pressOversampling = 0b101; //Setting Pressure Oversampling to factor 16 (default)


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //For precise altitude measurements please put in the current pressure corrected for the sea level
  //On doubt, just leave the standard pressure as default (1013.25 hPa)

  bme680.parameter.pressureSeaLevel = 1013.25; //default value of 1013.25 hPa

  //Now write here the current average temperature outside (yes, the outside temperature!)
  //You can either use the value in Celsius or in Fahrenheit, but only one of them (comment out the other value)
  //In order to calculate the altitude, this temperature is converted by the library into Kelvin
  //For slightly less precise altitude measurements, just leave the standard temperature as default (15°C)
  //Remember, leave one of the values here commented, and change the other one!
  //If both values are left commented, the default temperature of 15°C will be used
  //But if both values are left uncommented, then the value in Celsius will be used

  bme680.parameter.tempOutsideCelsius = 15; //default value of 15°C
  //bme680.parameter.tempOutsideFahrenheit = 59; //default value of 59°F


  //*********************************************************************
  //*************ADVANCED SETUP - SAFE TO IGNORE!************************

  //Here we need to set the target temperature of the gas sensor hot plate
  //According to the datasheet, the target temperature is typically set between 200°C and 400°C
  //The default value is 320°C

  bme680.parameter.target_temp = 320;


  //*********************************************************************
  //*************ADVANCED SETUP IS OVER - LET'S CHECK THE CHIP ID!*******

  if (bme680.init() != 0x61)
  {
    //If the Arduino fails to identify the BME680, program stops and shows the Troubleshooting guide


    Serial.println(F("Ops! BME680 could not be found!"));
    Serial.println(F("Please check your connections."));
    Serial.println();
    Serial.println(F("Troubleshooting Guide"));
    Serial.println(F("*************************************************************"));
    Serial.println(F("1. Let's check the basics: Are the VCC and GND pins connected correctly? If the BME680 is getting really hot, then the wires are crossed."));
    Serial.println();
    Serial.println(F("2. Are you using the I2C mode? Did you connect the SDI pin from your BME680 to the SDA line from the Arduino?"));
    Serial.println();
    Serial.println(F("3. And did you connect the SCK pin from the BME680 to the SCL line from your Arduino?"));
    Serial.println();
    Serial.println(F("4. Are you using the alternative I2C Address(0x76)? Did you remember to connect the SDO pin to GND?"));
    Serial.println();
    Serial.println(F("5. If you are using the default I2C Address (0x77), did you remember to leave the SDO pin unconnected?"));
    Serial.println();
    Serial.println(F("6. Are you using the SPI mode? Did you connect the Chip Select (CS) pin to the pin 10 of your Arduino (or to wherever pin you choosed)?"));
    Serial.println();
    Serial.println(F("7. Did you connect the SDI pin from the BME680 to the MOSI pin from your Arduino?"));
    Serial.println();
    Serial.println(F("8. Did you connect the SDO pin from the BME680 to the MISO pin from your Arduino?"));
    Serial.println();
    Serial.println(F("9. And finally, did you connect the SCK pin from the BME680 to the SCK pin from your Arduino?"));
    Serial.println();

    while (1);
  }
  else
  {
    Serial.println(F("BME680 detected!"));
  }
  Serial.println();
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
  Bluefruit.Advertising.setInterval(160, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
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

  pressuremc.setProperties(CHR_PROPS_NOTIFY);
  pressuremc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  pressuremc.setFixedLen(4);
  pressuremc.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  pressuremc.begin();
  uint8_t pressuredata[4] = { 0x00 };
  pressuremc.write(pressuredata, 4);

  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.temperature.xml
  // Properties = Notify
  temprtmc.setProperties(CHR_PROPS_NOTIFY);
  temprtmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  temprtmc.setFixedLen(2);
  temprtmc.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  temprtmc.begin();
  uint8_t tmpdata[2] = { 0x00 }; // Set the characteristic to use 8-bit values, with the sensor connected and detected
  temprtmc.write(tmpdata, 2);

  // Configure the Environment Measurement characteristic
  // See: https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.humidity.xml
  // Properties = Notify
  humidmc.setProperties(CHR_PROPS_NOTIFY);
  humidmc.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  humidmc.setFixedLen(2);
  humidmc.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  humidmc.begin();
  uint8_t humiddata[2] = { 0x00 }; // Set the characteristic to use 8-bit values, with the sensor connected and detected
  humidmc.write(humiddata, 2);
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
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
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value)
{
  // Display the raw request packet
  Serial.print("CCCD Updated: ");
  //Serial.printBuffer(request->data, request->len);
  Serial.print(cccd_value);
  Serial.println("");

  // Check the characteristic this CCCD update is associated with in case
  // this handler is used for multiple CCCD records.
  if (chr->uuid == pressuremc.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Temperature Measurement 'Notify' enabled");
    } else {
      Serial.println("Temperature Measurement 'Notify' disabled");
    }
  }
  else if (chr->uuid == temprtmc.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Temperature Measurement 'Notify' enabled");
    } else {
      Serial.println("Temperature Measurement 'Notify' disabled");
    }
  }
  else if (chr->uuid == humidmc.uuid) {
    if (chr->notifyEnabled(conn_hdl)) {
      Serial.println("Humidity Measurement 'Notify' enabled");
    } else {
      Serial.println("Humidity Measurement 'Notify' disabled");
    }
  }
}

void setup()
{
  Serial.begin(115200);
  // while ( !Serial ) delay(10);   // for nrf52840 with native usb

  /* bme680 init */
  bme680_init();

  Serial.println("RAK Wisnode EnvM Example");
  Serial.println("-----------------------\n");

  // Initialise the Bluefruit module
  Serial.println("Initialise the Bluefruit module");
  Bluefruit.begin();

  // Set the advertised device name (keep it short!)
  Serial.println("Setting Device Name to 'RAKwisnode EnvM'");
  Bluefruit.setName("RAKwisnode EnvM");

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
  digitalToggle(LED_RED);

  if ( Bluefruit.connected() ) {
    bme680.writeCTRLMeas();
    temperature = bme680.readTempC();
    humidity = bme680.readHumidity();
    pressure = bme680.readPressure();

    uint8_t tmpdata[2] = { 0x0 }; // little-endian
    tmpdata[1] = (uint8_t)(((int)(temperature * 100) & 0xff00) >> 8);
    tmpdata[0] = (uint8_t)((int)(temperature * 100) & 0xff);

    uint8_t humiddata[2] = { 0x0 };// little-endian
    humiddata[1] = (uint8_t)(((int)(humidity * 100) & 0xff00) >> 8);
    humiddata[0] = (uint8_t)((int)(humidity * 100) & 0xff);

    uint8_t pressuredata[4] = { 0x0 };// little-endian
    pressuredata[3] = (uint8_t)(((int)(pressure * 1000) & 0xff000000) >> 24);
    pressuredata[2] = (uint8_t)(((int)(pressure * 1000) & 0xff0000) >> 16);
    pressuredata[1] = (uint8_t)(((int)(pressure * 1000) & 0xff00) >> 8);
    pressuredata[0] = (uint8_t)((int)(pressure * 1000) & 0xff);

    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent
    if ( pressuremc.notify(pressuredata, sizeof(pressuredata)) ) {
      Serial.print("Pressure Measurement updated to: "); Serial.println(pressure);
    }
    if ( temprtmc.notify(tmpdata, sizeof(tmpdata)) ) {
      Serial.print("Temperature Measurement updated to: "); Serial.println(temperature);
    }
    if ( humidmc.notify(humiddata, sizeof(humiddata)) ) {
      Serial.print("Humidityrature Measurement updated to: "); Serial.println(humidity);
    }
  }

  delay(20000);
}
