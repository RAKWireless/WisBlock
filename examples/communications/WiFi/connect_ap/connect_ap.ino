/**
 * @file connect_ap.ino
 * @author Taylor lee (taylor.lee@rakwireless.com)
 * @brief connect a known AP and PING the IP "8.8.8.8" every 5 seconds
 * @version 0.1
 * @date 2020-07-28
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

/**
   @brief  execute at command
   @param  at: the at command you want to execute
   @param  expect: the respond you want to get
   @param  timeout: the timout of receive respond
*/
void execute_at(char *at, char *expect = NULL, int32_t timeout = 1000)
{
	String resp = "";

	Serial1.write(at);
	delay(10);

	while (timeout--)
	{
		if (Serial1.available())
		{
			resp += Serial1.readString();
		}
		delay(1);
	}

	Serial.println(resp);
	if (expect != NULL)
	{
		if (resp.indexOf(expect) != -1)
		{
			Serial.println("Execute OK.");
		}
		else
		{
			Serial.println("Execute Fail.");
		}
	}
	resp = "";
}

/**
   @brief Arduino setup function. Called once after power on or reset

*/
void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial)
		delay(10);

	Serial.println("================================");
	Serial.println("RAK2305 WiFI example");
	Serial.println("================================");

	Serial1.begin(115200);
	delay(1000);

	// Set RAK2305 as AP and STA role
	execute_at("AT+CWMODE=3\r\n", "OK");

	// Set contry code
	execute_at("AT+CWCOUNTRY=0,\"CN\",1,13\r\n", "OK");

	// Connect AP with ssid and password
	execute_at("AT+CWJAP=\"Room-905\",\"Mxjmxj_905\"\r\n", "OK");
}

/**
   @brief Arduino loop. Runs forever until power off or reset

*/
void loop()
{
	Serial1.println("AT+PING=\"8.8.8.8\"");
	execute_at("AT+QPING=1,\"8.8.8.8\"\r\n", "OK");
	delay(5000);
}
