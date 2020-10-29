/**
 * @file RAK1902_Pressure_LPS22HB.ino
 * @author rakwireless.com
 * @brief Setup and read values from a LPS22HB barometric  sensor
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
#include <Arduino_LPS22HB.h> // Click here to get the library: http://librarymanager/All#Arduino_LPS22HB

void setup()
{
	// Setup usb
	Serial.begin(115200);
	while (!Serial)
		;

	/* LPS22HB init */
	if (!BARO.begin())
	{
		Serial.println("Failed to initialize pressure sensor!");
		while (1)
			;
	}
}

void loop()
{

	lps22hb_get();
	delay(1000);
}

void lps22hb_get()
{
	// read the sensor value
	float pressure = BARO.readPressure();

	// print the sensor value
	Serial.print("Pressure = ");
	Serial.print(pressure);
	Serial.println(" kPa");
}
