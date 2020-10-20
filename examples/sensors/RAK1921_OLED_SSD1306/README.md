# WisBlock IO RAK1921 OLED Board Programming Guide

This example shows how to use WisBlock IO RAK1921 which is an OLED SSD1306(128*64, I2C) board.

<img src="../../../assets/repo/rak1921.png" alt="rak1921" style="zoom:25%;" />

----
## Hardware required
----
The following hardware are required.

- WisBlock Base RAK5005-O  \*  1pcs    
- WisBlock Core RAK4631      \*  1pcs    
- WisBlock Sensor RAK1906          \*  1pcs        (Optional)    
- WisBlock IO RAK1921          \*  1pcs    



Hardware is shown as follows:

![exa-rak1921-rak1906-assy](../../../assets/repo/exa-rak1921-rak1906-assy.png)

Assembled as follows:

![exa-rak1921-rak1906-assy-2](../../../assets/repo/exa-rak1921-rak1906-assy-2.png)

For stable connection, please use screws to tighten.

----
## Software required
----
To use the environment WisBlock Sensor RAK1906, The following software is required.

- [ArduinoIDE](https://www.arduino.cc/en/Main/Software)
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)    
- U8g2 library

![lib-u8g-install](../../../assets/Arduino/lib-u8g-install.png)



- CloseCube BME680 Library

![lib-bme680-install](../../../assets/Arduino/lib-bme680-install.png)



Connect the assembled hardware to the PC with a USB cable, open the Arduino, and select the development board and port in the tool menu.

![ard-install-21](../../../assets/Arduino/ard-install-21.png)



## Demo

### Demo 1

The Demo  is designed to display BME680 sensor data on SSD1306.

```
/*
   This sketch demonstrate how to use SSD1306 to display sensor data of BME680.
*/
#include <Wire.h>
#include "ClosedCube_BME680.h"  //http://librarymanager/All#ClosedCube_BME680
#include <U8g2lib.h>   //http://librarymanager/All#U8g2

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
ClosedCube_BME680 bme680;

void bme680_init(){
  Wire.begin();
  bme680.init(0x76); // I2C address: 0x76 or 0x77
  bme680.reset();

  Serial.print("Chip ID=0x");
  Serial.println(bme680.getChipID(), HEX);

  // oversampling: humidity = x1, temperature = x2, pressure = x16
  bme680.setOversampling(BME680_OVERSAMPLING_X1, BME680_OVERSAMPLING_X2, BME680_OVERSAMPLING_X16);
  bme680.setIIRFilter(BME680_FILTER_3);
  bme680.setGasOn(300, 100); // 300 degree Celsius and 100 milliseconds 

  bme680.setForcedMode();
}

void bme680_get(){
    char data[32] = {0};
    double temp = bme680.readTemperature();
    double pres = bme680.readPressure();
    double hum = bme680.readHumidity();

    Serial.print("T=");
    Serial.print(temp);
    Serial.print("C, RH=");
    Serial.print(hum);
    Serial.print("%, P=");
    Serial.print(pres);
    Serial.print("hPa");
      
    uint32_t gas = bme680.readGasResistance();

    Serial.print(", G=");
    Serial.print(gas);
    Serial.print(" Ohms");
    Serial.println();

    bme680.setForcedMode();


    // display bme680 sensor data on OLED
    u8g2.clearBuffer();                   // clear the internal memory
    u8g2.setFont(u8g2_font_ncenB10_tr);   // choose a suitable font
    
    memset(data, 0, sizeof(data));
    sprintf(data, "T=%.2fC", temp);
    u8g2.drawStr(3, 15, data);
    
    memset(data, 0, sizeof(data));
    snprintf(data, 64, "RH=%.2f%%", hum);
    u8g2.drawStr(3, 30, data);

    memset(data, 0, sizeof(data));
    sprintf(data, "P=%.2fhPa", pres);
    u8g2.drawStr(3, 45, data);

    memset(data, 0, sizeof(data));
    sprintf(data, "G=%dOhms", gas);
    u8g2.drawStr(3, 60, data);

    u8g2.sendBuffer();                    // transfer internal memory to the display
}

void setup()
{
  // Initialize Serial for debug output
  Serial.begin(115200);

  bme680_init();
  u8g2.begin();
}

void loop()
{
  bme680_get();
  delay(5000);
}
```



The test results are as follows：

![exa-rak1921-temp](../../../assets/Examples/exa-rak1921-temp.png)







