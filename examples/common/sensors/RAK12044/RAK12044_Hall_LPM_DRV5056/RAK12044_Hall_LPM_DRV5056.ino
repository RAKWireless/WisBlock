/**
 * @file RAK12044_Hall_LPM_DRV5056.ino
 * @author rakwireless.com
 * @brief Linear Position Measurement. 
 *        This example shows how to use the RAK12044 to sense the linear displacement of a permanent magnet array.
 * @version 0.1
 * @date 2022-02-22
 * @copyright Copyright (c) 2022
 */
#include <Wire.h>
#include "ADC121C021.h"

/*
 * @note The DRV5056 only responds to the flux density of a magnetic south pole.
 */
#define DRV5056_ADDRESS               0x56          // The device i2c address

/*
 * @breif Unit: mm 
 *        Specifies the magnet length, which is used to calculate the displacement.
 */
#define MAGNET_LENGTH                 10

/*
 * @breif Unit: mT 
 *        Specifies the magnetic field strength, used to indicate the passage of 
 *        a single magnet, depending on the magnet used.
 */
#define MAGNETIC_STRENGTH_THRESHOLD   10

#define FILTER_N 50 // Mean filtered data volume.

ADC121C021 DRV5056;

void setup()
{
	pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Power on RAK12044.

  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  
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
	Serial.println("RAK12044 Linear Position Measurement Example.");

  DRV5056.configCycleTime(CYCLE_TIME_512); // Set ADC121C021 Conversion Cycle Time.
  DRV5056.setVoltageResolution(3.3);
}

void loop()
{
	int Count = 0;
	bool flag = 0; 
	while(1)
	{
		float B , D;
		B = Filter();
		Serial.print(F("Count: Distance:"));
		if( B > MAGNETIC_STRENGTH_THRESHOLD )
		{
			if(flag == 0)
			{
				Count += 1;    
			}
			flag = 1; 
			
			B = MAGNETIC_STRENGTH_THRESHOLD;
//			Serial.println( B );
			digitalWrite(LED_GREEN, HIGH);
		}
		else 
		{
			if(flag == 1)
			{
				Count += 1;    
			}
			flag = 0;
			B = 0;
//			Serial.println( B );
			digitalWrite(LED_GREEN, LOW);
		}
		D = Count * MAGNET_LENGTH ; // Write according to your actual environment.
		Serial.println( D ); // Unit: mm .
	}
}

/*
 * @breif Simple arithmetic mean filtering method.
 *        Continuously take FILTER_N sample values for arithmetic mean operation.
 */
float Filter(void) 
{
  int i;
  float sum = 0;
  for(i = 0; i < FILTER_N; i++) 
  {
    sum += Get_Date();
    delay(1); // 1KHZ
  }
  return (float)(sum / FILTER_N);
}

/*
 * @breif Get RAK12044 data.
 */
float Get_Date(void)
{
  float Vout;
  float B;
  
  Vout = DRV5056.getSensorVoltage() * 1000;
  
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
  return B ;
}
