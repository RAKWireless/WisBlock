/**
 * @file RAK12009_MQ3_AlcoholDetection.ino
 * @author rakwireless.com
 * @brief use MQ3 sensor detect alcohol example.
 * @version 0.1
 * @date 2021-05-18
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include "ADC121C021.h"// Click to install library: http://librarymanager/All#MQx
#include <U8g2lib.h>       // Click to install library: http://librarymanager/All#u8g2

#define EN_PIN        WB_IO6  //Logic high enables the device. Logic low disables the device
#define ALERT_PIN     WB_IO5  //a high indicates that the respective limit has been violated.
#define MQ3_ADDRESS   0x55    //the device i2c address

#define      RatioMQ3CleanAir     (1)     //RS / R0 = 1 ppm 
#define      MQ3_RL               (10.0)  //RL = 10KΩ 

ADC121C021 MQ3;
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
  sprintf(displayData, "RAK12009 Test");
  u8g2.drawStr(3, 15, displayData);
  sprintf(displayData, "MQ3 checking...");
  u8g2.drawStr(3, 45, displayData);
  u8g2.sendBuffer();

 //********ADC121C021 ADC convert init ********************************
   while(!(MQ3.begin(MQ3_ADDRESS,Wire)))
   {
    Serial.println("please check device!!!"); 
    delay(200);
   }     
  Serial.println("RAK12009 test Example");
  
  //**************init MQ3******************************************** 
  //Set math model to calculate the PPM concentration and the value of constants
  MQ3.setRL(MQ3_RL);
  MQ3.setA(-1.176);  //A -> Slope,
  MQ3.setB(1.253);   //B -> Intersect with X - Axis
  //Set math model to calculate the PPM concentration and the value of constants
  MQ3.setRegressionMethod(0); //PPM =  pow(10, (log10(ratio)-B)/A)
  Serial.println("MQ3 initializing ......");
  delay(5000);  //Waiting for the power supply stabilization output
  float calcR0 = 0;
  for(int i = 1; i<=100; i ++)
  {   
    calcR0 += MQ3.calibrateR0(RatioMQ3CleanAir);    
  }
  MQ3.setR0(calcR0/100);  //set R0 the resister of sensor in the fresh air
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue founded, R0 is infite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue founded, R0 is zero (Analog pin with short circuit to ground) please check your wiring and supply"); while(1);}

  firstDisplay();     
  delay(3000);
}
void loop()
{  
  float sensorPPM;
  float PPMpercentage;
  
  Serial.println("Getting Conversion Readings from ADC121C021");
  Serial.println(" ");  
  sensorPPM = MQ3.readSensor(); 
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
  sprintf(displayData, "RAK12009 Test");
  u8g2.drawStr(3, 15, displayData);

  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "Alcohol:");
  u8g2.drawStr(3, 30, displayData);
  
  memset(displayData, 0, sizeof(displayData));
  sprintf(displayData, "%3.2fPPM",sensorPPM);
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
  sprintf(displayData, "RAK12009 Test");
  u8g2.drawStr(3, 15, displayData);
  u8g2.sendBuffer();

  sprintf(displayData, "R0:%3.3f", MQ3.getR0());
  u8g2.drawStr(3, 30, displayData);      
  u8g2.sendBuffer(); 

  float voltage = MQ3.getSensorVoltage(); 
  sprintf(displayData, "voltage:%3.3f",voltage);
  u8g2.drawStr(3, 45, displayData);      
  u8g2.sendBuffer(); 
}
