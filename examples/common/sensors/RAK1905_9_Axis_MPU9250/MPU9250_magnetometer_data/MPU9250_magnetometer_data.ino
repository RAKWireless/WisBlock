/**
   @file MPU9250_magnetometer_data.ino
   @author rakwireless.com
   @brief  This sketch shows how to read magnetometer values from the MPU9250, or
           - to be more exact - from the AK8983. The AK8963 is part of the MPU9250 only
           to a certain extent. It has an own I2C address and needs to be initiated
           separately. However the MPU9250 needs to be initiated before.
   @version 0.1
   @date 2022-03-17
   @copyright Copyright (c) 2022
**/

#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68  // library: http://librarymanager/All#MPU9250_WE

/* There are several ways to create your MPU9250 object:
   MPU9250_WE myMPU9250 = MPU9250_WE()              -> uses Wire / I2C Address = 0x68
   MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR)  -> uses Wire / MPU9250_ADDR
   MPU9250_WE myMPU9250 = MPU9250_WE(&wire2)        -> uses the TwoWire object wire2 / MPU9250_ADDR
   MPU9250_WE myMPU9250 = MPU9250_WE(&wire2, MPU9250_ADDR) -> all together
   Successfully tested with two I2C busses on an ESP32
*/
MPU9250_WE myMPU9250 = MPU9250_WE(MPU9250_ADDR);

void setup() {
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
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
  Wire.begin();
  if (!myMPU9250.init()) {
    Serial.println("MPU9250 does not respond");
    while (1) delay(100);
  }
  else {
    Serial.println("MPU9250 is connected");
  }
  if (!myMPU9250.initMagnetometer()) {
    Serial.println("Magnetometer does not respond");
    while (1) delay(100);
  }
  else {
    Serial.println("Magnetometer is connected");
  }
  Serial.println("RAK1905 Test!");
  /* You can choose the following operational modes
     AK8963_PWR_DOWN            power down (default)
     AK8963_CONT_MODE_8HZ       continuous at 8Hz sample rate
     AK8963_CONT_MODE_100HZ     continuous at 100Hz sample rate

     In trigger mode the AK8963 goes into power down after the measurement
  */
  myMPU9250.setMagOpMode(AK8963_CONT_MODE_100HZ);

  /* In continuous mode you need to wait for the first data to be available. If you
     comment the line below you will probably obtain zero.
  */
  delay(200);
}

void loop() {
  xyzFloat magValue = myMPU9250.getMagValues(); // returns magnetic flux density [µT]

  Serial.println("Magnetometer Data in µTesla: ");
  Serial.print(magValue.x);
  Serial.print("   ");
  Serial.print(magValue.y);
  Serial.print("   ");
  Serial.println(magValue.z);

  delay(1000);
}
