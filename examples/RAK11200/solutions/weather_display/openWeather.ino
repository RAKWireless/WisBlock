/**
   @file openWeather.ino
   @author rakwireless.com
   @brief get city weather from openweathermap.org
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <HTTPClient.h>

/** After registering openweather account, you can get your API key. **/
const char *g_apiKey = "<PUT YOUR WEATHERMAP API KEY HERE>";

/**
   @brief  get city weather by openWeather api
   @param  city: in param, the city you want to query, like "London"
           payload: array to store query result
		   payload_len: length of array
*/
int32_t get_current_weather_by_city_name(char *city, char *payload, uint32_t payload_len)
{
  HTTPClient http;
  int32_t result = -1;
  char api[256] = "";
  String weather;

  // API Documnet: https://openweathermap.org/current
  char *apiFmt = "http://api.openweathermap.org/data/2.5/weather?q=%s&APPID=%s";

  if (city == NULL || payload == NULL || payload_len <= 0)
  {
    return -1;
  }

  if (strlen(apiFmt) - 4 + strlen(city) + strlen(g_apiKey) > 128)
  {
    return -1;
  }

  snprintf(api, sizeof(api), apiFmt, city, g_apiKey);
  // Serial.println(api);

  http.begin(api);
  int httpCode = http.GET();
 
  if (httpCode > 0 && httpCode < 400) { //Check for the returning code
    result = httpCode;
    weather = http.getString();
    // Serial.println(weather);

    strncpy(payload, weather.c_str(), payload_len - 1);
    payload[payload_len - 1] = 0;
  }
  else
  {
    Serial.println("Get Weather information from openweathermap.org Failed!");
    Serial.println("Please check if your weathermap api key is right.");
  }


  http.end(); //Free the resources

  return result;
}

/**
   @brief  decode payload from api of openWeather
   @param  myObject: object to store the decode result
*/
void decode_weather(char *payload, JSONVar& myObject)
{
  if (payload == NULL)
  {
    return;
  }

  myObject = JSON.parse(payload);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
  {
    Serial.println("Parsing received data failed!");
    return;
  }
}
