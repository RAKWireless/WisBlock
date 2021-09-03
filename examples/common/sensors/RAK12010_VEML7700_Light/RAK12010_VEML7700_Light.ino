/**
   @file RAK12010_VEML7700_Light.ino
   @author rakwireless.com
   @brief Measure ambient light.
   @version 0.1
   @date 2021-7-15
   @copyright Copyright (c) 2021
**/
#include "Light_VEML7700.h"     ////http://librarymanager/All#Light_veml7700

Light_VEML7700 VMEL = Light_VEML7700();

void setup()
{
  // Initialize Serial for debug output

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
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
  if (!VMEL.begin())
  {
    Serial.println("Sensor not found");
    while (1);
  }

  VMEL.setGain(VEML7700_GAIN_1);
  VMEL.setIntegrationTime(VEML7700_IT_800MS);

  Serial.print(F("Gain: "));
  switch (VMEL.getGain())
  {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
  }

  Serial.print(F("Integration Time (ms): "));
  switch (VMEL.getIntegrationTime())
  {
    case VEML7700_IT_25MS: Serial.println("25"); break;
    case VEML7700_IT_50MS: Serial.println("50"); break;
    case VEML7700_IT_100MS: Serial.println("100"); break;
    case VEML7700_IT_200MS: Serial.println("200"); break;
    case VEML7700_IT_400MS: Serial.println("400"); break;
    case VEML7700_IT_800MS: Serial.println("800"); break;
  }

  //veml.powerSaveEnable(true);
  //veml.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);

  VMEL.setLowThreshold(10000);
  VMEL.setHighThreshold(20000);
  VMEL.interruptEnable(true);
}

void loop()
{
  Serial.print("Lux: ");
  Serial.println(VMEL.readLux());
  Serial.print("White: ");
  Serial.println(VMEL.readWhite());
  Serial.print("Raw ALS: ");
  Serial.println(VMEL.readALS());
  uint16_t irq = VMEL.interruptStatus();
  if (irq & VEML7700_INTERRUPT_LOW)
  {
    Serial.println("Low threshold");
  }
  if (irq & VEML7700_INTERRUPT_HIGH)
  {
    Serial.println("High threshold");
  }
  delay(500);
}
