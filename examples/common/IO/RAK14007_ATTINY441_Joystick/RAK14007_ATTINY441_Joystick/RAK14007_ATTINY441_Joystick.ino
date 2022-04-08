/**
   @file RAK14007_ATTINY441_Joystick.ino
   @author rakwireless.com
   @brief Joystick example
   @version 0.1
   @date 2021-7-28
   @copyright Copyright (c) 2020
**/
#include <Wire.h>

#define INT_LED                           LED_GREEN
#define RESET_PIN                         WB_IO4
#define SLAVE_I2C_ADDRESS_DEFAULT         0x21
//JOYSTICK Sensor Register Addresses
#define JOYSTICK_GET_DATA             0x01 // (r)   2 bytes
#define JOYSTICK_GET_I2C_ADDRESS      0x02  //(r)   1 bytes
#define JOYSTICK_SET_I2C_ADDRESS      0x03  //(w)   1 bytes
#define JOYSTICK_GET_VERSION          0x04 // (r)   1 bytes
#define JOYSTICK_SET_INT_TYPE         0x05 // (W)   1 bytes, five keys, from bit4-bit0, 0-push, 1-release. 
#define JOYSTICK_GET_X_POSITION       0x06 // (r)   2 bytes, return the analog quantity. Range:0-1024, middle position is 507(near 512)
#define JOYSTICK_GET_Y_POSITION       0x07 // (r)   2 bytes, return the analog quantity. Range:0-1024, middle position is 507(near 512)
#define JOYSTICK_SET_X_LEFT_THREHOLD  0x08 // (w)   2 bytes, set the analog quantity. Range:0-1024
#define JOYSTICK_SET_X_RIGHT_THREHOLD 0x09 // (w)   2 bytes, set the analog quantity. Range:0-1024
#define JOYSTICK_SET_Y_UP_THREHOLD    0x0A // (w)   2 bytes, set the analog quantity. Range:0-1024
#define JOYSTICK_SET_Y_DOWN_THREHOLD  0x0B // (w)   2 bytes, set the analog quantity. Range:0-1024
#define JOYSTICK_GET_X_LEFT_THREHOLD  0x0C // (r)   2 bytes, get the analog quantity. Range:0-1024
#define JOYSTICK_GET_X_RIGHT_THREHOLD 0x0D // (r)   2 bytes, get the analog quantity. Range:0-1024
#define JOYSTICK_GET_Y_UP_THREHOLD    0x0E // (r)   2 bytes, get the analog quantity. Range:0-1024
#define JOYSTICK_GET_Y_DOWN_THREHOLD  0x0F // (r)   2 bytes, get the analog quantity. Range:0-1024


uint8_t flag = 0;

void detect()
{
  flag = 1;
  digitalWrite(INT_LED, HIGH);
}

//motion1       NA      NA     NA   keyE keyD keyC keyB keyA     
//bit           7       6      5     4     3    2    1    0
//motion2       NA     NA     NA    NA    x-l   x-r  y-u  y-d
//bit           7       6      5     4     3    2    1    0
void judge()
{
  uint8_t data[2] = {0};
  uint8_t motion[2]  = {0}; //we use 8 bit to record the motion
  uint16_t tmp = 0;
  read_from_ttiny441(JOYSTICK_GET_DATA,motion,2);
  if((motion[0] & 0x01) == 0x01){Serial.println("KeyA Pressed");}
  if((motion[0] & 0x02) == 0x02){Serial.println("KeyB Pressed");}  
  if((motion[0] & 0x04) == 0x04){Serial.println("KeyC Pressed");}
  if((motion[0] & 0x08) == 0x08){Serial.println("KeyD Pressed");}
  if((motion[0] & 0x10) == 0x10){Serial.println("KeyE Pressed");}  
  if((motion[1] & 0x04) == 0x04)
  {
    tmp = 0;
    read_from_ttiny441(JOYSTICK_GET_X_POSITION,data,2);
    tmp = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
    if(tmp != 0xFFFF)
    {
       Serial.print("RIGHT ");
       Serial.print("X Position is ");
       Serial.println(tmp); 
    }
  }
  if((motion[1] & 0x08) == 0x08)
  {
    tmp = 0;
    read_from_ttiny441(JOYSTICK_GET_X_POSITION,data,2);
    tmp = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
    if(tmp != 0xFFFF)
    {
       Serial.print("LEFT ");
       Serial.print("X Position is ");
       Serial.println(tmp); 
    }
  }
  if((motion[1] & 0x02) == 0x02)
  {
    tmp = 0;
    read_from_ttiny441(JOYSTICK_GET_Y_POSITION,data,2);
    tmp = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
    if(tmp != 0xFFFF)
    {    
       Serial.print("UP ");
       Serial.print("Y Position is ");
       Serial.println(tmp); 
    }
  }
  if((motion[1] & 0x01) == 0x01)
  {
    tmp = 0;
    read_from_ttiny441(JOYSTICK_GET_Y_POSITION,data,2);
    tmp = (((uint16_t)data[0]) << 8) | ((uint16_t)data[1]);
    if(tmp != 0xFFFF)
    {    
       Serial.print("DOWN ");
       Serial.print("Y Position is ");
       Serial.println(tmp); 
    }
  }   
  digitalWrite(INT_LED, LOW);   
}

void setup() {
  pinMode(WB_IO2, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(WB_IO2, LOW);
  delay(1000);
  digitalWrite(WB_IO2, HIGH);  
  pinMode(INT_LED, OUTPUT);
  digitalWrite(INT_LED, LOW);
  Serial.begin(115200);
  Serial.print("Joystick Test!");
  Wire.begin();  
  delay(1000);
  uint8_t data = 0;
  read_from_ttiny441(JOYSTICK_GET_VERSION,&data,1);
  Serial.print("Sensor Firmware version: ");
  Serial.println(data,HEX);
  Serial.println(); 
  //here we use interrupt to notify
  pinMode(WB_IO5, INPUT);
  attachInterrupt(WB_IO5, detect, RISING);
  //set interrupt type for each key. 0 for push/ 1 for release. This step is necessary.
  uint8_t itp = 0x00; 
  write_to_ttiny441(JOYSTICK_SET_INT_TYPE,&itp,1); 
}

void loop() {
  if(flag == 1)
  {
    //slave interrupt will clear after host read
    judge();
    flag = 0;
  }
  delay(10);
}

void read_from_ttiny441(uint8_t reg, uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(SLAVE_I2C_ADDRESS_DEFAULT); 
  Wire.write(reg);        // sends five bytes
  Wire.endTransmission();    // stop transmitting
  delay(20);
  Wire.requestFrom(SLAVE_I2C_ADDRESS_DEFAULT,length);
  int i = 0;
  while ( Wire.available() ) // slave may send less than requested
  {
    data[i++]= Wire.read(); // receive a byte as a proper uint8_t
  }  
}

void write_to_ttiny441(uint8_t reg, uint8_t *data, uint8_t length)
{
  Wire.beginTransmission(SLAVE_I2C_ADDRESS_DEFAULT); 
  Wire.write(reg);        // sends five bytes
  for(int i=0;i<length;i++)
  {
    Wire.write(data[i]);
  }
  Wire.endTransmission();    // stop transmitting
}
