/**
   @file RAK12033_6_Axis_Calibration_IIM_42652.ino
   @author rakwireless.com
   @brief  Perform zero offset calibration on the gyro output within 5 seconds after reset.
					 Keep the gyro absolutely still during calibration.
           Only the zero offset of the gyroscope is calibrated.
           Using the arithmetic mean calibration method.
           Can be changed to a calibration method more suitable for your application.
   @version 0.1
   @date 2022-01-01
   @copyright Copyright (c) 2022
**/
#include "RAK12033-IIM42652.h"

IIM42652 IMU;

/*
 * @brief Gyro XYZ axis zero offset value.
 */
float g_Xoffset;
float g_Yoffset;
float g_Zoffset;

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
  
  Serial.println("RAK12033 Set Range Example.");

  Wire.begin();
  if (IMU.begin() == false) 
  {
    while (1) 
    {
      Serial.println("IIM-42652 is not connected.");
      delay(5000);
    }
  }

  /*
    @Reference data type: IIM42652_ACCEL_CONFIG0_FS_SEL_t
   */
  IMU.set_accel_fsr(IIM42652_ACCEL_CONFIG0_FS_SEL_16g);

  /*
    @Reference data type:  IIM42652_ACCEL_CONFIG0_ODR_t
   */
  IMU.set_accel_frequency(IIM42652_ACCEL_CONFIG0_ODR_50_HZ);

  /*
    @Reference data type:  IIM42652_GYRO_CONFIG0_FS_SEL_t
   */
  IMU.set_gyro_fsr(IIM42652_GYRO_CONFIG0_FS_SEL_16dps);
  
  /*
    @Reference data type:  IIM42652_GYRO_CONFIG0_ODR_t
   */
  IMU.set_gyro_frequency(IIM42652_GYRO_CONFIG0_ODR_50_HZ);

  IMU.accelerometer_enable();
  IMU.gyroscope_enable();
  IMU.temperature_enable();
	delay(200);
 
	Serial.println("Start calibration, please make sure the gyroscope is still.");
	getOffset(&g_Xoffset , &g_Yoffset , &g_Zoffset);
  Serial.println("Get zero offset completed.");
}

void loop() 
{
  IIM42652_axis_t  accel_data;
  IIM42652_axis_t  gyro_data;
  float temp;

  float acc_x ,acc_y ,acc_z;
  float gyro_x,gyro_y,gyro_z;

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
  gyro_x = (float)(gyro_data.x / 2097.2) - g_Xoffset;
  gyro_y = (float)(gyro_data.y / 2097.2) - g_Yoffset;
  gyro_z = (float)(gyro_data.z / 2097.2) - g_Zoffset;
  
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

  delay(200);
}

/*
 * @breif Get the offset value of the gyroscope X, Y, Z axis.
 *				Arithmetic mean filtering method.
 * @param  xOffset	: X-axis offset value pointer.
 * @param  yOffset	: Y-axis offset value pointer.
 * @param  zOffset	: Z-axis offset value pointer.
 */
void getOffset(float *xOffset , float *yOffset , float *zOffset)
{
	uint8_t count = 0;
	float sumx, sumy, sumz;
	IIM42652_axis_t  gyro_data;
	time_t timeout = millis();
	while((millis() - timeout) < 5000)
	{
		IMU.get_gyro_data( &gyro_data );
   
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
		sumx += (float)(gyro_data.x / 2097.2);
		sumy += (float)(gyro_data.y / 2097.2);
		sumz += (float)(gyro_data.z / 2097.2);
		count++;
		delay(100);
	}
	*xOffset = sumx / count;
	*yOffset = sumy / count;
	*zOffset = sumz / count;
  Serial.print("xOffset:");
  Serial.print(*xOffset);
  Serial.print(" , yOffset: ");
  Serial.print(*yOffset);
  Serial.print(" , zOffset: ");
  Serial.println(*zOffset);
}
