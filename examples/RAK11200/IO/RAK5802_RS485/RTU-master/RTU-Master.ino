/**
 * @file RTU-master.ino
 * @author rakwireless.com
 * @brief master app for RS485 interface example.
 * @version 0.1
 * @date 2020-07-28
 * @copyright Copyright (c) 2020 
 */

#include <ModbusRTU.h>  // Click here to get the library: http://librarymanager/All#modbus-esp8266

ModbusRTU g_mb;

bool cbWrite(Modbus::ResultCode event, uint16_t transactionId, void* data) 
{
  Serial.printf_P("Request result: 0x%02X, Mem: %d\n", event, ESP.getFreeHeap());
  return true;
}

void setup() {
  /*
  * WisBLOCK RAK5802 Power On
  */
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
  Serial1.begin(9600,SERIAL_8N1);    //Data bit 8, check bit None, Stop bit 1
  while (!Serial1);
  g_mb.begin(&Serial1);
  g_mb.master();
  
  Serial.println("MODBUS RTU-Master Init Succeed.");
}

bool coils[20];

void loop() {
  if (!g_mb.slave()) //Returns slave id for active request or 0 if no request in-progress.
  {
    g_mb.readCoil(1, 1, coils, 20, cbWrite);
  }
  g_mb.task();
  yield();
}
