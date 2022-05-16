/*!
   @file readMagnData.ino
   @brief Through the example, you can get the magnetometer data
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
  delay(100);
}

void loop() {
  sBmx160SensorData_t Omagn;

  /* Get a new sensor event */
  bmx160.getAllData(&Omagn, NULL, NULL);

  /* Display the magnetometer results (magn is magnetometer in uTesla) */
  Serial.print("Magn ");
  Serial.print("X: "); Serial.print(Omagn.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(Omagn.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(Omagn.z); Serial.print("  ");
  Serial.println("uT");

  Serial.println("");

  delay(500);
}
