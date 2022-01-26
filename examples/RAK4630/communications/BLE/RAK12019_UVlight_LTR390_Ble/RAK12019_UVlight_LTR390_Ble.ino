/**
   @file ble_environment_node.ino
   @author rakwireless.com
   @brief This sketch demonstrate how to get environment data from LTR390
      and send the data to ble gateway with ble gatt.
      ble environment node <-ble-> ble gateway <-lora-> lora gateway <--> lora server
   @version 0.1
   @date 2021-09-13
   @copyright Copyright (c) 2021
**/

#include <bluefruit.h>
#include <Wire.h>

#include "UVlight_LTR390.h" //Click here to get the library:

//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
UVlight_LTR390 ltr = UVlight_LTR390();
void LTR390_Init(void);
/*
   For device under tinted window with coated-ink of flat transmission rate at 400-600nm wavelength,
   window factor  is to  compensate light  loss due to the  lower  transmission rate from the coated-ink.
      a. WFAC = 1 for NO window / clear window glass.
      b. WFAC >1 device under tinted window glass. Calibrate under white LED.
*/
static float ALS = 0;
static float LUX = 0;
static float UVS = 0;
static float UVI = 0;
static uint8_t lightdata[8] = {0};

/* Environment Service Definitions
   Environment Monitor Service:  0x181A
   Pressure Measurement Char: 0x2A6D
   Temperature Measurement Char: 0x2A6E
   humidity Measurement Char: 0x2A6F
   UV Index Measurement Char: 0x2A76
*/
BLEService envms = BLEService(UUID16_SVC_ENVIRONMENTAL_SENSING);
BLECharacteristic environment_UVlight = BLECharacteristic(UUID16_CHR_UV_INDEX);

BLEDis bledis; // DIS (Device Information Service) helper class instance
BLEBas blebas; // BAS (Battery Service) helper class instance

/*
  @brief LTP390 initialize
*/
void LTR390_Init(void)
{
	//Sensor power switch
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(300);

	Serial.println("RAK12019 Ble test");
	Wire.begin();
	if (!ltr.init())
	{
		Serial.println("Couldn't find LTR sensor!");
		while (1)
			delay(10);
	}
	Serial.println("Found LTR390 sensor!");

	//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
	ltr.setMode(LTR390_MODE_ALS); //LTR390_MODE_UVS
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		Serial.println("In ALS mode");
	}
	else
	{
		Serial.println("In UVS mode");
	}

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
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		ltr.configInterrupt(true, LTR390_MODE_ALS); //Configure the interrupt based on the thresholds in setThresholds()
	}
	else
	{
		ltr.configInterrupt(true, LTR390_MODE_UVS);
	}
}

/**@brief Function for the Sensor get data.
*/
String data = "";
void UVlight_Ltr390_GetData(void)
{
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		uint32_t sendlux = 0;
		uint32_t sendals = 0;
		uint8_t count = 0;
		ALS = ltr.readALS();
		LUX = ltr.getLUX();
		sendlux = LUX * 100;
		sendals = ALS;
		data = "Lux = " + String(LUX) + " " + "sendals=" + String(sendals) + " ";
		Serial.println(data);
		data = "";
		lightdata[count++] = (uint8_t)((sendlux & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((sendlux & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((sendlux & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(sendlux & 0x000000FF);
		lightdata[count++] = (uint8_t)((sendals & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((sendals & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((sendals & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(sendals & 0x000000FF);
		ALS = 0;
		LUX = 0;
	}
	else
	{
		uint32_t senduvs = 0;
		uint32_t senduvi = 0;
		uint8_t count = 0;
		UVS = ltr.readUVS();
		UVI = ltr.getUVI(); //calculate the UVI
		senduvi = UVI * 100;
		senduvs = UVS;
		data = "UVI = " + String(UVI) + " " + " UVS=" + String(senduvs) + " ";
		Serial.println(data);
		data = "";
		lightdata[count++] = (uint8_t)((senduvi & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((senduvi & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((senduvi & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(senduvi & 0x000000FF);
		lightdata[count++] = (uint8_t)((senduvs & 0xFF000000) >> 24);
		lightdata[count++] = (uint8_t)((senduvs & 0x00FF0000) >> 16);
		lightdata[count++] = (uint8_t)((senduvs & 0x0000FF00) >> 8);
		lightdata[count++] = (uint8_t)(senduvs & 0x000000FF);
		UVS = 0;
		LUX = 0;
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

	environment_UVlight.setProperties(CHR_PROPS_NOTIFY);
	environment_UVlight.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
	environment_UVlight.setFixedLen(8);
	environment_UVlight.setCccdWriteCallback(cccd_callback); // Optionally capture CCCD updates
	environment_UVlight.begin();
	uint8_t environment_UVlight_data[8] = {0x00};
	environment_UVlight.write(environment_UVlight_data, 8);
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
	if (chr->uuid == environment_UVlight.uuid)
	{
		if (chr->notifyEnabled(conn_hdl))
		{
			Serial.println("UVlight  Measurement 'Notify' enabled");
		}
		else
		{
			Serial.println("UVlight  Measurement 'Notify' disabled");
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
	/* LTR390 init */
	LTR390_Init();
	Serial.println("RAK4631 UVLIGHT Ble Example");
	Serial.println("-----------------------\n");

	// Initialise the Bluefruit module
	Serial.println("Initialise the Bluefruit module");
	Bluefruit.begin();

	// Set the advertised device name (keep it short!)
	Serial.println("Setting Device Name to 'RAK4631 UVLIGHT'");
	Bluefruit.setName("RAK4631 UVLIGHT");

	Bluefruit.setTxPower(4);

	// Set the connect/disconnect callback handlers
	Bluefruit.Periph.setConnectCallback(connect_callback);
	Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

	// Configure and Start the Device Information Service
	Serial.println("Configuring the Device Information Service");
	bledis.setManufacturer("Rakwireless Industries");
	bledis.setModel("Bluefruit Feather52");
	bledis.begin();

	// Start the BLE Battery Service and set it to 100%
	Serial.println("Configuring the Battery Service");
	blebas.begin();
	blebas.write(100);

	// Setup the Environment service using
	// BLEService and BLECharacteristic classes
	Serial.println("Configuring the UVlight Monitor Service");
	setupEnvm();

	// Setup the advertising packet(s)
	Serial.println("Setting up the advertising payload(s)");
	startAdv();

	Serial.println("\nAdvertising");
}

void loop()
{
	//digitalToggle(LED_GREEN);
	if (Bluefruit.connected())
	{
		UVlight_Ltr390_GetData();
		// Note: We use .notify instead of .write!
		// If it is connected but CCCD is not enabled
		// The characteristic's value is still updated although notification is not sent
		if (environment_UVlight.notify(lightdata, sizeof(lightdata)))
		{
			Serial.print("Light Measurement updated to: ");
			//Serial.println(LUX_VEML);
		}
	}
	delay(2000);
}
