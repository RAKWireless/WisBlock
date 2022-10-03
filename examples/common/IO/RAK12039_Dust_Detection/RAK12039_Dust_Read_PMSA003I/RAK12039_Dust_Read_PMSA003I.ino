 /**
   @file RAK12039_Dust_Read_PMSA003I.ino
   @author rakwireless.com
   @brief  Read PMSA003I data and output through serial port.
   @version 0.1
   @date 2022-01-07
   @copyright Copyright (c) 2022
**/

#include "RAK12039_PMSA003I.h"

RAK_PMSA003I PMSA003I;

/*
 * @brief WB_IO6 is connected to the SET pin.
 *        Set pin/TTL level @3.3V, high level or suspending is normal working status.
 *        while low level is sleeping mode.
 */
#define SET_PIN   WB_IO6

void setup() 
{
  // Sensor power on.
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); 

  pinMode(SET_PIN, OUTPUT);
  digitalWrite(SET_PIN, HIGH);  
   
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

  Wire.begin();
  if(!PMSA003I.begin()) 
  {
    Serial.println("PMSA003I begin fail,please check connection!");
    delay(100);
    while(1);
  }
}

void loop() 
{
  PMSA_Data_t data;
  
  if (PMSA003I.readDate(&data)) 
  {
    Serial.println("PMSA003I read date success.");

    Serial.println("Standard particulate matter:");
    Serial.print("PM1.0: "); 
    Serial.print(data.pm10_standard);
    Serial.println(" [µg/𝑚3]"); 
    
    Serial.print("PM2.5: "); 
    Serial.print(data.pm25_standard);
    Serial.println(" [µg/𝑚3]"); 
    
    Serial.print("PM10 : "); 
    Serial.print(data.pm100_standard);
    Serial.println(" [µg/𝑚3]"); 

    Serial.println("Atmospheric environment:");
    Serial.print("PM1.0: "); 
    Serial.print(data.pm10_env);
    Serial.println(" [µg/𝑚3]");
     
    Serial.print("PM2.5: "); 
    Serial.print(data.pm25_env);
    Serial.println(" [µg/𝑚3]"); 
    
    Serial.print("PM10 : "); 
    Serial.print(data.pm100_env);
    Serial.println(" [µg/𝑚3]");

    Serial.println("The number of particles in 0.1L air (above diameter):");
    Serial.print("0.3um:"); 
    Serial.println(data.particles_03um);
    Serial.print("0.5um:"); 
    Serial.println(data.particles_05um);
    Serial.print("1.0um:"); 
    Serial.println(data.particles_10um);
    Serial.print("2.5um:"); 
    Serial.println(data.particles_25um);
    Serial.print("5.0um:"); 
    Serial.println(data.particles_50um);
    Serial.print("10 um:"); 
    Serial.println(data.particles_100um);
  }
  else
  {
    Serial.println("PMSA003I read failed!");
  }
  Serial.println();
  delay(1000);
}
