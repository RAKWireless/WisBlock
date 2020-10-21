/**
 * @file Cellular_Ping.ino
 * @author rakwireless.com
 * @brief Example shows how to ping a website
 * @version 0.1
 * @date 2020-08-21
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */

#define BG77_POWER_KEY 17
String bg77_rsp = "";
void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial)
		delay(10); // for nrf52840 with native usb
	Serial.println("RAK4630 Cellular TEST With CHINA UNICOM NBIOT sim card!");

	//BG77 init
	pinMode(BG77_POWER_KEY, OUTPUT);
	digitalWrite(BG77_POWER_KEY, 0);
	delay(1000);
	digitalWrite(BG77_POWER_KEY, 1);
	delay(2000);
	digitalWrite(BG77_POWER_KEY, 0);
	Serial1.begin(115200);
	delay(1000);
	Serial.println("BG77 power up!");
	bg77_at("ATI", 500);
	delay(2000);
	bg77_at("AT+CGDCONT=1,\"IP\",\"snbiot\"", 500);
	delay(2000);
	bg77_at("AT+QCFG=\"nwscanseq\",03,1", 500);
	delay(2000);
	bg77_at("AT+QCFG=\"band\",F,2000095,2000095", 500);
	delay(2000);
	bg77_at("AT+QCFG=\"iotopmode\",1,1", 500);
	delay(2000);
	bg77_at("AT+CFUN=1,0", 500);
	delay(2000);
	bg77_at("AT+CPIN?", 500);
	delay(2000);
	bg77_at("AT+QNWINFO", 500);
	delay(2000);
	bg77_at("AT+QCSQ", 500);
	delay(2000);
	bg77_at("AT+CSQ", 500);
	delay(2000);
	bg77_at("AT+QIACT=1", 3000);
	delay(2000);
}

//this function is suitable for most AT commands of bg96. e.g. bg96_at("ATI")
void bg77_at(char *at, uint16_t timeout)
{
	char tmp[256] = {0};
	int len = strlen(at);
	strncpy(tmp, at, len);
	uint16_t t = timeout;
	tmp[len] = '\r';
	Serial1.write(tmp);
	delay(10);
	while (t--)
	{
		if (Serial1.available())
		{
			bg77_rsp += char(Serial1.read());
		}
		delay(1);
	}
	Serial.println(bg77_rsp);
	bg77_rsp = "";
}

void ping_at(char *at)
{
	char tmp[256] = {0};
	int len = strlen(at);
	uint16_t timeout = 10000;
	strncpy(tmp, at, len);
	tmp[len] = '\r';
	Serial1.write(tmp);
	delay(10);

	while (timeout--)
	{
		if (Serial1.available())
		{
			bg77_rsp += char(Serial1.read());
		}
		delay(1);
	}
	Serial.println(bg77_rsp);
	bg77_rsp = "";
}

void loop()
{
	Serial.println("Begin receive packet from www.baidu.com!");
	ping_at("AT+QPING=1,\"www.baidu.com\"");
	delay(1000);
}
