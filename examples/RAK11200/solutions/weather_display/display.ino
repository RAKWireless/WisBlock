/**
   @file display.ino
   @author rakwireless.com
   @brief function that write bme680 and weather data on oled1306
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include <U8g2lib.h> //http://librarymanager/All#U8g2
#include <Arduino_JSON.h>//http://librarymanager/All#Arduino_Json

extern double g_temperature;
extern double g_pressure;
extern double g_humidity;
extern double g_gas_resist;

extern JSONVar g_myObject;

/** u8g2 instance **/
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

/**
   @brief u8g2 init
*/
void oled_display_init()
{
  u8g2.begin();
}

/**
   @brief write bme680 and weather data on oled1306
*/
void oled_display_bme680_weather()
{
  // display indoor bme680 sensor data on OLED
  u8g2.clearBuffer();                 // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font

  char data[32] = {0};
  memset(data, 0, sizeof(data));
  snprintf(data, sizeof(data), "Indoor: T:%.1fC", g_temperature);
  u8g2.drawStr(0, 10, data);

  memset(data, 0, sizeof(data));
  snprintf(data, sizeof(data), "RH:%.1f%% P:%.1fhPa", g_humidity, g_pressure);
  u8g2.drawStr(3, 20, data);

  if (WiFi.isConnected()) {
    // display outdoor weather data on OLED
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "Outdoor: %s ", (const char*)g_myObject["weather"][0]["description"]);
    u8g2.drawStr(0, 30, data);

    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "Wind: %.1fm/s %ddeg ", (double)g_myObject["wind"]["speed"], (int)g_myObject["wind"]["deg"]);
    u8g2.drawStr(3, 40, data);

    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "T: %.1fC [ %.1fC, %.1fC ]", (double)g_myObject["main"]["temp"] - 273.15, (double)g_myObject["main"]["temp_min"] - 273.15, (double)g_myObject["main"]["temp_max"] - 273.15);
    u8g2.drawStr(3, 50, data);

    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "RH:%.1f%% P:%.1fhPa", (double)((int)g_myObject["main"]["humidity"]), (double)((int)g_myObject["main"]["pressure"]));
    u8g2.drawStr(3, 60, data);

  } else {
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "No WiFi connection!");
    u8g2.drawStr(0, 40, data);
  }
  u8g2.sendBuffer(); // transfer internal memory to the display
}
