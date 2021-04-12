/**
   @file weather_display.ino
   @author rakwireless.com
   @brief Use nrf_connect phone app set wifi ssid and psk.
          Get city weather from openWeather.org by http api.
		  Display BME680 data and Weather on oled SSD1306
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/


#include <Arduino_JSON.h>//http://librarymanager/All#Arduino_Json
#include <WiFi.h>

extern void bme680_init();
extern uint32_t bme680_read_data(double& temperature, double& pressure, double& humidity, double& gas_resist);
double g_temperature;
double g_pressure;
double g_humidity;
double g_gas_resist;

void oled_display_bme680_weather();

extern int32_t get_current_weather_by_city_name(String city, String payload);
extern void decode_weather(char *payload, JSONVar& myObject);
char *g_city = "Xi'an";

JSONVar g_myObject;

extern bool got_ip;

void setup()
{
  // wifi led init
  pinMode(WB_LED1, OUTPUT);

  // ble led init
  pinMode(WB_LED2, OUTPUT);

  // ble start advertisement
  ble_service_start();

  // wifi init
  wifi_init();

  oled_display_init();

  // sntp client init
  configTzTime("UTC-8:00", "0.asia.pool.ntp.org", "1.asia.pool.ntp.org", "2.asia.pool.ntp.org");

  // bme680 sensor init
  bme680_init();
}

void loop()
{
  int32_t ret;
  char weather_data[512] = {0};

  ret = bme680_read_data(g_temperature, g_pressure, g_humidity, g_gas_resist);
  if (ret != -1)
  {
    Serial.println("BME680 Data:");
    Serial.printf("\tT:%.1f C\n", g_temperature);
    Serial.printf("\tRH:%.1f%%\n", g_humidity);
    Serial.printf("\tP:%.1f hPa\n", g_pressure);
    Serial.printf("\tG:%.1f KOhms\n", g_gas_resist);
  }

  if (WiFi.isConnected())
  {
    if (got_ip)
    {

      update_characteristic();
      got_ip = false;
    }

    printLocalTime();

    ret = get_current_weather_by_city_name(g_city, weather_data, sizeof(weather_data));
    if (ret != -1)
    {
      // Serial.println(weather_data);

      decode_weather(weather_data, g_myObject);

      Serial.println("Weather Data:");
      Serial.printf("\tWeather:%s %s\n", (const char *)g_myObject["weather"][0]["main"], (const char *)g_myObject["weather"][0]["description"]);
      Serial.printf("\tT:%.1f C, Max=%.1f C, Min=%.1f C\n",
                    (double)g_myObject["main"]["temp"] - 273.15, (double)g_myObject["main"]["temp_min"] - 273.15, (double)g_myObject["main"]["temp_min"] - 273.15);
      Serial.printf("\tRH:%d%%\n", (int)g_myObject["main"]["humidity"]);
      Serial.printf("\tP:%d hPa\n", (int)g_myObject["main"]["pressure"]);

      Serial.printf("\tWind: %.1f m/s %d degree\n", (double)g_myObject["wind"]["speed"], (int)g_myObject["wind"]["deg"]);
    }

  }

  oled_display_bme680_weather();
  delay(20000);
}
