/**
 * @file RAK12008_CO2_Detection.ino
 * @author rakwireless.com
 * @brief use MG812 sensor detect CO2 example.
 * @version 0.1
 * @date 2021-06-08
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include "ADC121C021.h"     // Click to install library: http://librarymanager/All#MQx
#include <U8g2lib.h>       // Click to install library: http://librarymanager/All#u8g2

#define EN_PIN          WB_IO6  //Logic high enables the device. Logic low disables the device
#define ALERT_PIN       WB_IO5  //a high indicates that the respective limit has been violated.
#define MG812_ADDRESS   0x52    //the device i2c address

#define   V_RATIO  3.0 // voltage amplification factor 

float constantA = -0.101;   //log(y) = constantA*log(x) + constantB,  y:sensor voltage,  x:gas concentration ppm
float constantB = -0.282;   //log(y) = constantA*log(x) + constantB,  y:sensor voltage,  x:gas concentration ppm

ADC121C021 MG812;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);
  
uint16_t result;
char displayData[32];    //OLED dispaly datas

//Function declaration
void firstDisplay();

void setup()
{  
 pinMode(ALERT_PIN,INPUT);
 pinMode(EN_PIN,OUTPUT);
 digitalWrite(EN_PIN,HIGH);  //power on RAK12008
 delay(500);
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
//***********OLED display init ***************************************** 
  u8g2.begin();
  u8g2.clearDisplay();
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font  
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "RAK12008 Test");
  u8g2.drawStr(3, 15, displayData);
  sprintf(displayData, "MG812 checking...");
  u8g2.drawStr(3, 45, displayData);
  u8g2.sendBuffer();

 //********ADC121C021 ADC convert init ********************************
   while(!(MG812.begin(MG812_ADDRESS,Wire)))
   {
    Serial.println("please check device!!!"); 
    delay(200);
   }     
  Serial.println("RAK12008 test Example");

 //**************init MG812********************************************
  
  firstDisplay();     
  delay(3000);
}
void loop()
{  
  float sensorPPM;
  float PPMpercentage;
  float sensorVoltage;
  
  Serial.println("Getting Conversion Readings from ADC121C021");
  Serial.println(" ");  
  sensorVoltage = MG812.getSensorVoltage()/V_RATIO;
  double ppm_log = (log10(sensorVoltage)-constantB)/constantA; //Get ppm value in linear scale according to the the ratio value  
  sensorPPM = pow(10, ppm_log); //Convert ppm value to log scale  PPM =  pow(10, (log10(voltage)-B)/A)

  Serial.printf("sensor voltage Value is: %3.2f\r\n",sensorVoltage);    
  Serial.printf("sensor PPM Value is: %3.2f\r\n",sensorPPM);   
  PPMpercentage = sensorPPM/10000;
  Serial.printf("PPM percentage Value is:%3.2f%%\r\n",PPMpercentage);   
  Serial.println(" ");
  Serial.println("        ***************************        ");
  Serial.println(" ");  
     
  u8g2.clearDisplay();
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font  
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "RAK12008 Test");
  u8g2.drawStr(3, 15, displayData);

  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "CO2:");
  u8g2.drawStr(3, 30, displayData);
  
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "%3.2f PPM",sensorPPM);
  u8g2.drawStr(3, 45, displayData);    
    
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "%3.2f %%",PPMpercentage);
  u8g2.drawStr(3, 60, displayData); 
       
  u8g2.sendBuffer();  
           
  delay(1000); 
}
void firstDisplay()
{  
  u8g2.clearDisplay();
  u8g2.clearBuffer();  
  u8g2.setFont(u8g2_font_ncenB10_tr); // choose a suitable font  
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "RAK12008 Test");
  u8g2.drawStr(3, 15, displayData);
  u8g2.sendBuffer();
    
  float voltage = MG812.getSensorVoltage(); 
  sprintf(displayData, "voltage:%3.3f",voltage);
  u8g2.drawStr(3, 45, displayData);      
  u8g2.sendBuffer(); 
}
