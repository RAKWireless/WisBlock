/**
 *  @file RAK13000_WirelessCharger.ino
 *  @author rakwireless.com
 *  @brief show the wireless charger power voltage by the oled
 *  @version 0.1
 *  @date 2021-02-23
 *  @copyright Copyright (c) 2020
 *  @note platform RAK4631
**/
#include <Wire.h>
#include <U8g2lib.h>       //Click here to get the library: http://librarymanager/All#u8g2

#define LED1  35 
#define LED2  36 

#define PIN_VBAT A0 //Analog pin to read battery level definition
/** Millivolt per LSB constant value = 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096 */
#define VBAT_MV_PER_LSB (0.73242188F)
/** Voltage divider constant = 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M)) */
#define VBAT_DIVIDER (0.6F)
/** Compensation factor for the VBAT divider */
#define VBAT_DIVIDER_COMP (1.74)
/** Formula to calculate real battery voltage */
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

float g_powerVoltage = 0;   

void oled_show();
void readVBAT_init();
void get_BATvoltage();
void led1_on();
void led1_off();
void led2_on();
void led2_off();

void setup()
{
  time_t timeout = millis();
  readVBAT_init();
  u8g2.begin();
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
  pinMode(LED1, OUTPUT);  
  pinMode(LED2, OUTPUT);   
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void loop()
{  
  led1_on();
  led2_off();
  get_BATvoltage();
  oled_show();
  delay(200);
  led2_on();
  led1_off();
  delay(200);
}
void led1_on()
{
  digitalWrite(LED1, HIGH);
}
void led1_off()
{
  digitalWrite(LED1, LOW);
}
void led2_on()
{
  digitalWrite(LED2, HIGH);
}
void led2_off()
{
  digitalWrite(LED2, LOW);
}
void oled_show()
{
  char data[32] = {0};
  u8g2.clearBuffer();         // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font
  
  u8g2.drawStr(3, 15, "Power  voltage:");
  memset(data, 0, sizeof(data));
  sprintf(data, "%3.2f V",g_powerVoltage);
  u8g2.drawStr(3, 45, data);
  u8g2.sendBuffer(); // transfer internal memory to the display
}
void readVBAT_init()
{
  // Set the analog reference to 3.0V (default = 3.6V)
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12); // Can be 8, 10, 12 or 14
  // Let the ADC settle
  delay(10);
}
void get_BATvoltage()
{
  unsigned int sum = 0,average_value = 0;
  unsigned int read_temp[10] = {0};
  unsigned char i = 0;
  unsigned int adc_max = 0;
  unsigned int adc_min = 4095; 
  average_value = analogRead(PIN_VBAT);
  for(i=0;i<10;i++)
  {
    read_temp[i] = analogRead(PIN_VBAT);
    if(read_temp[i] < adc_min)  
      {
        adc_min = read_temp[i];
      }
    if(read_temp[i] > adc_max)  
      {
        adc_max = read_temp[i];
      }
     sum = sum + read_temp[i];
//     Serial.println(read_temp[i]);
//     delay(1);
  }
  average_value = (sum - adc_max - adc_min) >> 3; 
//  Serial.println(average_value);
  g_powerVoltage = average_value * REAL_VBAT_MV_PER_LSB * 0.001;  
  Serial.print("The battery voltage is:");
  Serial.print(g_powerVoltage,2);
  Serial.println(" V");
} 
