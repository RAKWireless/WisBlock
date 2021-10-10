/**
 * @file Read_Battery_Level.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read charging level from a battery connected to the RAK5005-O base board
 * Setup and main loop
 * @version 0.1
 * @date 2020-08-21
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>       //Click here to get the library: http://librarymanager/All#u8g2

#define PIN_VBAT WB_A0

uint32_t vbat_pin = PIN_VBAT;

#define VBAT_MV_PER_LSB (0.806F) // 3.0V ADC range and 12 - bit ADC resolution = 3300mV / 4096
#define VBAT_DIVIDER (0.6F)           // 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER_COMP (1.846F)      //  // Compensation factor for the VBAT divider

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);



/**
 * @brief Get RAW Battery Voltage
 */
float readVBAT(void)
{
  unsigned int sum = 0,average_value = 0;
  unsigned int read_temp[10] = {0};
  unsigned char i = 0;
  unsigned int adc_max = 0;
  unsigned int adc_min = 4095; 
  average_value = analogRead(vbat_pin);
  for(i=0;i<10;i++)
  {
    read_temp[i] = analogRead(vbat_pin);
    if(read_temp[i] < adc_min)  
      {
        adc_min = read_temp[i];
      }
    if(read_temp[i] > adc_max)  
      {
        adc_max = read_temp[i];
      }
     sum = sum + read_temp[i];
  }
  average_value = (sum - adc_max - adc_min) >> 3; 
  Serial.printf("The ADC value is:%d\r\n",average_value);
  
  // Convert the raw value to compensated mv, taking the resistor-
  // divider into account (providing the actual LIPO voltage)
  // ADC range is 0..3300mV and resolution is 12-bit (0..4095)
  float volt = average_value * REAL_VBAT_MV_PER_LSB;
 
  Serial.printf("The battery voltage is: %3.2f V\r\n",volt);
  return volt;
}

/**
 * @brief Convert from raw mv to percentage
 * @param mvolts
 *    RAW Battery Voltage
 */
uint8_t mvToPercent(float mvolts)
{
    if (mvolts < 3300)
        return 0;

    if (mvolts < 3600)
    {
        mvolts -= 3300;
        return mvolts / 30;
    }

    mvolts -= 3600;
    return 10 + (mvolts * 0.15F); // thats mvolts /6.66666666
}

/**
 * @brief get LoRaWan Battery value
 * @param mvolts
 *    Raw Battery Voltage
 */
uint8_t mvToLoRaWanBattVal(float mvolts)
{
    if (mvolts < 3300)
        return 0;

    if (mvolts < 3600)
    {
        mvolts -= 3300;
        return mvolts / 30 * 2.55;
    }

    mvolts -= 3600;
    return (10 + (mvolts * 0.15F)) * 2.55;
}

void setup()
{
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
  Serial.println("=====================================");
  Serial.println("RAK11300 Battery test");
  Serial.println("=====================================");
   // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12); // Can be 8, 10, 12 or 14
  // Let the ADC settle
  delay(10);
  u8g2.begin();  
  // Get a single ADC sample and throw it away
  readVBAT();
}

void loop()
{
    // Get a raw ADC reading
    float vbat_mv = readVBAT();

    // Convert from raw mv to percentage (based on LIPO chemistry)
    uint8_t vbat_per = mvToPercent(vbat_mv);

    // Display the results
   char data[32] = {0};
   u8g2.clearBuffer();         // clear the internal memory
   u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font
  
   u8g2.drawStr(3, 15, "Power  voltage:");
   memset(data, 0, sizeof(data));
   
   sprintf(data,  "%.3fV \t %d%%", vbat_mv / 1000, vbat_per);
   u8g2.drawStr(3, 45, data);
   u8g2.sendBuffer(); // transfer internal memory to the display
    
    Serial.print("LIPO = ");
    Serial.print(vbat_mv);
    Serial.print(" mV (");
    Serial.print(vbat_per);
    Serial.print("%)  Battery ");
    Serial.println(mvToLoRaWanBattVal(vbat_mv));

    delay(1000);
}
