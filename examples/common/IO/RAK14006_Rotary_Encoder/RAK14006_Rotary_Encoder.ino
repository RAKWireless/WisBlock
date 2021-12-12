/**
   @file RAK14006_Rotary_Encoder.ino
   @author rakwireless.com
   @brief Rotary Encoder Example
   @version 0.1
   @date 2021-7-28
   @copyright Copyright (c) 2020
**/
#include <Wire.h>

#define ENCODER_A_PIN WB_IO6  //clockwise
#define ENCODER_B_PIN WB_IO4  //anticlockwise
#define SWITCH_PIN    WB_IO5  //press key

//position is 0 when start up
long position = 0;
bool press_flag = false;
bool clockwise_flag = false;
bool anticlockwise_flag = false;

void setup(){
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  //setup our pins 
  pinMode(ENCODER_A_PIN, INPUT);
  pinMode(ENCODER_B_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT);

  attachInterrupt(SWITCH_PIN, handle_1, FALLING);
  attachInterrupt(ENCODER_A_PIN, handle_2, RISING);
  attachInterrupt(ENCODER_B_PIN, handle_3, RISING);
  //setup our serial 
  Serial.begin(115200);
  Serial.println("You can press and twist the button!!"); 
}

void loop(){
  if(press_flag==true)
  {
    press_flag = false;
    Serial.println("press key");
  }
  if(clockwise_flag==true)
  {
    clockwise_flag = false;
    position++;
    Serial.print("position step is ");
    Serial.println(position);
  }
  if(anticlockwise_flag==true)
  {
    anticlockwise_flag = false;
    position--;
    Serial.print("position step is ");
    Serial.println(position);
  }  
}

void handle_1(){  
  press_flag = true;
}
void handle_2(){  
  clockwise_flag = true;
}
void handle_3(){  
  anticlockwise_flag = true;
}
