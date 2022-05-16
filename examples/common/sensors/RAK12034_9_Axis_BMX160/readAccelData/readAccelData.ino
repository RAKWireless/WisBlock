/*!
   @file readAccelData.ino
   @author rakwireless.com
   @brief Through the example, you can get the accelerometer data
   @version 0.1
   @date 2022-01-04
   @copyright Copyright (c) 2020
*/

#include "Rak_BMX160.h"

RAK_BMX160 bmx160;
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

  //init the hardware bmx160
  if (bmx160.begin() != true) {
    Serial.println("bmx160 init false");
    while (1) delay(100);
  }

  bmx160.ODR_Config(BMX160_ACCEL_ODR_200HZ, BMX160_GYRO_ODR_200HZ); //set output data rate

  float OrdBuf[2] = {0};
  bmx160.get_ORD_Config(&OrdBuf[0], &OrdBuf[1]);
  Serial.printf("OrdBuf[0]=%f,OrdBuf[1]=%f\r\n", OrdBuf[0], OrdBuf[1]);

  /**
      enum{eAccelRange_2G,
            eAccelRange_4G,
            eAccelRange_8G,
            eAccelRange_16G
            }eAccelRange_t;
  */
  bmx160.setAccelRange(eAccelRange_2G);
  delay(100);
}

void loop() {
  sBmx160SensorData_t  Oaccel;

  /* Get a new sensor event */
  bmx160.getAllData(NULL, NULL, &Oaccel);

  /* Display the accelerometer results (accelerometer data is in m/s^2) */
  Serial.print("Accel ");
  Serial.print("X: "); Serial.print(Oaccel.x    ); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Oaccel.y    ); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Oaccel.z    ); Serial.print("  ");
  Serial.println("m/s^2");

  Serial.println("");

  delay(500);
}
