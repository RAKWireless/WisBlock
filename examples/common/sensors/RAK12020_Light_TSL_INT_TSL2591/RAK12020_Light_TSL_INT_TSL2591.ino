 /**
   @file RAK12020_Light_TSL_INT_TSL2591.ino
   @author rakwireless.com
   @brief   This example shows how the interrupt system on the TLS2591.
						Can be used to detect a meaningful change in light levels.
   @version 0.1
   @date 2022-03-15
   @copyright Copyright (c) 2022
**/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h" // Click to install library: http://librarymanager/All#Adafruit_TSL2591

#define THRESHOLD_LOWER               (100)                 // Interrupt thresholds.
#define THRESHOLD_UPPER               (1500)

#define TLS2591_INT_PERSIST           (TSL2591_PERSIST_ANY) // Fire on any valid change.

#define INT_PIN                       WB_IO1                // Mount in SLOT A.
//#define INT_PIN                       WB_IO3                // Mount in SLOT C.
//#define INT_PIN                       WB_IO5                // Mount in SLOT D.

Adafruit_TSL2591 TSL2591 = Adafruit_TSL2591(2591);

volatile uint8_t g_intFlag = 0;

void INT_call_back()
{
  g_intFlag = 1;
}

void setup(void)
{
	// Sensor power on.
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); 
   
  // Initialize Serial for debug output.
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

  Serial.println("Starting RAK12020 TSL2591 interrupt Test!");

  if (TSL2591.begin())
  {
    Serial.println("Found a TSL2591 sensor");
  }
  else
  {
    while (1)
    {
      Serial.println("No sensor found ... check your wiring?");
      delay(1000);
    }
  }

  // Display some basic information on this sensor.
  sensor_t sensor;
  TSL2591.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); 
  Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); 
  Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); 
  Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); 
  Serial.print  (sensor.max_value); 
  Serial.println(" lux");
  Serial.print  ("Min Value:    "); 
  Serial.print  (sensor.min_value); 
  Serial.println(" lux");
  Serial.print  ("Resolution:   "); 
  Serial.print  (sensor.resolution, 4); 
  Serial.println(" lux");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);

  /*
   * @brief You can change the gain on the fly, to adapt to brighter/dimmer light situations.
   * @param
   *    TSL2591_GAIN_LOW    1x    gain
   *    TSL2591_GAIN_MED    25x   gain
   *    TSL2591_GAIN_HIGH   428x  gain
   *    TSL2591_GAIN_MAX    9876x gain
   */
  TSL2591.setGain(TSL2591_GAIN_MED);  // 25x gain.

  /*
   * @brief Changing the integration time gives you a longer time over which to sense light
   *        longer timelines are slower, but are good in very low light situtations!
   * @param
   *    TSL2591_INTEGRATIONTIME_100MS     shortest integration time (bright light)
   *    TSL2591_INTEGRATIONTIME_200MS     
   *    TSL2591_INTEGRATIONTIME_300MS     
   *    TSL2591_INTEGRATIONTIME_400MS     
   *    TSL2591_INTEGRATIONTIME_500MS     
   *    TSL2591_INTEGRATIONTIME_600MS     longest integration time (dim light)
   */
  TSL2591.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)

  /*
   * @brief Setup the SW interrupt to trigger between 100 and 1500 lux.
   *        Threshold values are defined at the top of this sketch.
   */
  TSL2591.clearInterrupt();
  TSL2591.registerInterrupt(THRESHOLD_LOWER,THRESHOLD_UPPER,TLS2591_INT_PERSIST);

  pinMode(INT_PIN,INPUT);  // Connect with INT pin.
  attachInterrupt(digitalPinToInterrupt(INT_PIN) ,INT_call_back ,FALLING);  // Enable interrupts.
  
  Serial.print("Interrupt Threshold Window: ");
  Serial.print(THRESHOLD_LOWER, DEC);
  Serial.print(" to ");
  Serial.println(THRESHOLD_UPPER, DEC);  
}

void loop(void)
{
  uint32_t lum = TSL2591.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  Serial.print("IR: "); 
  Serial.print(ir);  
  Serial.print("  ");
  Serial.print("Full: "); 
  Serial.print(full); 
  Serial.print("  ");
  Serial.print("Visible: "); 
  Serial.print(full - ir); 
  Serial.print("  ");
  Serial.print("Lux: "); 
  Serial.println(TSL2591.calculateLux(full, ir));

  if( g_intFlag == 1)
  {
    g_intFlag = 0;
    uint8_t status = TSL2591.getStatus();
  
    if (status & 0x10) 
    {
      Serial.println("ALS Interrupt occured.");
      Serial.print("Status: ");
      Serial.println(status, BIN);
    }
    TSL2591.clearInterrupt();
  }
	
  delay(500);
}
