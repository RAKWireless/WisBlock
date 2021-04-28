
/**
   @file RAK1902_Pressure_LPS22HB.ino
   @author rakwireless.com
   @brief Setup and read values from a lps22hb sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/


#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>  // Click here to get the library: http://librarymanager/All#Adafruit_LPS2X

Adafruit_LPS22 g_lps22hb;

void setup(void) {
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

  Serial.println("Adafruit LPS22 test!");

  // Try to initialize!
  if (!g_lps22hb.begin_I2C(0x5c)) 
  {
    Serial.println("Failed to find LPS22 chip");
    while (1) 
    { 
      delay(10); 
    }
  }

  Serial.println("LPS22 Found!");

  g_lps22hb.setDataRate(LPS22_RATE_10_HZ);
  Serial.print("Data rate set to: ");

  switch (g_lps22hb.getDataRate()) 
  {
    case LPS22_RATE_ONE_SHOT: Serial.println("One Shot / Power Down"); 
      break;
    case LPS22_RATE_1_HZ: Serial.println("1 Hz"); 
      break;
    case LPS22_RATE_10_HZ: Serial.println("10 Hz"); 
      break;
    case LPS22_RATE_25_HZ: Serial.println("25 Hz"); 
      break;
    case LPS22_RATE_50_HZ: Serial.println("50 Hz"); 
      break;

  }
}

void loop() {
  sensors_event_t temp;
  sensors_event_t pressure;
  g_lps22hb.getEvent(&pressure, &temp);
  Serial.print("Temperature: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.print("Pressure: ");Serial.print(pressure.pressure);Serial.println(" hPa");
  Serial.println("");
  delay(1000);
}
