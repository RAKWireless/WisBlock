/**
 * @file Read_Battery_Level.ino
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Read charging level from a battery connected to the RAK5005-O base board
 * Setup and main loop
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
#include <Arduino.h>
#include <bluefruit.h>

#define PIN_VBAT WB_A0

uint32_t vbat_pin = PIN_VBAT;

/*
 * Battery conversion from raw ADC value to single-precision mV
 *  Vref == 3000.0 mV (For Vref 3.0V, includes V to mV conversion)
 *  ADCresolution == 4096.0 (For 12-bit resolution, 2^12)
 *  RAK5005 VBatt divider factor == 5/3 (reciprocal of divider value)
 *
 * VBatt (mv) = ADC_Value * Vref / ADCresolution * 5 / 3 ->
 * VBatt (mv) = (ADC_Value * 3000.0 * 5.0) / (4096.0 * 3.0) ->
 * VBatt (mv) = (ADC_Value * 15000.0) / 12288.0
 */
#define ADC_TO_VBAT(ADC_Value)  ((ADC_Value) * 15000.0f) / 12288.0f

#include <bluefruit.h>
void ble_init();
extern bool g_bleUARTisConnected;
extern BLEUart g_bleUart;

char g_bleString[256] = {0};

void initDisplay(void);
void dispAddLine(char *line);
char g_dispText[256] = {0};

/**
 * @brief Get RAW Battery Voltage
 */
float readVBAT(void)
{
    float raw;

    // Might want to calibrate SAADC offset on each call
	// analogCalibrateOffset();

    // Get the raw 12-bit, 0..3000mV ADC value
    raw = analogRead(vbat_pin);

    return ADC_TO_VBAT(raw);
}

/**
 * @brief Convert from raw mv to percentage
 * @param mvolts
 *    RAW Battery Voltage
 */
uint8_t mvToPercent(float mvolts)
{
    if (mvolts < 3300)
        return 0;

    if (mvolts < 3600)
    {
        mvolts -= 3300;
        return mvolts / 30;
    }

    mvolts -= 3600;
    return 10 + (mvolts * 0.15F); // thats mvolts /6.66666666
}

/**
 * @brief get LoRaWan Battery value
 * @param mvolts
 *    Raw Battery Voltage
 */
uint8_t mvToLoRaWanBattVal(float mvolts)
{
    if (mvolts < 3300)
        return 0;

    if (mvolts < 3600)
    {
        mvolts -= 3300;
        return mvolts / 30 * 2.55;
    }

    mvolts -= 3600;
    return (10 + (mvolts * 0.15F)) * 2.55;
}

void setup()
{
    initDisplay();

    Serial.begin(115200);
    dispAddLine("Waiting for Serial");
    time_t serialTimeout = millis();
    while (!Serial)
    {
        delay(10); // for nrf52840 with native usb
        if ((millis() - serialTimeout) > 5000)
        {
            break;
        }
    }

    Serial.println("=====================================");
    Serial.println("RAK4631 Battery test");
    Serial.println("=====================================");

    // Start BLE
    dispAddLine("Init BLE");
    ble_init();

    // Set the analog reference to 3.0V (default = 3.6V)
    analogReference(AR_INTERNAL_3_0);

    // Set the resolution to 12-bit (0..4095)
    analogReadResolution(12); // Can be 8, 10, 12 or 14

    // Set analog sample time to 10x VBatt divider + sample cap time-constant
    analogSampleTime(15);

    // Set oversampling to 32x to improve SNR
    analogOverSampling(32); // 1, 2, 4, 8, 16, 32, 64, 128, 256

    // Let the ADC settle
    delay(1);

    // Internal calibration for SAADC offset voltage
    // Ideally do this periodically
	analogCalibrateOffset();

    // Get a single ADC sample and throw it away
    readVBAT();
}

void loop()
{
    // Get a raw ADC reading
    float vbat_mv = readVBAT();

    // Convert from raw mv to percentage (based on LIPO chemistry)
    uint8_t vbat_per = mvToPercent(vbat_mv);

    // Display the results

    snprintf(g_dispText, 255, "%.3fV \t %d%%", vbat_mv / 1000, vbat_per);
    dispAddLine(g_dispText);

    Serial.print("LIPO = ");
    Serial.print(vbat_mv);
    Serial.print(" mV (");
    Serial.print(vbat_per);
    Serial.print("%) LoRaWan Batt ");
    Serial.println(mvToLoRaWanBattVal(vbat_mv));

    if (g_bleUARTisConnected)
    {
        snprintf(g_bleString, 255, "%.0fmV %d%% %d/255\n", vbat_mv, vbat_per, mvToLoRaWanBattVal(vbat_mv));
        g_bleUart.print(g_bleString);
    }

    delay(1000);
}
