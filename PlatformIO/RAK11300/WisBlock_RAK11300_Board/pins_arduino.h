#pragma once
#include <macros.h>
#include <stdint.h>

#ifndef __PINS_ARDUINO__
#define __PINS_ARDUINO__

#ifdef __cplusplus
extern "C" unsigned int PINCOUNT_fn();
#endif

#define F_CPU 64000000U		  //this is a debug marco for RAK15001
static const uint8_t A5 = -1; //this is a debug marco for RAK5802
static const uint8_t A6 = -1; //this is a debug marco for RAK5802
// Pin count
// ----
#define PINS_COUNT (PINCOUNT_fn())
#define NUM_DIGITAL_PINS (30u)
#define NUM_ANALOG_INPUTS (4u)
#define NUM_ANALOG_OUTPUTS (0u)

extern PinName digitalPinToPinName(pin_size_t P);

static const uint8_t WB_IO1 = 6;  // SLOT_A SLOT_B
static const uint8_t WB_IO2 = 22; // SLOT_A SLOT_B
static const uint8_t WB_IO3 = 7;  // SLOT_C
static const uint8_t WB_IO4 = 28; // SLOT_C
static const uint8_t WB_IO5 = 9;  // SLOT_D
static const uint8_t WB_IO6 = 8;  // SLOT_D
static const uint8_t WB_A0 = 26;  // IO_SLOT
static const uint8_t WB_A1 = 27;  // IO_SLOT

// LEDs
#define PIN_LED1 (23)
#define PIN_LED2 (24)

#define LED_BUILTIN PIN_LED1
#define LED_CONN PIN_LED2

#define LED_GREEN PIN_LED1
#define LED_BLUE PIN_LED2

// Analog pins
// -----------
#define PIN_A0 (26u)
#define PIN_A1 (27u)
#define PIN_A2 (28u)
#define PIN_A3 (29u)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;

#define ADC_RESOLUTION 12

// Serial
#define PIN_SERIAL1_TX (0ul)
#define PIN_SERIAL1_RX (1ul)

#define PIN_SERIAL2_RX (5ul)
#define PIN_SERIAL2_TX (4ul)

// SPI
#define PIN_SPI_MISO (16u)
#define PIN_SPI_MOSI (19u)
#define PIN_SPI_SCK (18u)
#define PIN_SPI_SS (17u)

static const uint8_t SS = PIN_SPI_SS; // SPI Slave SS not used. Set here only for reference.
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK = PIN_SPI_SCK;

// Wire
// #define WIRE_HOWMANY        2
#define PIN_WIRE_SDA (2u)
#define PIN_WIRE_SCL (3u)

// #define I2C_SDA        PIN_WIRE_SDA
// #define I2C_SCL        PIN_WIRE_SCL

#define PIN_WIRE_SDA1 (20u)
#define PIN_WIRE_SCL1 (21u)

// #define I2C_SDA1        PIN_WIRE_SDA1
// #define I2C_SCL1        PIN_WIRE_SCL1

#define SERIAL_HOWMANY 2
#define SERIAL1_TX (digitalPinToPinName(PIN_SERIAL1_TX))
#define SERIAL1_RX (digitalPinToPinName(PIN_SERIAL1_RX))

#define SERIAL2_TX (digitalPinToPinName(PIN_SERIAL2_TX))
#define SERIAL2_RX (digitalPinToPinName(PIN_SERIAL2_RX))

#define SERIAL_CDC 1
#define HAS_UNIQUE_ISERIAL_DESCRIPTOR
#define BOARD_VENDORID 0x2e8a
#define BOARD_PRODUCTID 0x00c0
#define BOARD_NAME "WisBlock RAK11300 Board"

void eraseDataFlash(void);
void writeDataToFlash(uint8_t *data);
void getUniqueDeviceID(uint8_t *id);
uint8_t getUniqueSerialNumber(uint8_t *name);
void _ontouch1200bps_();

#define SPI_HOWMANY (1)
#define SPI_MISO (digitalPinToPinName(PIN_SPI_MISO))
#define SPI_MOSI (digitalPinToPinName(PIN_SPI_MOSI))
#define SPI_SCK (digitalPinToPinName(PIN_SPI_SCK))

#define WIRE_HOWMANY (2)
#define I2C_SDA (digitalPinToPinName(PIN_WIRE_SDA))
#define I2C_SCL (digitalPinToPinName(PIN_WIRE_SCL))
#define I2C_SDA1 (digitalPinToPinName(PIN_WIRE_SDA1))
#define I2C_SCL1 (digitalPinToPinName(PIN_WIRE_SCL1))

#define digitalPinToPort(P) (digitalPinToPinName(P) / 32)

#define SERIAL_PORT_USBVIRTUAL SerialUSB
#define SERIAL_PORT_MONITOR SerialUSB
#define SERIAL_PORT_HARDWARE Serial1
#define SERIAL_PORT_HARDWARE_OPEN Serial1

#define USB_MAX_POWER (500)

#endif //__PINS_ARDUINO__
