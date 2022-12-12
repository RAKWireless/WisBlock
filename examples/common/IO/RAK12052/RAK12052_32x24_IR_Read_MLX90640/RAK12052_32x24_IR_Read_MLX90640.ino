/**
   @file RAK12052_32x24_IR_Read_MLX90640.ino
   @author rakwireless.com
   @brief Read the temperature array collected by the sensor and output it through the serial port.
   @version 0.1
   @date 2021-02-09
   @copyright Copyright (c) 2022
**/

#include "RAK12052-MLX90640.h"  // Click here to get the library: http://librarymanager/All#RAK12052-MLX90640

RAK_MLX90640 MLX90640;

#define PRINT_ASCIIART
//#define PRINT_TEMPERATURES

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
  Serial.println("    RAK12052 32x24 IR Read Example");
  Serial.println("======================================");
  
  if (! MLX90640.begin()) 
  {
    Serial.println("MLX90640 not found!");
    while (1) 
    {
      delay(10);
    }
  }
  Serial.println("Found MLX90640");
  Serial.print("Serial number: ");
  Serial.print(MLX90640.serialNumber[0], HEX);
  Serial.print(MLX90640.serialNumber[1], HEX);
  Serial.println(MLX90640.serialNumber[2], HEX);
  
  //MLX90640.setMode(MLX90640_INTERLEAVED);
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

  MLX90640.setRefreshRate(MLX90640_2_HZ);
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
  delay(500);
  if (MLX90640.getFrame(MLX90640.frame) != 0) 
  {
    Serial.println("Failed");
    return;
  }
  Serial.println();
  Serial.println();
  for (uint8_t h=0; h<24; h++) 
  {
    for (uint8_t w=0; w<32; w++) 
    {
      float t = MLX90640.frame[h*32 + w];
#ifdef PRINT_TEMPERATURES
      Serial.print(t, 1);
      Serial.print(", ");
#endif
#ifdef PRINT_ASCIIART
      char c = '&';
      if (t < 20) 
        c = ' ';
      else if (t < 23) 
        c = '.';  //  
      else if (t < 25) 
        c = '-';
      else if (t < 27) 
        c = '*';
      else if (t < 29) 
        c = '+';
      else if (t < 31) 
        c = 'x';
      else if (t < 33) 
        c = '%';
      else if (t < 35) 
        c = '#';
      else if (t < 37) 
        c = 'X';
      Serial.print(c);
#endif
    }
    Serial.println();
  }
}
