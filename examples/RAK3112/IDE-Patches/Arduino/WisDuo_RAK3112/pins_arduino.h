#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#ifndef _VARIANT_RAK3112_
#define _VARIANT_RAK3112_
#endif

#define USB_VID 0x303a
#define USB_PID 0x1001

// GPIO's
#define WB_IO1 21
#define WB_IO2 2
// #define WB_IO2 14
#define WB_IO3 41
#define WB_IO4 42
#define WB_IO5 38
#define WB_IO6 39
// #define WB_SW1 35 NC
#define WB_A0 1
#define WB_A1 2
#define WB_CS 12
#define WB_LED1 46
#define WB_LED2 45

// LEDs
#define PIN_LED1 (46)
#define PIN_LED2 (45)
#define LED_BLUE PIN_LED2
#define LED_GREEN PIN_LED1
#ifdef LED_BUILTIN
#undef LED_BUILTIN
#endif
#define LED_BUILTIN LED_GREEN
#define LED_CONN PIN_LED2
#define LED_STATE_ON 1 // State when LED is litted

/*
 * Analog pins
 */
#define PIN_A0 (21)
#define PIN_A1 (14)

/*
 * Serial interfaces
 */
// TXD1 RXD1 on Base Board
#define PIN_SERIAL1_RX (44)
#define PIN_SERIAL1_TX (43)

// TXD0 RXD0 on Base Board (NC due to large flash chip)
// #define PIN_SERIAL2_RX (19)
// #define PIN_SERIAL2_TX (20)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO (10)
#define PIN_SPI_MOSI (11)
#define PIN_SPI_SCK (13)
#define SPI_CS (12)

static const uint8_t SS = SPI_CS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK = PIN_SPI_SCK;

// Internal SPI to LoRa transceiver
#define LORA_SX126X_SCK 5
#define LORA_SX126X_MISO 3
#define LORA_SX126X_MOSI 6
#define LORA_SX126X_CS 7
#define LORA_SX126X_RESET 8
#define LORA_SX126X_DIO1 47
#define LORA_SX126X_BUSY 48
#define LORA_SX126X_DIO2_AS_RF_SWITCH 1
#define LORA_SX126X_DIO3_TCXO_VOLTAGE 1.8

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 2

#define PIN_WIRE_SDA (9)
#define PIN_WIRE_SCL (40)
#define SDA PIN_WIRE_SDA
#define SCL PIN_WIRE_SCL

#define PIN_WIRE1_SDA (17)
#define PIN_WIRE1_SCL (18)

//??????????????????????????????????????????????
// MESHTASTIC ??????????????????????????????????
//??????????????????????????????????????????????
// static const uint8_t TX = 43;
// static const uint8_t RX = 44;

// static const uint8_t SDA = 9;
// static const uint8_t SCL = 40;

// static const uint8_t SS = 10;
// static const uint8_t MOSI = 11;
// static const uint8_t MISO = 13;
// static const uint8_t SCK = 12;

// static const uint8_t A8 = 9;
// static const uint8_t A9 = 10;
// static const uint8_t A10 = 11;
// static const uint8_t A11 = 12;
// static const uint8_t A12 = 13;
// static const uint8_t A13 = 14;

// static const uint8_t T9 = 9;
// static const uint8_t T10 = 10;
// static const uint8_t T11 = 11;
// static const uint8_t T12 = 12;
// static const uint8_t T13 = 13;
// static const uint8_t T14 = 14;

// // Wire1 for ES7210 MIC ADC, ES8311 I2S DAC, ICM-42607-P IMU and TT21100 Touch Panel
// #define I2C_SDA 8
// #define I2C_SCL 18

// #define ES7210_ADDR    0x40  //MIC ADC
// #define ES8311_ADDR    0x18  //I2S DAC
// #define ICM42607P_ADDR 0x68  //IMU
// #define TT21100_ADDR   0x24  //Touch Panel

// #define TFT_DC   4
// #define TFT_CS   5
// #define TFT_MOSI 6
// #define TFT_CLK  7
// #define TFT_MISO 0
// #define TFT_BL   45
// #define TFT_RST  48

// #define I2S_LRCK 47
// #define I2S_MCLK 2
// #define I2S_SCLK 17
// #define I2S_SDIN 16
// #define I2S_DOUT 15

// #define PA_PIN   46  //Audio Amp Power
// #define MUTE_PIN 1   //MUTE Button
// #define TS_IRQ   3   //Touch Screen IRQ

#endif /* Pins_Arduino_h */
