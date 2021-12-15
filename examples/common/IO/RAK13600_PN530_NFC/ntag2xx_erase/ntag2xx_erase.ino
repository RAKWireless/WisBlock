
/**
   @file ntag2xx_erase.ino
   @author rakwireless.com
   @brief  This example will wait for any NTAG203 or NTAG213 card or tag,and will attempt to erase the user data section of the card (setting all user bytes to 0x00)
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

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an NTAG203 card.  When one is found 'uid' will be populated with
  // the UID, and uidLength will indicate the size of the UUID (normally 7)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 7)
    {
      uint8_t data[32];
      
      // We probably have an NTAG2xx card (though it could be Ultralight as well)
      Serial.println("Seems to be an NTAG2xx tag (7 byte UID)");	  
      
      // NTAG2x3 cards have 39*4 bytes of user pages (156 user bytes),
      // starting at page 4 ... larger cards just add pages to the end of
      // this range:
      
      // See: http://www.nxp.com/documents/short_data_sheet/NTAG203_SDS.pdf

      // TAG Type       PAGES   USER START    USER STOP
      // --------       -----   ----------    ---------
      // NTAG 203       42      4             39
      // NTAG 213       45      4             39
      // NTAG 215       135     4             129
      // NTAG 216       231     4             225      

      Serial.println("");
      Serial.println("Writing 0x00 0x00 0x00 0x00 to pages 4..39");
      Serial.println("");
      for (uint8_t i = 4; i < 39; i++) 
      {
        memset(data, 0, 4);
        success = nfc.ntag2xx_WritePage(i, data);
        
        // Display the current page number
        Serial.print("Page ");
        if (i < 10)
        {
          Serial.print("0");
          Serial.print(i);
        }
        else
        {
          Serial.print(i);
        }
        Serial.print(": ");

        // Display the results, depending on 'success'
        if (success) 
        {
          Serial.println("Erased");
        }
        else
        {
          Serial.println("Unable to write to the requested page!");
        }
      }      
    }
    else
    {
      Serial.println("This doesn't seem to be an NTAG203 tag (UUID length != 7 bytes)!");
    }
    
    // Wait a bit before trying again
    Serial.println("\n\nSend a character to scan another tag!");
    Serial.flush();
    while (!Serial.available());
    while (Serial.available()) {
    Serial.read();
    }
    Serial.flush();    
  }
}

