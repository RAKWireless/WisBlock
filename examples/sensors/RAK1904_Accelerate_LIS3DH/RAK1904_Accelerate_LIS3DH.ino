#include "SparkFunLIS3DH.h" ////https://github.com/sparkfun/SparkFun_LIS3DH_Arduino_Library/blob/master/examples/MultiI2C/MultiI2C.ino
#include <Wire.h>
  
LIS3DH SensorTwo( I2C_MODE, 0x18 );



void setup() {
  // Setup usb 
  Serial.begin(115200);
  while(!Serial);

  if( SensorTwo.begin() != 0 )
  {
    Serial.println("Problem starting the sensor at 0x18.");
  }
  else
  {
    Serial.println("Sensor at 0x18 started.");
  }
    Serial.println("enter !");
}

void loop() {

  lis3dh_get();
  delay(1000);
}

void lis3dh_get(){
  // read the sensor value
  uint8_t cnt = 0;

  Serial.print(" X(g) = ");
  Serial.println(SensorTwo.readFloatAccelX(), 4);
  Serial.print(" Y(g) = ");
  Serial.println(SensorTwo.readFloatAccelY(), 4);
  Serial.print(" Z(g)= ");
  Serial.println(SensorTwo.readFloatAccelZ(), 4);
}
