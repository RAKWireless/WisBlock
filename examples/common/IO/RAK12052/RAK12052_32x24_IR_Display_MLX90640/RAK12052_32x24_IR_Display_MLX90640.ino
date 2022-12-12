/**
   @file RAK12052_32x24_IR_Display_MLX90640.ino
   @author rakwireless.com
   @brief Serial output temperature array data and display the binarized graphics through RAK1921.
   @version 0.1
   @date 2021-02-09
   @copyright Copyright (c) 2022
**/

#include "RAK12052-MLX90640.h"  // Click to install library: http://librarymanager/All#RAK12052-MLX90640
#include <U8g2lib.h>            // Click to install library: http://librarymanager/All#u8g2

RAK_MLX90640 MLX90640;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

void setup() 
{
  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  // Initialize Serial for debug output.
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

  Serial.println("======================================");
  Serial.println("  RAK12052 32x24 IR Display Example");
  Serial.println("======================================");
  
  if (! MLX90640.begin()) 
  {
    Serial.println("MLX90640 not found!");
    while (1) 
    {
      delay(10);
    }
  }

  u8g2.begin();
  u8g2.clearDisplay();
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  
  Serial.println("Found MLX90640");
  Serial.print("Serial number: ");
  Serial.print(MLX90640.serialNumber[0], HEX);
  Serial.print(MLX90640.serialNumber[1], HEX);
  Serial.println(MLX90640.serialNumber[2], HEX);
  
  MLX90640.setMode(MLX90640_CHESS);
  Serial.print("Current mode: ");
  delay(200);
  
  if (MLX90640.getMode() == MLX90640_CHESS) 
  {
    Serial.println("Chess");
  } 
  else 
  {
    Serial.println("Interleave");    
  }

  MLX90640.setResolution(MLX90640_ADC_18BIT);
  Serial.print("Current resolution: ");
  mlx90640_resolution_t res = MLX90640.getResolution();
  switch (res) 
  {
    case MLX90640_ADC_16BIT: Serial.println("16 bit"); break;
    case MLX90640_ADC_17BIT: Serial.println("17 bit"); break;
    case MLX90640_ADC_18BIT: Serial.println("18 bit"); break;
    case MLX90640_ADC_19BIT: Serial.println("19 bit"); break;
  }

  MLX90640.setRefreshRate(MLX90640_16_HZ);
  Serial.print("Current frame rate: ");
  mlx90640_refreshrate_t rate = MLX90640.getRefreshRate();
  switch (rate) 
  {
    case MLX90640_0_5_HZ: Serial.println("0.5 Hz"); break;
    case MLX90640_1_HZ: Serial.println("1 Hz"); break; 
    case MLX90640_2_HZ: Serial.println("2 Hz"); break;
    case MLX90640_4_HZ: Serial.println("4 Hz"); break;
    case MLX90640_8_HZ: Serial.println("8 Hz"); break;
    case MLX90640_16_HZ: Serial.println("16 Hz"); break;
    case MLX90640_32_HZ: Serial.println("32 Hz"); break;
    case MLX90640_64_HZ: Serial.println("64 Hz"); break;
  }
}

void loop() 
{
  delay(64);
  u8g2.clearBuffer();
  
  // Draw outline.
  u8g2.drawHLine(31,11,65);
  u8g2.drawHLine(31,11+49,65);
  u8g2.drawVLine(31,11,49);
  u8g2.drawVLine(31+65,11,49);
  
  if (MLX90640.getFrame(MLX90640.frame) != 0) 
  {
    Serial.println("Failed");
  }
  Serial.println();
  
  for (uint8_t h=0; h<24; h++) 
  {
    for (uint8_t w=0; w<32; w++) 
    {
      float t = MLX90640.frame[h*32 + w];
      Serial.print(t, 1);
      Serial.print(", ");
      if(t>25.5)
      {
        uint8_t x,y;
        x= 32 + w*2 ;
        y= 12 + h*2 ;
        u8g2.drawPixel(x,y); 
        u8g2.drawPixel(x+1,y);
        u8g2.drawPixel(x+1,y+1);  
        u8g2.drawPixel(x,y+1);  // Simple interpolation to make the image bigger.
      }
    }
    Serial.println();
  }
  u8g2.sendBuffer();
}
