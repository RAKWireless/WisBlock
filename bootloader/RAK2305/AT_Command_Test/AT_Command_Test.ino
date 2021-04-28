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
    Serial.println("RAK2305 AT CMD TEST!");
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
