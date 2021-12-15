
/**
   @file FeliCa_card_detection.ino
   @author rakwireless.com
   @brief This example will attempt to connect to an FeliCa card or tag and retrieve some basic information about it that can be used to determine what type of card it is. out the data and read from the card at the same time!
   @version 0.1
   @date 2021-10-14
   @copyright Copyright (c) 2021
**/

#include <Wire.h>
#include <SPI.h>
#include <RAK13600-PN532.h> // Click here to get the library: http://librarymanager/All#RAK13600-PN532

// If using the breakout or shield with I2C, define just the pins connected
#define PN532_IRQ   (WB_IO6)
#define PN532_RESET (WB_IO5)  // Not connected by default on the NFC Shield

// Or use this line for a breakout or shield with an I2C connection:
NFC_PN532 nfc(PN532_IRQ, PN532_RESET);

uint8_t        _prevIDm[8];
unsigned long  _prevTime;

void setup(void)
{
  Serial.begin(115200);
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  while (!Serial) delay(10);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN53x board");
    while (1) {
      delay(10);
    };      // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata >> 8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();

  memset(_prevIDm, 0, 8);
  delay(1000);
  Serial.println("*******************************************");
}


void loop(void)
{
  uint8_t ret;
  uint16_t systemCode = 0xFFFF;
  uint8_t requestCode = 0x01;       // System Code request
  uint8_t idm[8];
  uint8_t pmm[8];
  uint16_t systemCodeResponse;

  // Wait for an FeliCa type cards.
  // When one is found, some basic information such as IDm, PMm, and System Code are retrieved.
  Serial.println("Waiting for an FeliCa card...  ");
  ret = nfc.felica_Polling(systemCode, requestCode, idm, pmm, &systemCodeResponse, 5000);

  if (ret != 1)
  {
    Serial.println("Could not find a card");
    delay(500);
    return;
  }

  if ( memcmp(idm, _prevIDm, 8) == 0 ) {
    if ( (millis() - _prevTime) < 3000 ) {
      Serial.println("Same card");
      delay(500);
      return;
    }
  }

  Serial.println("Found a card!");
  Serial.print("  IDm: ");
  nfc.PrintHex(idm, 8);
  Serial.print("  PMm: ");
  nfc.PrintHex(pmm, 8);
  Serial.print("  System Code: ");
  Serial.print(systemCodeResponse, HEX);
  Serial.print("\n");

  memcpy(_prevIDm, idm, 8);
  _prevTime = millis();

  // Wait 1 second before continuing
  Serial.println("Card access completed!\n");
  delay(1000);
}
