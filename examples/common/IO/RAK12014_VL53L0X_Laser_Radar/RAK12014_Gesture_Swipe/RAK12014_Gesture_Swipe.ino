/**
   @file RAK12014_Gesture_Swipe.ino
   @author rakwireless.com
   @brief Detect swipe gesture
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include <vl53l0x_class.h>  // Click to install library: http://librarymanager/All#stm32duino_vl53l0x
#include <tof_gestures.h> // Click to install library: http://librarymanager/All#stm32duino_proximity
#include <tof_gestures_SWIPE_1.h>

VL53L0X sensor_vl53l0x(&Wire, WB_IO2); //XSHUT=15

// Gesture structure.
Gesture_SWIPE_1_Data_t gestureSwipeData;

// Range value
uint32_t distance_top;

/**
 *  Setup all sensors for single shot mode
 */
void SetupSingleShot(void){
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  int status;
  uint8_t VhvSettings;
  uint8_t PhaseCal;
  uint32_t refSpadCount;
  uint8_t isApertureSpads;

  status = sensor_vl53l0x.StaticInit();
  if( status ){
    Serial.println("StaticInit sensor failed");
  }

  status = sensor_vl53l0x.PerformRefCalibration(&VhvSettings, &PhaseCal);
  if( status ){
    Serial.println("PerformRefCalibration sensor failed");
  }

  status = sensor_vl53l0x.PerformRefSpadManagement(&refSpadCount, &isApertureSpads);
  if( status ){
    Serial.println("PerformRefSpadManagement sensor failed");
  }

  status = sensor_vl53l0x.SetDeviceMode(VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
  if( status ){
    Serial.println("SetDeviceMode sensor failed");
  }

  status = sensor_vl53l0x.SetMeasurementTimingBudgetMicroSeconds(20*1000);
  if( status ){
    Serial.println("SetMeasurementTimingBudgetMicroSeconds sensor failed");
  }
}

/* Setup ---------------------------------------------------------------------*/

void setup() {
  int status;

  // Initialize serial for output.
  Serial.begin(115200);

  // Initialize I2C bus.
  Wire.begin();

  // Configure VL53L0X component.
  sensor_vl53l0x.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X component.
  status = sensor_vl53l0x.InitSensor(0x52);
  if(status)
  {
    Serial.println("Init sensor_vl53l0x failed...");
  }

  // Initialize gesture library.
  tof_gestures_initSWIPE_1(&gestureSwipeData);

  SetupSingleShot();
}


/* Loop ----------------------------------------------------------------------*/

void loop() {
  int gesture_code;
  int status;

  sensor_vl53l0x.StartMeasurement();

  int top_done = 0;
  uint8_t NewDataReady=0;
  VL53L0X_RangingMeasurementData_t pRangingMeasurementData;

  do
  {
    if(top_done == 0)
    {
      NewDataReady = 0;
      status = sensor_vl53l0x.GetMeasurementDataReady(&NewDataReady);

      if( status ){
        Serial.println("GetMeasurementDataReady sensor failed");
      }

      if(NewDataReady)
      {
        status = sensor_vl53l0x.ClearInterruptMask(0);
        if( status ){
          Serial.println("ClearInterruptMask sensor failed");
        }

        status = sensor_vl53l0x.GetRangingMeasurementData(&pRangingMeasurementData);
        if( status ){
          Serial.println("GetRangingMeasurementData sensor failed");
        }

        if (pRangingMeasurementData.RangeStatus == 0) {
          // we have a valid range.
          distance_top = pRangingMeasurementData.RangeMilliMeter;
        }else {
          distance_top = 1200;
        }

        top_done = 1;
      }
    }
  }while(top_done == 0);

  // Launch gesture detection algorithm.
  gesture_code = tof_gestures_detectSWIPE_1(distance_top, &gestureSwipeData);

  // Check the result of the gesture detection algorithm.
  switch(gesture_code)
  {
    case GESTURES_SINGLE_SWIPE:
      Serial.println("GESTURES_SINGLE_SWIPE DETECTED!!!");
      break;
    default:
      // Do nothing
      break;
  }
}
