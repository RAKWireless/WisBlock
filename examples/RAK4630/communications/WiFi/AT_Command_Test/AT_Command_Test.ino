/**
 * @file AT_Command_Test.ino
 * @author rakwireless.com
 * @brief ESP32 AT command example
 *    Please see the detailed commands from
 *    https://docs.espressif.com/projects/esp-at/en/latest/AT_Command_Set/index.html
 * @version 0.1
 * @date 2020-08-21
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

void setup()
{
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
    Serial.println("ESP32 AT CMD TEST!");
    Serial.println("================================");

    //esp32 init
    Serial1.begin(115200);
}

void loop()
{
    uint8_t resp;
    uint8_t snd;

    // wait for response from esp32
    while (Serial1.available() > 0)
    {
        resp = (uint8_t)(Serial1.read());
        Serial.write(resp);
    }

    // wait for AT cmd which user send with serial tool
    while (Serial.available() > 0)
    {
        snd = (uint8_t)(Serial.read());
        Serial1.write(snd);
    }
}
