#include <Wire.h>
#include "ClosedCube_BME680.h"  //https://github.com/closedcube/ClosedCube_BME680_Arduino

ClosedCube_BME680 bme680;

void bme680_init()
{
  Wire.begin();
  bme680.init(0x76); // I2C address: 0x76 or 0x77
  bme680.reset();

  Serial.print("Chip ID=0x");
  Serial.println(bme680.getChipID(), HEX);

  // oversampling: humidity = x1, temperature = x2, pressure = x16
  bme680.setOversampling(BME680_OVERSAMPLING_X1, BME680_OVERSAMPLING_X2, BME680_OVERSAMPLING_X16);
  bme680.setIIRFilter(BME680_FILTER_3);
  bme680.setGasOn(300, 100); // 300 degree Celsius and 100 milliseconds

  bme680.setForcedMode();
}

void bme680_get()
{
  double temp = bme680.readTemperature();
  double pres = bme680.readPressure();
  double hum = bme680.readHumidity();

  Serial.print("T=");
  Serial.print(temp);
  Serial.print("C, RH=");
  Serial.print(hum);
  Serial.print("%, P=");
  Serial.print(pres);
  Serial.print("hPa");

  uint32_t gas = bme680.readGasResistance();

  Serial.print(", G=");
  Serial.print(gas);
  Serial.print(" Ohms");
  Serial.println();

  bme680.setForcedMode();
}

void setup()
{
  // Initialize Serial for debug output
  Serial.begin(115200);
  // while(!Serial){delay(10);}

  bme680_init();
}

void loop()
{
  bme680_get();
  delay(5000);
}
