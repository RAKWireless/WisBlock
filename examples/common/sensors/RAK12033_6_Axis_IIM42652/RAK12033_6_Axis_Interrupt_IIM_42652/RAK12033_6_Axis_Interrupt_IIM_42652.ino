/**
   @file RAK12033_6_Axis_Interrupt_IIM_42652.ino
   @author rakwireless.com
   @brief  Trigger an interrupt when the acceleration on the axis exceeds the threshold.
   @version 0.1
   @date 2022-01-01
   @copyright Copyright (c) 2022
**/

#include "RAK12033-IIM42652.h"

/*
 * Range [0g : 1g]; Resolution 1g/256=~3.9mg.
 */
#define ACCEL_X_THR 200
#define ACCEL_Y_THR 200
#define ACCEL_Z_THR 200

#define INT_PIN  WB_IO1 // Slot A
//#define INT_PIN  WB_IO3 // Slot C
//#define INT_PIN  WB_IO5 // Slot D

IIM42652 IMU;

bool g_awake = true;           // Global variable to keep track.
bool g_interruptFlag = false;  // Global variabl to keep track of new interrupts. Only ever set true by ISR.

void setup() 
{
  delay(1000);
  time_t timeout = millis();
  // Initialize Serial for debug output.
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
  pinMode(INT_PIN, INPUT); // setup for interrupt
  attachInterrupt(digitalPinToInterrupt(INT_PIN), int1_ISR, FALLING);
  
  Serial.println("RAK12033 Interrupt example.");

  Wire.begin();
  if (IMU.begin() == false) 
  {
    while (1) 
    {
      Serial.println("IIM-42652 is not connected.");
      delay(5000);
    }
  }
  IMU.accelerometer_enable();
  IMU.gyroscope_enable();
  IMU.temperature_enable();

  IMU.set_accel_fsr( IIM42652_ACCEL_CONFIG0_FS_SEL_16g );
  IMU.set_accel_frequency( IIM42652_ACCEL_CONFIG0_ODR_50_HZ );

  IMU.enable_accel_low_power_mode();
  IMU.wake_on_motion_configuration( ACCEL_X_THR, ACCEL_Y_THR, ACCEL_Z_THR); 
}

void loop() 
{
  IIM42652_axis_t  accel_data;
  IIM42652_axis_t  gyro_data;
  float temp;
  uint8_t int_status;

  float acc_x ,acc_y ,acc_z;
  float gyro_x,gyro_y,gyro_z;

  if (g_interruptFlag == true) // Sensor is awake (this variable is only ever set true in int1_ISR)
  {
    int_status = IMU.get_WOM_INT();
    if( int_status & WOM_X_INT)
    {
      Serial.println("X detected.");  
    }
    if( int_status & WOM_Y_INT)
    {
      Serial.println("Y detected.");  
    }
    if( int_status & WOM_Z_INT)
    {
      Serial.println("Z detected.");  
    }
    g_interruptFlag = false;
    IMU.get_accel_data(&accel_data );
    IMU.get_gyro_data(&gyro_data ); 
    IMU.get_temperature(&temp );
  
    /*
     * ±16 g  : 2048  LSB/g 
     * ±8 g   : 4096  LSB/g 
     * ±4 g   : 8192  LSB/g 
     * ±2 g   : 16384 LSB/g 
     */
    acc_x = (float)accel_data.x / 2048;
    acc_y = (float)accel_data.y / 2048;
    acc_z = (float)accel_data.z / 2048;
      
    Serial.print("Accel X: ");
    Serial.print(acc_x);
    Serial.print("[g]  Y: ");
    Serial.print(acc_y);
    Serial.print("[g]  Z: ");
    Serial.print(acc_z);
    Serial.println("[g]");

    /*
     * ±2000 º/s    : 16.4   LSB/(º/s)
     * ±1000 º/s    : 32.8   LSB/(º/s)
     * ±500  º/s    : 65.5   LSB/(º/s)
     * ±250  º/s    : 131    LSB/(º/s)
     * ±125  º/s    : 262    LSB/(º/s)
     * ±62.5  º/s   : 524.3  LSB/(º/s)
     * ±31.25  º/s  : 1048.6 LSB/(º/s) 
     * ±15.625 º/s  : 2097.2 LSB/(º/s)
     */
    gyro_x = (float)gyro_data.x / 16.4;
    gyro_y = (float)gyro_data.y / 16.4;
    gyro_z = (float)gyro_data.z / 16.4;
    
    Serial.print("Gyro  X:");
    Serial.print(gyro_x);
    Serial.print("º/s  Y: ");
    Serial.print(gyro_y);
    Serial.print("º/s  Z: ");
    Serial.print(gyro_z);
    Serial.println("º/s");

    Serial.print("Temper : ");
    Serial.print(temp);
    Serial.println("[ºC]");
  }
}

void int1_ISR()
{
  g_interruptFlag = true;
}
