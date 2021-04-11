# WisBlock Sensor RAK1906 Environment board User Guide

This example shows how to use the WisBlock Sensor RAK1906 environment board.

The WisBlock Sensor RAK1906 environment board uses gas sensor BME680  which can measure relative humidity, barometric pressure, ambient temperature and gas (VOC).

![exa-bme680-features](../../../../assets/Examples/exa-bme680-features.png)



Product physical map is as follows:

<img src="../../../../assets/repo/rak1906-top.png" alt="rak1906-top" style="zoom:25%;" />

----
## Hardware required
----
To use the WisBlock Sensor RAK1906 environment board, The following hardware is required.

- WisBlock Base RAK5005-O  \*  1pcs
- WisBlock Core RAK4631      \*  1pcs
- WisBlock Sensor RAK1906          \*  1pcs



Hardware is shown as follows:

![exa-rak1906-assy](../../../../assets/repo/exa-rak1906-assy.png)

For stable connection, please use screws to tighten.

----
## Software required
----
To use the WisBlock Sensor RAK1906 environment board, The following software is required.

- [ArduinoIDE](https://www.arduino.cc/en/Main/Software)
- [RAK4630 BSP](https://github.com/RAKWireless/RAK-nRF52-Arduino)
- Adafruit BME680 Library

![lib-bme680-install](../../../../assets/Arduino/lib-bme680-install.png)

During the installation you will be asked to install the Adafruit Unified Sensor library. Please install it as well.
![lib-bme680-install](../../../../assets/Arduino/lib-bme680-install-2.png)

Connect the assembled hardware to the PC with a USB cable, open the Arduino, and select the development board and port in the tool menu.

![ard-install-21](../../../../assets/Arduino/ard-install-21.png)



## Demo

The Demo is designed to get BME680 sensor data every 5 seconds and print the results on serial port.

```
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h> // Click to install library: http://librarymanager/All#Adafruit_BME680

Adafruit_BME680 bme;
// Might need adjustments
#define SEALEVELPRESSURE_HPA (1010.0)

void bme680_init()
{
  Wire.begin();

  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    return;
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

void bme680_get()
{
  Serial.print("Temperature = ");
  Serial.print(bme.temperature);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Humidity = ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Gas = ");
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(" KOhms");

  Serial.println();
}

void setup()
{
  // Initialize the built in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Initialize Serial for debug output
  Serial.begin(115200);

  time_t serial_timeout = millis();
  // On nRF52840 the USB serial is not available immediately
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    else
    {
      break;
    }
  }

  bme680_init();
}

void loop()
{
  if (! bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  bme680_get();
  delay(5000);
}

```



The test results are as follows：

```
Temperature = 30.66 *C
Pressure = 1013.80 hPa
Humidity = 44.38 %
Gas = 3.89 KOhms

Temperature = 30.69 *C
Pressure = 1013.78 hPa
Humidity = 44.59 %
Gas = 3.88 KOhms

Temperature = 30.72 *C
Pressure = 1013.82 hPa
Humidity = 44.37 %
Gas = 3.87 KOhms

```
