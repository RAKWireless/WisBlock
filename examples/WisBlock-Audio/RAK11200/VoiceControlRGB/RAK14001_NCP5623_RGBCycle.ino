#include <Wire.h>
#include <NCP5623.h>	//http://librarymanager/All#NCP5623 By:RAKWireless

NCP5623 rgb;

uint8_t g_brightness = 15;

void rgb_init() {
  // enable RAK14001
  pinMode(WB_IO6, OUTPUT);
  digitalWrite(WB_IO6, HIGH);
  delay(50);
  // If using Native I2C
  Wire.begin();
  Wire.setClock(100000); 
  if (!rgb.begin())
  {
    Serial.println("RAK14001 not found on the I2C line");
    while (1);
  }
  else
  {
    Serial.println("RAK14001 Found. Begining execution");
  }
  // set the current output level max, the range is 1 to 31
  rgb.setCurrent(g_brightness);
}

void breathingLamp(bool Red, bool Green, bool Blue)
{
  int pwm = 31;
  for(int i=pwm; i>=0; i--)
  {
    rgb.setColor(Red?i*8:0, Green?i*8:0, Blue?i*8:0);
    delay(100);
  }
  delay(200);
  for(int j=1; j<pwm; j++)
  {
    rgb.setColor(Red?j*8:0, Green?j*8:0, Blue?j*8:0);
    delay(100);
  }
}

void control_rgb(int element)
{
  switch(element)
  {
    case 2000:  //Turn Lights on
    rgb.setColor(255,255,255); // WHITE
    Serial.println("Turn Lights on");
    break;  
    case 2001:  //Turn Lights off
    rgb.setColor(0,0,0);     // OFF
    Serial.println("Turn Lights off");
    break;  
    case 2002:  //Lights on
    rgb.setColor(255,255,255); // WHITE
    Serial.println("Lights on");
    break;  
    case 2003:  //Lights off
    rgb.setColor(0,0,0);     // OFF
    Serial.println("Lights off");
    break;  
    case 2004:  //Turn lights up
     rgb.setColor(255,255,255); // WHITE
     Serial.println("Turn lights up");
    break;  
    case 2005:  //Turn lights down
    rgb.setColor(0,0,0);     // OFF
    Serial.println("Turn lights down");
    break;  
    case 2006:  //Dim lights
    if(g_brightness>1) g_brightness = g_brightness-5;
    if(g_brightness<1) g_brightness = 1;
    if(g_brightness>31) g_brightness = 31;
    // set the current output level max, the range is 1 to 31
    rgb.setCurrent(g_brightness);
    Serial.printf("Dim lights %d\r\n",g_brightness);
    break;  
    case 2007:  //Brighten lights
    if(g_brightness<31) g_brightness = g_brightness+5;
    if(g_brightness<1) g_brightness = 1;
    if(g_brightness>31) g_brightness = 31;
    // set the current output level max, the range is 1 to 31
    rgb.setCurrent(g_brightness);
    Serial.printf("Brighten lights %d\r\n",g_brightness);
    break;  
    case 2008:  //Lights Red 
    rgb.setColor(255,0,0);    // RED
    Serial.println("Lights Red");
    break;  
    case 2009:  //Lights Blue
    rgb.setColor(0,0,255);    // BLUE
    Serial.println("Lights Blue");
    break;  
    case 2010:  //Lights Green
    rgb.setColor(0,255,0);    // GREEN
    Serial.println("Lights Green");
    break;  
    case 2011:  //Lights Yellow
    rgb.setColor(255,255,0);  // YELLOW
    Serial.println("Lights Yellow");
    break;  
    case 2012:  //Lights Orange
    rgb.setColor(240,141,13); 
    Serial.println("Lights Orange");
    break;  
    case 2013:  //Lights Pink
    rgb.setColor(252,141,195); 
    Serial.println("Lights Pink");
    break; 
    default: break;     
  }  
}

void rgb_test() {
  rgb.setColor(0,0,0);     // OFF
  delay(1000);
  rgb.setColor(255,0,0);    // RED
  delay(1000);
  rgb.setColor(0,255,0);    // GREEN
  delay(1000);
  rgb.setColor(0,0,255);    // BLUE
  delay(1000);
  rgb.setColor(255,255,0);  // YELLOW
  delay(1000);
  rgb.setColor(255,0,255);  // PURPLE
  delay(1000);
  rgb.setColor(0,255,255);  // CYAN
  delay(1000);
  rgb.setColor(255,255,255); // WHITE
  delay(1000);
//  breathingLamp(true, false, false); // RED Breathing Lamp
//  breathingLamp(false, true, false); // GREEN Breathing Lamp
//  breathingLamp(false, false, true); // BLUE Breathing Lamp
//  breathingLamp(true, true, false);  // YELLOW Breathing Lamp
//  breathingLamp(true, false, true);   // PURPLE Breathing Lamp
//  breathingLamp(false, true, true);  // CYAN Breathing Lamp
//  breathingLamp(true, true, true);   // WHITE Breathing Lamp
}
