
#include "WiFi.h"

#include <ArduinoJson.h>    // library
#include <HTTPClient.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>   // library
#include <Adafruit_EPD.h>   // library

#define POWER_ENABLE   WB_IO2
#define EPD_MOSI       MOSI
#define EPD_MISO       -1     // not use
#define EPD_SCK        SCK
#define EPD_CS         SS
#define EPD_DC         WB_IO1
#define SRAM_CS        -1     // not use
#define EPD_RESET      -1     // not use
#define EPD_BUSY       WB_IO4

#define LEFT_BUTTON    WB_IO3
#define MIDDLE_BUTTON  WB_IO5
#define RIGHT_BUTTON   WB_IO6

#define BTC 1
#define ETH 2
#define HNT 3

uint8_t gKeyNum = 1; // which button is pressed. Default will be Bitcoin!

char wifiPassword[] = "YOUR WIFI PASSWORD";
char wifiSSID[] = "YOUR WIFI ROUTER";

char btc_usd_buffer[8];
char btc_eur_buffer[8];
char btc_chnge_buffer[8];
char eth_usd_buffer[8];
char eth_eur_buffer[8];
char eth_chnge_buffer[8];
char hnt_usd_buffer[8];
char hnt_eur_buffer[8];
char hnt_chnge_buffer[8];

char token_selected = 1;

unsigned long previousMillis = 0;       
const long interval = 60000;           // interval at which to update the values (milliseconds)

// 2.13" EPD with SSD1680, width=250 pixels, heigh=122 pixels
Adafruit_SSD1680 display(250, 122, EPD_MOSI,
                         EPD_SCK, EPD_DC, EPD_RESET,
                         EPD_CS, SRAM_CS, EPD_MISO,
                         EPD_BUSY);

// left button interrupt handle function
void interruptHandle1()
{
  if (gKeyNum == 0)
  {
    gKeyNum = 1;
  }
}

// middle button interrupt handle function
void interruptHandle2()
{
  if (gKeyNum == 0)
  {
    gKeyNum = 2;
  }
}

// right button interrupt handle function
void interruptHandle3()
{
  if (gKeyNum == 0)
  {
    gKeyNum = 3;
  }
}

void testdrawtext(int16_t x, int16_t y, char *text, uint16_t text_color, uint32_t text_size)
{
  display.setCursor(x, y);
  display.setTextColor(text_color);
  display.setTextSize(text_size);
  display.setTextWrap(true);
  display.print(text);
}

void setup() {

  pinMode(POWER_ENABLE, INPUT_PULLUP);
  digitalWrite(POWER_ENABLE, HIGH);

  Serial.begin(115200);
  time_t timeout = millis();
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

  pinMode(WB_LED1, OUTPUT);     // On board LED in RAK5005-O you can use.

  Serial.print("Connecting to Wifi");
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay (1250);
  }

  // set left button interrupt
  pinMode(LEFT_BUTTON, INPUT);
  attachInterrupt(LEFT_BUTTON, interruptHandle1, FALLING);

  // set middle button interrupt
  pinMode(MIDDLE_BUTTON, INPUT);
  attachInterrupt(MIDDLE_BUTTON, interruptHandle2, FALLING);

  // set right button interrupt
  pinMode(RIGHT_BUTTON, INPUT);
  attachInterrupt(RIGHT_BUTTON, interruptHandle3, FALLING);

  Serial.println("RAK11200 Crypto Tracker");

  display.begin(false);

  display.clearBuffer();
  testdrawtext(50, 30, "WisBlock", (uint16_t)EPD_RED, 3);
  testdrawtext(40, 60, "Crypto Tracker", (uint16_t)EPD_BLACK, 2);
  display.display(true);

  delay (10000);
}


void loop()
{
  unsigned long currentMillis = millis();

  // BTC
  if (gKeyNum == 1)
  {
    token_selected = 1;
    Serial.println("Left button pressed");
  }
  // ETH
  else if (gKeyNum == 2)
  {
    token_selected = 2;
    Serial.println("Middle button pressed");
  }
  // HNT
  else if (gKeyNum == 3)
  {
    token_selected = 3;
    Serial.println("Right button pressed");
  }

  if ((currentMillis - previousMillis >= interval) || (gKeyNum != 0))
  {
    gKeyNum = 0;
    previousMillis = currentMillis;
    if ((WiFi.status() == WL_CONNECTED))
    {

      HTTPClient http;
      
      Serial.println ("Get COINGECKO API");
      http.begin ("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin%2Cethereum%2Chelium&vs_currencies=eur%2Cusd&include_24hr_change=true");
      int httpCode = http.GET();

      if (httpCode > 0) {

      }

      Serial.println ("\nStatuscode: " + String(httpCode));
      delay (100);

      const size_t capacity = JSON_OBJECT_SIZE(2) + 2 * JSON_OBJECT_SIZE(6) + 150;
      DynamicJsonDocument doc(capacity);
      String payload = http.getString();
      const char* json = payload.c_str();

      deserializeJson(doc, json);

      JsonObject bitcoin = doc["bitcoin"];
      float bitcoin_eur = bitcoin["eur"]; // 9473.3
      float bitcoin_eur_24h_change = bitcoin["eur_24h_change"]; // 11.379516678954898
      float bitcoin_usd = bitcoin["usd"]; // 11140.6
      float bitcoin_usd_24h_change = bitcoin["usd_24h_change"]; // 12.464050392648252

      JsonObject ethereum = doc["ethereum"];
      float ethereum_eur = ethereum["eur"]; // 276.02
      float ethereum_eur_24h_change = ethereum["eur_24h_change"]; // 3.5689620753981264
      float ethereum_usd = ethereum["usd"]; // 324.6
      float ethereum_usd_24h_change = ethereum["usd_24h_change"]; // 4.577442219792744

      JsonObject helium = doc["helium"];
      float helium_eur = helium["eur"]; // 9473.3
      float helium_eur_24h_change = helium["eur_24h_change"]; // 11.379516678954898
      float helium_usd = helium["usd"]; // 11140.6
      float helium_usd_24h_change = helium["usd_24h_change"]; // 12.464050392648252

      http.end();

      if (token_selected == BTC)
      {
        display.clearBuffer();
        testdrawtext(40, 5, "Crypto Tracker", (uint16_t)EPD_RED, 2);
        testdrawtext(90, 30, "BTC", (uint16_t)EPD_RED, 3);
        testdrawtext(25, 60, "USD: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 80, "EUR: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 100, "%Change:", (uint16_t)EPD_BLACK, 2);
        dtostrf(bitcoin_usd, 6, 2, btc_usd_buffer);
        testdrawtext(130, 60, btc_usd_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(bitcoin_eur, 6, 2, btc_eur_buffer);
        testdrawtext(130, 80, btc_eur_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(bitcoin_usd_24h_change, 6, 2, btc_chnge_buffer);
        testdrawtext(130, 100, btc_chnge_buffer, (uint16_t)EPD_BLACK, 2);
        display.display(true);
      }
      else if (token_selected == ETH)
      {
        display.clearBuffer();
        testdrawtext(40, 5, "Crypto Tracker", (uint16_t)EPD_RED, 2);
        testdrawtext(90, 30, "ETH", (uint16_t)EPD_RED, 3);
        testdrawtext(25, 60, "USD: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 80, "EUR: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 100, "%Change:", (uint16_t)EPD_BLACK, 2);
        dtostrf(ethereum_usd, 6, 2, eth_usd_buffer);
        testdrawtext(130, 60, eth_usd_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(ethereum_eur, 6, 2, eth_eur_buffer);
        testdrawtext(130, 80, eth_eur_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(ethereum_usd_24h_change, 6, 2, eth_chnge_buffer);
        testdrawtext(130, 100, eth_chnge_buffer, (uint16_t)EPD_BLACK, 2);
        display.display(true);
      }
      else if (token_selected == HNT)
      {
        display.clearBuffer();
        testdrawtext(40, 5, "Crypto Tracker", (uint16_t)EPD_RED, 2);
        testdrawtext(90, 30, "HNT", (uint16_t)EPD_RED, 3);
        testdrawtext(25, 60, "USD: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 80, "EUR: ", (uint16_t)EPD_BLACK, 2);
        testdrawtext(25, 100, "%Change:", (uint16_t)EPD_BLACK, 2);
        dtostrf(helium_usd, 6, 2, hnt_usd_buffer);
        testdrawtext(130, 60, hnt_usd_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(helium_eur, 6, 2, hnt_eur_buffer);
        testdrawtext(130, 80, hnt_eur_buffer, (uint16_t)EPD_BLACK, 2);
        dtostrf(helium_usd_24h_change, 6, 2, hnt_chnge_buffer);
        testdrawtext(130, 100, hnt_chnge_buffer, (uint16_t)EPD_BLACK, 2);
        display.display(true);
      }

      Serial.println("-------------------------------");

      //BITCOIN
      Serial.print("EUR: ");
      Serial.println(bitcoin_eur);
      Serial.print("USD: ");
      Serial.println(bitcoin_usd);
      Serial.print("USD 24hr %: ");
      Serial.println(bitcoin_usd_24h_change);

      //ETHEREUM
      Serial.print("EUR: ");
      Serial.println(ethereum_eur);
      Serial.print("USD: ");
      Serial.println(ethereum_usd);
      Serial.print("USD 24hr %: ");
      Serial.println(ethereum_usd_24h_change);

      //HELIUM
      Serial.print("EUR: ");
      Serial.println(helium_eur);
      Serial.print("USD: ");
      Serial.println(helium_usd);
      Serial.print("USD 24hr %: ");
      Serial.println(helium_usd_24h_change);

      Serial.println("-------------------------------");
    }
  }
}
