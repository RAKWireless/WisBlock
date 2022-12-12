/**
 * @file RAK12044_Hall_BasicReadings_DRV5056.ino
 * @author rakwireless.com
 * @brief Read the magnetic field strength and output it through the serial port.
 * @version 0.1
 * @date 2022-02-22
 * @copyright Copyright (c) 2022
 */
#include <Wire.h>
#include "ADC121C021.h"

/*
 * @note The DRV5056 only responds to the flux density of a magnetic south pole.
 */
#define DRV5056_ADDRESS   0x56    // The device i2c address

ADC121C021 DRV5056;

void setup()
{
	pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); //power on RAK12044

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

	while (!(DRV5056.begin(DRV5056_ADDRESS, Wire)))
	{
		Serial.println("please check device!!!");
		delay(200);
	}
	Serial.println("RAK12044 Basic Readings Example.");

  DRV5056.configCycleTime(CYCLE_TIME_2048); //set ADC121C021 Conversion Cycle Time
  DRV5056.setVoltageResolution(3.3);
}
void loop()
{
	float Vout;
  float B;

	Vout = DRV5056.getSensorVoltage() * 1000;
	Serial.printf("Sensor analog output  : %5.1f mV\r\n", Vout);
  
  /*
   * @brief The DRV5056 outputs an analog voltage according to equation:
   *    B = (VOUT - VQ) / (Sensitivity(25°C) × (1 + STC × (TA ± 25°C))
   *    B   : is the applied magnetic flux density.
   *    VOUT: is within the VL(VQ ~ VCC–0.2) range.
   *    VQ  : typically 600 mV.
   *    Sensitivity(25°C) : depends on the device option and VCC. 
   *                        typically 15mV/mT,@VCC=3.3V,TA=25°C,DRV5056A4/Z4.
   *    STC : is typically 0.12%/°C for device options DRV5056A1 - DRV5056A4.
   *    TA  : is the ambient temperature.
   */
  B = (Vout - 600) / (15 * (1 + 0.0012 * (25 - 25)));     // DRV5056A4 @25°C.
  Serial.printf("Magnetic flux density : %5.2f mT\r\n", B);
	delay(200);
}
