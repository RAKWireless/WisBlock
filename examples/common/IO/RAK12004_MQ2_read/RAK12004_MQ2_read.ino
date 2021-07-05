/**
 * @file RAK12004_MQ2_read.ino
 * @author rakwireless.com
 * @brief use MQ2 gas sensor read sensor data and display on OLED example.
 * @version 0.1
 * @date 2021-05-08
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include "ADC121C021.h"
#include <U8g2lib.h>       // Click to install library: http://librarymanager/All#u8g2

#define EN_PIN        WB_IO6  //Logic high enables the device. Logic low disables the device
#define ALERT_PIN     WB_IO5  //a high indicates that the respective limit has been violated.
#define MQ2_ADDRESS   0x51    //the device i2c address

#define      RatioMQ2CleanAir     (1.0)   //RS / R0 = 1.0 ppm 
#define      MQ2_RL               (10.0)  //the board RL = 10KΩ  can adjust

ADC121C021 MQ2;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

uint16_t result;
char displayData[32];    //OLED dispaly datas

//Function declaration
void firstDisplay();

void setup()
{  
 pinMode(ALERT_PIN,INPUT);
 pinMode(EN_PIN,OUTPUT);
 digitalWrite(EN_PIN,HIGH);  //power on RAK12004
 delay(5);
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
  sprintf(displayData, "RAK12004 Test");
  u8g2.drawStr(3, 15, displayData);
  sprintf(displayData, "MQ2 checking...");
  u8g2.drawStr(3, 45, displayData);
  u8g2.sendBuffer();

 //********ADC121C021 ADC convert init ********************************
   while(!(MQ2.begin(MQ2_ADDRESS,Wire)))
   {
    Serial.println("please check device!!!"); 
    delay(200);
   }     
  Serial.println("RAK12004 test Example");
 
  //**************init MQ2 *****************************************************
  MQ2.setRL(MQ2_RL);
  /*
   *detect Propane gas if to detect other gas  need to reset A and B value，it depend on MQ sensor datasheet 
   */
  MQ2.setA(-0.890);   //A -> Slope, -0.685   
  MQ2.setB(1.125);    //B -> Intersect with X - Axis  1.019
 //Set math model to calculate the PPM concentration and the value of constants
  MQ2.setRegressionMethod(0); //PPM =  pow(10, (log10(ratio)-B)/A)
 
  float calcR0 = 0;
  for(int i = 1; i<=100; i ++)
  {   
    calcR0 += MQ2.calibrateR0(RatioMQ2CleanAir);    
  }
  MQ2.setR0(calcR0/10);
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}

  float r0 = MQ2.getR0();
  Serial.printf("R0 Value is:%3.2f\r\n",r0);  
  firstDisplay();     
  delay(3000);
}
void loop()
{  
  float sensorPPM;
  float PPMpercentage;
  
  Serial.println("Getting Conversion Readings from ADC121C021");
  Serial.println(" ");  
  sensorPPM = MQ2.readSensor(); 
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
  sprintf(displayData, "RAK12004 Test");
  u8g2.drawStr(3, 15, displayData);
 
  sprintf(displayData, "Propane:");
  u8g2.drawStr(3, 30, displayData);      

  sprintf(displayData, "%3.2f PPM",sensorPPM);
  u8g2.drawStr(3, 45, displayData);  

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
  sprintf(displayData, "RAK12004 Test");
  u8g2.drawStr(3, 15, displayData);
  u8g2.sendBuffer();

  sprintf(displayData, "R0:%3.3f", MQ2.getR0());
  u8g2.drawStr(3, 30, displayData);      
  u8g2.sendBuffer(); 

  float voltage = MQ2.getSensorVoltage(); 
  sprintf(displayData, "voltage:%3.3f",voltage);
  u8g2.drawStr(3, 45, displayData);      
  u8g2.sendBuffer(); 
}
