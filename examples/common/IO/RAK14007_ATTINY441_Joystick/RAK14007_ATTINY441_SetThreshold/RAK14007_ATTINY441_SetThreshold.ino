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

  //set interrupt type for each key. 0 for push/ 1 for release. This step is necessary.
  uint8_t itp = 0x00; // Set A B as push Key, C D E as release Key
  write_to_ttiny441(JOYSTICK_SET_INT_TYPE,&itp,1); 
  
  uint8_t tmp[2] = {0};
  //set threshold of x right axis MSB first
  uint16_t x_right = 700;
  tmp[0]= (uint8_t)((x_right & 0xFF00) >> 8);
  tmp[1]= (uint8_t)(x_right & 0x00FF);
  write_to_ttiny441(JOYSTICK_SET_X_RIGHT_THREHOLD,tmp,2);
  
  //set threshold of x left axis MSB first
  uint16_t x_left = 100;
  tmp[0]= (uint8_t)((x_left & 0xFF00) >> 8);
  tmp[1]= (uint8_t)(x_left & 0x00FF);
  write_to_ttiny441(JOYSTICK_SET_X_LEFT_THREHOLD,tmp,2);

  //set threshold of y up axis MSB first
  uint16_t y_up = 900;
  tmp[0]= (uint8_t)((y_up & 0xFF00) >> 8);
  tmp[1]= (uint8_t)(y_up & 0x00FF);  
  write_to_ttiny441(JOYSTICK_SET_Y_UP_THREHOLD,tmp,2);
  
  //set threshold of y down axis MSB first
  uint16_t y_down = 300;
  tmp[0]= (uint8_t)((y_down & 0xFF00) >> 8);
  tmp[1]= (uint8_t)(y_down & 0x00FF); 
  write_to_ttiny441(JOYSTICK_SET_Y_DOWN_THREHOLD,tmp,2);

}

void loop() {

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
