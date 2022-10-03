
/**
   @file MPU9250_calibration.ino
   @author rakwireless.com
   @brief  This sketch can be used to determine the (non-auto) offsets for the MPU9250.
		   It does not use the internal offset registers.
           For the gyroscope offsets just use the raw values you obtain when the module is not moved.
		   For the accelerometer offsets turn the MPU9250 slowly(!) in all directions and find the minimum and maximum raw values for all axes.
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
  Serial.println("RAK1905 Test!");
  /*  This is a more accurate method for calibration than the autoOffset. You have to
      determine the minimum and maximum raw acceleration values of the axes determined
      in the range +/- 2 g.
      You call the function as follows: setAccOffsets(xMin,xMax,yMin,yMax,zMin,zMax);
      Use either autoOffset or setAccOffsets, not both.
  */
  //myMPU9250.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

  /*  The gyroscope data is not zero, even if don't move the MPU9250.
      To start at zero, you can apply offset values. These are the gyroscope raw values you obtain
      using the +/- 250 degrees/s range.
      Use either autoOffset or setGyrOffsets, not both.
  */
  //myMPU9250.setGyrOffsets(45.0, 145.0, -105.0);

  /*  You can enable or disable the digital low pass filter (DLPF). If you disable the DLPF, you
       need to select the bandwidth, which can be either 8800 or 3600 Hz. 8800 Hz has a shorter delay,
       but higher noise level. If DLPF is disabled, the output rate is 32 kHz.
       MPU9250_BW_WO_DLPF_3600
       MPU9250_BW_WO_DLPF_8800
  */
  myMPU9250.enableGyrDLPF();
  //myMPU9250.disableGyrDLPF(MPU9250_BW_WO_DLPF_8800); // bandwidth without DLPF

  /*  Digital Low Pass Filter for the gyroscope must be enabled to choose the level.
      MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7

      DLPF    Bandwidth [Hz]   Delay [ms]   Output Rate [kHz]
        0         250            0.97             8
        1         184            2.9              1
        2          92            3.9              1
        3          41            5.9              1
        4          20            9.9              1
        5          10           17.85             1
        6           5           33.48             1
        7        3600            0.17             8

        You achieve lowest noise using level 6
  */
  myMPU9250.setGyrDLPF(MPU9250_DLPF_6);  // lowest noise

  /*  MPU9250_GYRO_RANGE_250       250 degrees per second (default)
      MPU9250_GYRO_RANGE_500       500 degrees per second
      MPU9250_GYRO_RANGE_1000     1000 degrees per second
      MPU9250_GYRO_RANGE_2000     2000 degrees per second
  */
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);

  /*  MPU9250_ACC_RANGE_2G      2 g
      MPU9250_ACC_RANGE_4G      4 g
      MPU9250_ACC_RANGE_8G      8 g
      MPU9250_ACC_RANGE_16G    16 g
  */
  myMPU9250.setAccRange(MPU9250_ACC_RANGE_2G);

  /*  Enable/disable the digital low pass filter for the accelerometer
       If disabled the bandwidth is 1.13 kHz, delay is 0.75 ms, output rate is 4 kHz
  */
  myMPU9250.enableAccDLPF(true);

  /*  Digital low pass filter (DLPF) for the accelerometer, if enabled
      MPU9250_DPLF_0, MPU9250_DPLF_2, ...... MPU9250_DPLF_7
       DLPF     Bandwidth [Hz]      Delay [ms]    Output rate [kHz]
         0           460               1.94           1
         1           184               5.80           1
         2            92               7.80           1
         3            41              11.80           1
         4            20              19.80           1
         5            10              35.70           1
         6             5              66.96           1
         7           460               1.94           1
  */
  myMPU9250.setAccDLPF(MPU9250_DLPF_6);  // lowest noise

  Serial.print("Turn your MPU9250 slowly(!) in all directions to determine ");
  Serial.println(" the min/max raw acceleration values.");
  Serial.println("For the gyroscope offsets just note the gyro raw values for the unmoved sensor");
  delay(1000);
}

void loop() {
  xyzFloat accRaw;
  xyzFloat gyrRaw;
  xyzFloat corrAccRaw;
  xyzFloat corrGyrRaw;
  accRaw = myMPU9250.getAccRawValues();
  gyrRaw = myMPU9250.getGyrRawValues();
  corrAccRaw = myMPU9250.getCorrectedAccRawValues();
  corrGyrRaw = myMPU9250.getCorrectedGyrRawValues();

  Serial.println("Acceleration raw values without offset:");
  Serial.print(accRaw.x);
  Serial.print("   ");
  Serial.print(accRaw.y);
  Serial.print("   ");
  Serial.println(accRaw.z);

  Serial.println("Gyroscope raw values without offset:");
  Serial.print(gyrRaw.x);
  Serial.print("   ");
  Serial.print(gyrRaw.y);
  Serial.print("   ");
  Serial.println(gyrRaw.z);

  Serial.println("Acceleration raw values with offset:");
  Serial.print(corrAccRaw.x);
  Serial.print("   ");
  Serial.print(corrAccRaw.y);
  Serial.print("   ");
  Serial.println(corrAccRaw.z);

  Serial.println("Gyroscope raw values with offset:");
  Serial.print(corrGyrRaw.x);
  Serial.print("   ");
  Serial.print(corrGyrRaw.y);
  Serial.print("   ");
  Serial.println(corrGyrRaw.z);

  delay(2000);
}
