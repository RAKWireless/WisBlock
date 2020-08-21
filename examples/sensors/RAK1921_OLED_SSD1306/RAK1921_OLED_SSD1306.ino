/**
 * @file RAK1920_QWIIC_AirQuality_SGP30.ino
 * @author rakwireless.com
 * @brief This sketch demonstrate how to use SSD1306 to display sensor data of BME680.
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
 * IO1 <-> P0.17 (Arduino GPIO number 17)
 * IO2 <-> P1.02 (Arduino GPIO number 34)
 * IO3 <-> P0.21 (Arduino GPIO number 21)
 * IO4 <-> P0.04 (Arduino GPIO number 4)
 * IO5 <-> P0.09 (Arduino GPIO number 9)
 * IO6 <-> P0.10 (Arduino GPIO number 10)
 * SW1 <-> P0.01 (Arduino GPIO number 1)
 */

#include <Wire.h>
#include "ClosedCube_BME680.h" //https://github.com/closedcube/ClosedCube_BME680_Arduino
#include <U8g2lib.h>		   //https://github.com/olikraus/u8g2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
ClosedCube_BME680 bme680;

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

void bme680_get()
{
	char data[32] = {0};
	double temp = bme680.readTemperature();
	double pres = bme680.readPressure();
	double hum = bme680.readHumidity();

	Serial.print("T=");
	Serial.print(temp);
	Serial.print("C, RH=");
	Serial.print(hum);
	Serial.print("%, P=");
	Serial.print(pres);
	Serial.print("hPa");

	uint32_t gas = bme680.readGasResistance();

	Serial.print(", G=");
	Serial.print(gas);
	Serial.print(" Ohms");
	Serial.println();

	bme680.setForcedMode();

	// display bme680 sensor data on OLED
	u8g2.clearBuffer();					// clear the internal memory
	u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font

	memset(data, 0, sizeof(data));
	sprintf(data, "T=%.2fC", temp);
	u8g2.drawStr(3, 15, data);

	memset(data, 0, sizeof(data));
	snprintf(data, 64, "RH=%.2f%%", hum);
	u8g2.drawStr(3, 30, data);

	memset(data, 0, sizeof(data));
	sprintf(data, "P=%.2fhPa", pres);
	u8g2.drawStr(3, 45, data);

	memset(data, 0, sizeof(data));
	sprintf(data, "G=%dOhms", gas);
	u8g2.drawStr(3, 60, data);

	u8g2.sendBuffer(); // transfer internal memory to the display
}

void setup()
{
	// Initialize Serial for debug output
	Serial.begin(115200);
	// while(!Serial){delay(10);}

	bme680_init();
	u8g2.begin();
}

void loop()
{
	bme680_get();
	delay(5000);
}
