/**
 * @file connect_ap.ino
 * @author Taylor lee (taylor.lee@rakwireless.com)
 * @brief connect a known AP and PING the IP "8.8.8.8" every 5 seconds
 *    Please see the detailed commands from
 *    https://docs.espressif.com/projects/esp-at/en/latest/AT_Command_Set/index.html
 * @version 0.1
 * @date 2020-07-28
 *
 * @copyright Copyright (c) 2020
 *
 * @note RAK4631 GPIO mapping to nRF52840 GPIO ports
   RAK4631    <->  nRF52840
   WB_IO1     <->  P0.17 (GPIO 17)
   WB_IO2     <->  P1.02 (GPIO 34)
   WB_IO3     <->  P0.21 (GPIO 21)
   WB_IO4     <->  P0.04 (GPIO 4)
   WB_IO5     <->  P0.09 (GPIO 9)
   WB_IO6     <->  P0.10 (GPIO 10)
   WB_SW1     <->  P0.01 (GPIO 1)
   WB_A0      <->  P0.04/AIN2 (AnalogIn A2)
   WB_A1      <->  P0.31/AIN7 (AnalogIn A7)
 */

#define WIFI_SSID "Room-905"
#define WIFI_PASSWORD "Mxjmxj_905"

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
    char cmd[128] = "";
    time_t timeout = millis();
    // Open serial communications and wait for port to open:
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
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
    execute_at(cmd, "OK");
}

/**
   @brief Arduino loop. Runs forever until power off or reset

*/
void loop()
{
    Serial1.println("AT+PING=\"8.8.8.8\"");
    // ping 8.8.8.8
    execute_at("AT+QPING=1,\"8.8.8.8\"\r\n", "OK");
    delay(5000);
}
