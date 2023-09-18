 /**
   @file RAK12020_Light_TSL_Read_TSL2591.ino
   @author rakwireless.com
   @brief  Read TSL2591 data and output through serial port.
   @version 0.1
   @date 2022-02-25
   @copyright Copyright (c) 2022
**/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h" // Click to install library: http://librarymanager/All#Adafruit_TSL2591

Adafruit_TSL2591 TSL2591 = Adafruit_TSL2591(2591);

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

  Serial.println("Starting RAK12020 TSL2591 Read Test!");

  if (TSL2591.begin())
  {
    Serial.println("Found a TSL2591 sensor.");
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
  TSL2591.setGain(TSL2591_GAIN_MED);

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
  TSL2591.setTiming(TSL2591_INTEGRATIONTIME_100MS);
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
  
  delay(500);
}
