/**
   @file wifi.ino
   @author rakwireless.com
   @brief function of WiFi
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <Arduino_JSON.h> //http://librarymanager/All#Arduino_Json
#include <WiFi.h>

/** Unique device name */
extern char g_devName[];

// ping test
void ping_network_address(char *addr);

/**
   @brief  set wifi ssid and psk and begin wifi
   @param  jsonStr: string contian ssid and psk, format is {"ssid":"wifi ssid","psk":"wifi psk"}
           len: length of jsonStr
*/
void wifi_config_set(char *jsonStr, uint32_t len)
{
  if (jsonStr == NULL)
  {
    return;
  }

  // Decode data
  int key_index = 0;
  for (int index = 0; index < len; index++)
  {
    jsonStr[index] = (char)jsonStr[index] ^ (char)g_devName[key_index];
    key_index++;
    if (key_index >= strlen(g_devName))
      key_index = 0;
  }

  Serial.printf("Decoded message: %s\n", jsonStr);

  /** parse received data  begin **/
  // the msg should be like {"ssidPrim":"wifi ssid","pwPrim":"wifi psk","ssidSec":"wifi ssid","pwSec":"wifi psk"}, if not, discard it.
  JSONVar myObject = JSON.parse(jsonStr);

  // JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined")
  {
    Serial.println("Parsing received data failed!");
    return;
  }

  // myObject.hasOwnProperty(key) checks if the object contains an entry for key
  if (myObject.hasOwnProperty("ssidPrim"))
  {
    Serial.print("myObject[\"ssidPrim\"] = ");
    Serial.println(myObject["ssidPrim"]);
  }
  else
  {
    Serial.println("the msg should be like {\"ssidPrim\":\"wifi ssid\",\"pwPrim\":\"wifi psk\",\"ssidSec\":\"wifi ssid\",\"pwSec\":\"wifi psk\"}!");
    return;
  }
  if (myObject.hasOwnProperty("pwPrim"))
  {
    Serial.print("myObject[\"pwPrim\"] = ");
    Serial.println(myObject["pwPrim"]);
  }
  else
  {
    Serial.println("the msg should be like {\"ssidPrim\":\"wifi ssid\",\"pwPrim\":\"wifi psk\",\"ssidSec\":\"wifi ssid\",\"pwSec\":\"wifi psk\"}!");
    return;
  }
  /** parse received data  end **/

  // if wifi connected, disconnect it
  if (WiFi.isConnected())
  {
    WiFi.disconnect();
    delay(100);
  }
  // connect wifi with new ssid and psk
  WiFi.begin(myObject["ssidPrim"], myObject["pwPrim"]);
}

/**
   @brief  get wifi ssid and psk
   @param  wifi_config: array which store ssid and psk
           len: length of wifi_config array
*/
uint32_t wifi_config_get(char* wifi_config, uint32_t len)
{
  if (wifi_config == NULL || len < 32)
  {
    return 0;
  }

  char *fmt = "{\"ssidPrim\":\"%s\",\"pwPrim\":\"%s\",\"ssidSec\":\"%s\",\"pwSec\":\"%s\"}";

  int newLen = 0;

  if (WiFi.isConnected())
  {
    Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("PSK: %s\n", WiFi.psk().c_str());
    // Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    newLen = snprintf(wifi_config, len, fmt,  (char *)WiFi.SSID().c_str(), (char *)WiFi.psk().c_str(), (char *)WiFi.SSID().c_str(), (char *)WiFi.psk().c_str());
  } else {
    newLen = snprintf(wifi_config, len, "{\"ssidPrim\":\"\",\"pwPrim\":\"\",\"ssidSec\":\"\",\"pwSec\":\"\"}");
  }
  Serial.printf("Len: %d String: %s\n", newLen, wifi_config);

  // encode the data
  int key_index = 0;
  for (int index = 0; index < newLen; index++)
  {
    wifi_config[index] = (char)wifi_config[index] ^ (char)g_devName[key_index];
    key_index++;
    if (key_index >= strlen(g_devName))
      key_index = 0;
  }
  return newLen;
}

bool got_ip = false;

/**
   @brief  callback when wifi got ip, and ping 8.8.8.8
   @param  event: event type
           info: even info
*/
void wifi_got_ip_callback(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(IPAddress(info.got_ip.ip_info.ip.addr));
  // green light on
  digitalWrite(WB_LED1, HIGH);

  // ping address
  ping_network_address("8.8.8.8");
}

/**
   @brief  callback when wifi disconnect
   @param  event: event type
           info: even info
*/
void wifi_disconnect_callback(WiFiEvent_t event, WiFiEventInfo_t info)
{
  //  Serial.println("WiFi disconnected");
  // light off
  digitalWrite(WB_LED1, LOW);
}

/**
   @brief  set callback events and begin wifi with last config.
*/
void wifi_init()
{
  // set event function for specific event
  WiFi.onEvent(wifi_got_ip_callback, WiFiEvent_t::SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(wifi_disconnect_callback, WiFiEvent_t::SYSTEM_EVENT_STA_DISCONNECTED);
  // start wifi with stored wifi config
  WiFi.begin();
}
