/**
 * @file RTU-slave.ino
 * @author rakwireless.com
 * @brief master app for RS485 interface example.
 * @version 0.1
 * @date 2020-07-28
 * @copyright Copyright (c) 2020 
 */


#include <ModbusRTU.h>    // Click here to get the library: http://librarymanager/All#modbus-esp8266

#define REGN 10
#define SLAVE_ID 1

ModbusRTU g_mb;

void setup() {
  pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);

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
  Serial1.begin(9600, SERIAL_8N1);
  g_mb.begin(&Serial1);
  g_mb.slave(SLAVE_ID); //Set the device to operate in slave mode
  g_mb.addHreg(REGN);
  g_mb.Hreg(REGN, 1);

  Serial.println("RTU_slave Init Succeed");
}

void loop() {
  g_mb.task();
  yield();
}
