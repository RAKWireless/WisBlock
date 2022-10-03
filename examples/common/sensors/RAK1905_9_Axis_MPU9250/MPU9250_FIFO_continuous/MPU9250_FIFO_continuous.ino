
/**
   @file MPU9250_FIFO_continuous.ino
   @author rakwireless.com
   @brief This sketch shows how to use the Fifo in the continuous mode.
          The MPU9250 fills the Fifo continuously with acceleration and/or gyroscope data.
          When full, new data replaces the oldest. Reading the Fifo is slightly more complex in
          this mode, because the Fifo buffer does not necessarily start with the first data of an  acceleration or gyroscope data set.
          You define, when writing into Fifo shall stop. In this example the stop is triggered by a wake-on-motion event.
   @version 0.1
   @date 2022-03-17
   @copyright Copyright (c) 2022
**/

#include <MPU9250_WE.h>
#include <Wire.h>
#define MPU9250_ADDR 0x68                              // library: http://librarymanager/All#MPU9250_WE

const int intPin = WB_IO1;   //if use SLOT A intPin use WB_IO1,if use SLOT B,the intPin use WB_IO2. if use SLOT C,the intPin use WB_IO3.  if use SLOT D,the intPin use WB_IO5
volatile bool womEvent = false;

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

  /* The slope of the curve of acceleration vs measured values fits quite well to the theoretical
     values, e.g. 16384 units/g in the +/- 2g range. But the starting point, if you position the
     MPU9250 flat, is not necessarily 0g/0g/1g for x/y/z. The autoOffset function measures offset
     values. It assumes your MPU9250 is positioned flat with its x,y-plane. The more you deviate
     from this, the less accurate will be your results.
     The function also measures the offset of the gyroscope data. The gyroscope offset does not
     depend on the positioning.
     This function needs to be called at the beginning since it can overwrite your settings!
  */
  Serial.println("Position you MPU9250 flat and don't move it - calibrating...");
  delay(1000);
  myMPU9250.autoOffsets();
  Serial.println("Done!");

  /*  This is a more accurate method for calibration. You have to determine the minimum and maximum
      raw acceleration values of the axes determined in the range +/- 2 g.
      You call the function as follows: setAccOffsets(xMin,xMax,yMin,yMax,zMin,zMax);
      Use either autoOffset or setAccOffsets, not both.
  */
  //myMPU9250.setAccOffsets(-14240.0, 18220.0, -17280.0, 15590.0, -20930.0, 12080.0);

  /*  The gyroscope data is not zero, even if you don't move the MPU9250.
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
  myMPU9250.setGyrDLPF(MPU9250_DLPF_5);

  /*  Sample rate divider divides the output rate of the gyroscope and accelerometer.
      Sample rate = Internal sample rate / (1 + divider)
      It can only be applied if the corresponding DLPF is enabled and 0<DLPF<7!
      Divider is a number 0...255
  */
  myMPU9250.setSampleRateDivider(99);

  /*  MPU9250_GYRO_RANGE_250       250 degrees per second (default)
      MPU9250_GYRO_RANGE_500       500 degrees per second
      MPU9250_GYRO_RANGE_1000     1000 degrees per second
      MPU9250_GYRO_RANGE_2000     2000 degrees per second
  */
  myMPU9250.setGyrRange(MPU9250_GYRO_RANGE_250);

  /*  MPU9250_ACC_RANGE_2G      2 g   (default)
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
  myMPU9250.setAccDLPF(MPU9250_DLPF_0);

  /*  Set the interrupt pin:
      MPU9250_ACT_LOW  = active-low
      MPU9250_ACT_HIGH = active-high (default)
  */
  //myMPU9250.setIntPinPolarity(MPU9250_ACT_LOW);

  /*  If latch is enabled the interrupt pin level is held until the interrupt status
      is cleared. If latch is disabled the interrupt pulse is ~50µs (default).
  */
  myMPU9250.enableIntLatch(true);

  /*  The interrupt can be cleared by any read or it will only be cleared if the interrupt
      status register is read (default).
  */
  //myMPU9250.enableClearIntByAnyRead(true);

  /*  Enable/disable interrupts:
      MPU9250_DATA_READY
      MPU9250_FIFO_OVF
      MPU9250_WOM_INT

      You can enable all interrupts.
  */
  myMPU9250.enableInterrupt(MPU9250_WOM_INT);
  //myMPU9250.disableInterrupt(MPU9250_FIFO_OVF);

  /*  Set the wake on motion threshold (WOM threshold)
      Choose 1 (= 4 mg) ..... 255 (= 1020 mg);
  */
  myMPU9250.setWakeOnMotionThreshold(225); // Interrupt bei 0.9 g

  /*  Enable/disable wake on motion (WOM) and  WOM mode:
      MPU9250_WOM_DISABLE
      MPU9250_WOM_ENABLE
   *  ***
      MPU9250_WOM_COMP_DISABLE   // reference is the starting value
      MPU9250_WOM_COMP_ENABLE    // reference is tha last value
  */
  myMPU9250.enableWakeOnMotion(MPU9250_WOM_ENABLE, MPU9250_WOM_COMP_DISABLE);

  /* There are two different FIFO modes:
      MPU9250_CONTINUOUS --> samples are continuously stored in FIFO. If FIFO is full
      new data will replace the oldest.
      MPU9250_STOP_WHEN_FULL --> self-explaining
  */
  //myMPU9250.setFifoMode(MPU9250_STOP_WHEN_FULL); // used below, but explained here

  /* The argument of startFifo defines the data stored in the FIFO
     MPU9250_FIFO_ACC --> Acceleration Data ist stored in FIFO
     MPU9250_FIFO_GYR --> Gyroscope data is stored in FIFO
     MPU9250_FIFO_ACC_GYR --> Acceleration and Gyroscope Data is stored in FIFO
     The library does not (yet) support storing single gyroscope axes data, temperature
     or data from slaves.
  */
  //myMPU9250.startFifo(MPU9250_FIFO_ACC); // used below, but explained here

  /* stopFifo():
     - stops additional writes into Fifo
     - clears the data type written into Fifo (acceleration and/or gyroscope
  */
  //myMPU9250.stopFifo(); // used below, but explained here

  /* sets the Fifo counter to zero */
  //myMPU9250.resetFifo(); // used below, but explained here


  /* sleep() sends the MPU9250 to sleep or wakes it up.
     Please note that the gyroscope needs 35 milliseconds to wake up.
  */
  //myMPU9250.sleep(true);

  /* If cycle is set, and standby or sleep are not set, the module will cycle between
     sleep and taking a sample at a rate determined by setLowPowerAccDataRate().
  */
  //myMPU9250.enableCycle(true);

  /* This is a low power standby mode for the gyro function, which allows quick enabling.
     (see data sheet for further information)
  */
  //myMPU9250.enableGyrStandby(true);


  /* You can enable or disable the axes for gyroscope and/or accelerometer measurements.
     By default all axes are enabled. Parameters are:
     MPU9250_ENABLE_XYZ  //all axes are enabled (default)
     MPU9250_ENABLE_XY0  // X, Y enabled, Z disabled
     MPU9250_ENABLE_X0Z
     MPU9250_ENABLE_X00
     MPU9250_ENABLE_0YZ
     MPU9250_ENABLE_0Y0
     MPU9250_ENABLE_00Z
     MPU9250_ENABLE_000  // all axes disabled
  */
  //myMPU9250.enableAccAxes(MPU9250_ENABLE_X0Z);
  //myMPU9250.enableGyrAxes(MPU9250_ENABLE_XY0);

  attachInterrupt(digitalPinToInterrupt(intPin), eventISR, RISING);
  myMPU9250.setFifoMode(MPU9250_CONTINUOUS);
  myMPU9250.enableFifo(true);
  delay(100); // in some cases a delay after enabling Fifo makes sense
  myMPU9250.startFifo(MPU9250_FIFO_ACC_GYR);

  Serial.println("Turn your MPU9250 around the x or y axis.");
  Serial.println("Waiting for wake-on-motion event...");
}

void loop() {
  if (womEvent) {
    myMPU9250.stopFifo();
    printFifo();
    myMPU9250.resetFifo();
    myMPU9250.startFifo(MPU9250_FIFO_ACC_GYR);

    Serial.println("For another series of measurements, enter any key and send");

    while (!(Serial.available())) {}
    Serial.read();
    Serial.println();
    myMPU9250.readAndClearInterrupts();
    womEvent = false;
  }
}

void printFifo() {
  myMPU9250.findFifoBegin(); /* this is needed for continuous Fifo mode. The Fifo buffer ends with a
  complete data set, but the start is within a data set. 512/6 or 512/12 */
  int count = myMPU9250.getFifoCount();
  int dataSets = myMPU9250.getNumberOfFifoDataSets();
  Serial.print("Bytes in Fifo: ");
  Serial.println(count);
  Serial.print("Data Sets: ");
  Serial.println(dataSets);

  for (int i = 0; i < dataSets; i++) {
    /* if you read acceleration (g) and gyroscope values you need to start with g values */
    xyzFloat gValue = myMPU9250.getGValuesFromFifo();
    xyzFloat gyr = myMPU9250.getGyrValuesFromFifo();
    Serial.print("Data set ");
    Serial.print(i + 1);
    Serial.println(":");

    Serial.print(gValue.x);
    Serial.print("   ");
    Serial.print(gValue.y);
    Serial.print("   ");
    Serial.println(gValue.z);

    Serial.print(gyr.x);
    Serial.print("   ");
    Serial.print(gyr.y);
    Serial.print("   ");
    Serial.println(gyr.z);
  }
}

void eventISR() {
  womEvent = true;
}
