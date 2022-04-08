/**
   @file RAK14012_RGB_Matrix_SingleCycle.ino
   @author rakwireless.com
   @brief Light up the LED for a single cycle.
   @version 0.1
   @date 2021-11-29
   @copyright Copyright (c) 2021
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

#include <Rak_RGB_Matrix.h> // Click to install library: http://librarymanager/All#RAK14012-LED-Matrix

#define DATA_PIN        WB_IO5 
#define NUMPIXELS       256   

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
RAK_RGB_Matrix pixels(NUMPIXELS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() 
{
  // Enable 5v power supply on the board.
  // An external battery is required.
  pinMode(WB_IO6, OUTPUT);
  digitalWrite(WB_IO6, HIGH); 
  
  // Initialize Serial for debug output
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

  pixels.begin();
}

void loop() 
{
  for(int i = 0; i < 256; i++)
  { 
    pixels.setBrightness(i);    
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
    pixels.show();
    delay(1);
    pixels.clear();
    pixels.show();
    delay(1);        
  }
}
