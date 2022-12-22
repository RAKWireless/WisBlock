/**
   @file RAK14014_TFT_ST7789.ino
   @author rakwireless.com
   @brief This example is used to display Bmp pictures and text.
   @version 0.1
   @date 2022-6-28
   @copyright Copyright (c) 2022
**/
#include <Adafruit_GFX.h>			// Click here to get the library: http://librarymanager/All#Adafruit_GFX
#include <Adafruit_ST7789.h>	// Click here to get the library: http://librarymanager/All#Adafruit_ST7789
#include <./Fonts/FreeSerif9pt7b.h>  // Font file, you can include your favorite fonts.
#include <SPI.h>
#include "BMP.h"

#define CS            SS
#define BL            WB_IO3
#define RST           WB_IO5
#define DC            WB_IO4

#define WISBLOCK "WisBlock is an amazing product built by the RAK company for the \
IoT industry. Its modular approach solutions are built like clicking blocks \
together to realize your ideas making it easier to implement to low power wide \
area network (LPWAN) into your IoT solution. "

Adafruit_ST7789 tft = Adafruit_ST7789(CS, DC, RST);

void setup(void) 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  
  pinMode(BL, OUTPUT);
  digitalWrite(BL, HIGH); // Enable the backlight, you can also adjust the backlight brightness through PWM.

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

  Serial.println(F("Hello! ST7789 TFT Test"));
  tft.init(240, 320); // Init ST7789 240x240.
  tft.setRotation(3);
  Serial.println(F("Initialized"));
}

void loop() 
{
  tft.fillScreen(ST77XX_WHITE);
  drawBmp(&bmdemo,0,0);
  delay(1000);
  tft.fillScreen(ST77XX_WHITE);
  testdrawtext(WISBLOCK,ST77XX_BLACK);
  delay(10000);
}

static void testdrawtext(char *text, uint16_t color) 
{
  tft.setTextSize(0);
  tft.setFont(&FreeSerif9pt7b); 
  tft.setCursor(20, 50);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

static void drawBmp(const GUI_BITMAP *bmp , uint8_t x, uint8_t y) 
{
  uint16_t color = bmp->date[0];
  uint32_t count = 0;
  uint64_t bufSize = bmp->xSize * bmp->ySize;
  tft.startWrite();
  tft.setAddrWindow(x, y, bmp->xSize, bmp->ySize);

  for ( uint64_t i = 0 ; i < bufSize ; i++ ) 
  {
    if(color == bmp->date[i])
    {
      count++;
    }
    else
    {
      tft.writeColor(color,count); 
      count = 1;
      color = bmp->date[i];
    }
  }
  tft.writeColor(color,count); 
  tft.endWrite();
}
