/**
 * @file bme680_basic.ino
 * @author rakwireless.com
 * @brief Setup and read values from a BME680 environment sensor
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */
#include <Wire.h>
#include "ClosedCube_BME680.h" //http://librarymanager/All#ClosedCube_BME680

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
}

void setup()
{
	// Initialize Serial for debug output
	Serial.begin(115200);

	bme680_init();
}

void loop()
{
	bme680_get();
	delay(5000);
}
