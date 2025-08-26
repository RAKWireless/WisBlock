#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <Ticker.h>
#include <ArduinoOTA.h>
#include <WiFiUdp.h>
#include <ESPmDNS.h>

// Includes for Preferences
#include <Preferences.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <ArduinoJson.h>

#include "myLog.h"

#include <NeoPixelBus.h>

void initWiFi(void);
void createName();
extern WiFiMulti wifiMulti;
extern bool connStatusChanged;
extern bool isConnected;
extern String ssidPrim;
extern String ssidSec;
extern String pwPrim;
extern String pwSec;

void initBLE();
extern char apName[];

void startOTA(void);
extern boolean otaStarted;

/** WiFiUDP class for NTP server */
extern WiFiUDP ntpUDP;
bool initNTP(void);
void tryGetTime(void);
extern struct tm timeinfo;
extern bool ntpOk;

void initClock(void);
void stopClock();
void updateClock(void);
void setNumber(uint8_t value, uint8_t digit);
void setDay(uint8_t day);
void setColon(void);
void setBrightness(bool lightsOn);
void setWiFiStatus(void);
void setWiFiError(bool error);
void showMotion(void);
void clearStrip(void);
void showOTA(uint16_t pixel);

void initRadar(void);
void stopRadar(void);
void radarCDS(void);
extern boolean cdsStatus;
extern boolean radarTriggered;
