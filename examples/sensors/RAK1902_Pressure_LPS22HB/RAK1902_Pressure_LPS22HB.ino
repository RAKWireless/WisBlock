#include <Arduino_LPS22HB.h>  // Click here to get the library: http://librarymanager/All#Arduino_LPS22HB


void setup() {
  // Setup usb 
  Serial.begin(115200);
  while(!Serial);

    /* LPS22HB init */
    if (!BARO.begin()) {
      Serial.println("Failed to initialize pressure sensor!");
      while (1);
    }
}

void loop() {

  lps22hb_get();
  delay(1000);
}

void lps22hb_get(){
  // read the sensor value
  float pressure = BARO.readPressure();

  // print the sensor value
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" kPa");
}
