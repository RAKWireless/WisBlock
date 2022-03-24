/**
   @file soil.cpp
   @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
   @brief Soil sensor initialization and readings
   @version 0.1
   @date 2021-08-17

   @copyright Copyright (c) 2021

*/
#include "app.h"

/** Sensor */
RAK12035 sensor;

soil_data_s g_soil_data;

struct calib_values_s
{
  uint16_t dry_cal = 75;
  uint16_t wet_cal = 250;
};

calib_values_s calib_values;

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
using namespace Adafruit_LittleFS_Namespace;

static const char soil_name[] = "SOIL";

File soil_file(InternalFS);

uint8_t read_fail_counter = 0;

bool init_soil(void)
{
  // Check if sensors is available
  bool found_sensor = false;
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(WB_IO5, INPUT);

  Wire.begin();

  // Initialize the sensor
  sensor.begin();

  uint8_t data = 0;
  uint16_t value = 0;

  // Check the sensor version
  if (!sensor.get_sensor_version(&data))
  {
    MYLOG("SOIL", "No sensor found");
  }
  else
  {
    MYLOG("SOIL", "Sensor FW version %d", data);
    found_sensor = true;
  }

  // Check the sensor calibration values
  if (!sensor.get_dry_cal(&value))
  {
    MYLOG("SOIL", "No Dry calibration");
  }
  else
  {
    MYLOG("SOIL", "Sensor Dry Cal %d", value);
    found_sensor = true;
  }

  // Check the sensor calibration values
  if (!sensor.get_wet_cal(&value))
  {
    MYLOG("SOIL", "No Wet calibration");
  }
  else
  {
    MYLOG("SOIL", "Sensor Wet Cal %d", value);
    found_sensor = true;
  }

  // #define CAL_TEST

#ifdef CAL_TEST
  for (int i = 0; i < 100; i++)
  {
    MYLOG("SOIL", "Read cycle %d", i);
    // Check the sensor calibration values
    uint16_t value = 0;
    if (!sensor.get_dry_cal(&value))
    {
      MYLOG("SOIL", "No Dry calibration");
    }
    else
    {
      MYLOG("SOIL", "Sensor Dry Cal %d", value);
    }

    // Check the sensor calibration values
    if (!sensor.get_wet_cal(&value))
    {
      MYLOG("SOIL", "No Wet calibration");
    }
    else
    {
      MYLOG("SOIL", "Sensor Wet Cal %d", value);
    }

    MYLOG("SOIL", "Powercycle Sensor");
    sensor.sensor_sleep();
    digitalWrite(WB_IO2, LOW);
    delay(500);
    digitalWrite(WB_IO2, HIGH);
    delay(500);
    sensor.reset();
  }
#endif

  sensor.sensor_sleep();

  Wire.end();

  return found_sensor;
}

void read_soil(void)
{
  uint16_t sensTemp = 0;
  uint8_t sensHumid = 0;
  uint32_t avgTemp = 0;
  uint32_t avgHumid = 0;
  uint16_t sensCap = 0;
  uint32_t avgCap = 0;

  // Wake up the sensor
  Wire.begin();
  if (!sensor.sensor_on())
  {
    MYLOG("SOIL", "Can't wake up sensor");
    if (g_ble_uart_is_connected)
    {
      g_ble_uart.println("Can't wake up sensor");
    }
    g_soil_data.temp_1 = 0xFF;
    g_soil_data.temp_2 = 0xFF;

    g_soil_data.humid_1 = 0xFF;

    g_soil_data.valid = 0;

    Wire.end();

    read_fail_counter++;

    if (read_fail_counter == 5)
    {
      read_fail_counter = 0;
      delay(1000);
      NVIC_SystemReset();
    }
    return;
  }

  // Get the sensor values
  bool got_value = false;
  for (int retry = 0; retry < 3; retry++)
  {
    if (sensor.get_sensor_moisture(&sensHumid) && sensor.get_sensor_temperature(&sensTemp))
    {
      got_value = true;
      retry = 4;
      avgTemp = sensTemp;
      avgHumid = sensHumid;
      sensor.get_sensor_capacitance(&sensCap);

      delay(250);
      for (int avg = 0; avg < 50; avg++)
      {
        delay(250);
        if (sensor.get_sensor_temperature(&sensTemp))
        {
          avgTemp += sensTemp;
          avgTemp /= 2;
        }

        if (sensor.get_sensor_moisture(&sensHumid))
        {
          avgHumid += sensHumid;
          avgHumid /= 2;
        }

        if (sensor.get_sensor_capacitance(&sensCap))
        {
          avgCap += sensCap;
          avgCap /= 2;
        }
      }
    }
  }

  MYLOG("SOIL", "Sensor reading was %s", got_value ? "success" : "unsuccessful");
  MYLOG("SOIL", "T %.2f H %ld C %ld", (double)(avgTemp / 10.0), avgHumid, avgCap);

  if (g_ble_uart_is_connected)
  {
    g_ble_uart.printf("Sensor reading was %s\n", got_value ? "success" : "unsuccessful");
    g_ble_uart.printf("T %.2f H %ld C %ld\n", (double)(avgTemp / 10.0), avgHumid, avgCap);
  }

  avgHumid = avgHumid * 2.0;

  g_soil_data.temp_1 = (uint8_t)(avgTemp >> 8);
  g_soil_data.temp_2 = (uint8_t)(avgTemp);

  g_soil_data.humid_1 = (uint8_t)(avgHumid);

  g_soil_data.cap_1 = (uint8_t)(avgCap >> 8);
  g_soil_data.cap_2 = (uint8_t)(avgCap);

  if (got_value)
  {
    g_soil_data.valid = 1;
  }
  else
  {
    g_soil_data.valid = 0;
  }
  sensor.sensor_sleep();

  Wire.end();
}

uint16_t start_calib(bool is_dry)
{
  MYLOG("SOIL", "Starting calibration for %s", is_dry ? "dry" : "wet");
  Serial.flush();
  uint16_t new_reading = 0;
  uint16_t new_value = 0;
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);

  // Stop app timer while we do calibration
  api_timer_stop();

  Wire.begin();

  if (!sensor.sensor_on())
  {
    MYLOG("SOIL", "Can't wake up sensor");
    Wire.end();

    if (g_lorawan_settings.send_repeat_time != 0)
    {
      // Calibration finished, restart the timer that will wakeup the loop frequently
      api_timer_restart(g_lorawan_settings.send_repeat_time);
    }

    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);

    if (is_dry)
    {
      return 0xFFFF;
    }
    else
    {
      return 0xFFFF;
    }
  }

  sensor.get_sensor_capacitance(&new_value);

  for (int readings = 0; readings < 100; readings++)
  {
    sensor.get_sensor_capacitance(&new_reading);
    new_value += new_reading;
    new_value = new_value / 2;
    delay(250);
    digitalToggle(LED_GREEN);
    digitalToggle(LED_BLUE);
  }

  // Send calibration value
  if (is_dry)
  {
    MYLOG("SOIL", "Dry calibration value %d", new_value);
    sensor.set_dry_cal(new_value);
    calib_values.dry_cal = new_value;
  }
  else
  {
    MYLOG("SOIL", "Wet calibration value %d", new_value);
    sensor.set_wet_cal(new_value);
    calib_values.wet_cal = new_value;
  }

  if (g_lorawan_settings.send_repeat_time != 0)
  {
    // Calibration finished, restart the timer that will wakeup the loop frequently
    api_timer_restart(g_lorawan_settings.send_repeat_time);
  }

  // Return the result

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
  sensor.sensor_sleep();
  Wire.end();

  return new_value;
}

uint16_t get_calib(bool is_dry)
{
  uint16_t value = 0;
  Wire.begin();
  sensor.sensor_on();
  if (is_dry)
  {
    if (!sensor.get_dry_cal(&value))
    {
      MYLOG("SOIL", "No Dry calibration");
    }
    else
    {
      MYLOG("SOIL", "Sensor Dry Cal %d", value);
    }
  }
  else
  {
    if (!sensor.get_wet_cal(&value))
    {
      MYLOG("SOIL", "No Wet calibration");
    }
    else
    {
      MYLOG("SOIL", "Sensor Wet Cal %d", value);
    }
  }
  sensor.sensor_sleep();
  Wire.end();
  return value;
}
