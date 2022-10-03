#include <Rak_CAN.h>


void setup () {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.
  delay(300);
  Serial.begin (115200) ;
  delay(1000);
  Rak_Can.init();
  Serial.print ("sizeof (RAK_CAN_Settings): ") ;
  Serial.print (sizeof (RAK_CAN_Settings)) ;
  Serial.println (" bytes") ;
  Serial.println ("Configure ACAN2518") ;
  RAK_CAN_Settings settings (RAK_CAN_Settings::OSC_40MHz, 125 * 1000) ; // CAN bit rate 125 kb/s
  settings.mRequestedMode = RAK_CAN_Settings::Normal20B ;
  const uint32_t errorCode = Rak_Can.begin (settings, [] { Rak_Can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW:") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "yes" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  } else {
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}


static uint32_t gBlinkLedDate = 0 ;
static uint32_t gSentFrameCount = 0 ;


void loop () {
  CANMessage frame ;
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    frame.id = 0x05; //define the id of message
    frame.len = 0x08; //define the length of message
    memset(frame.data, 0x02, 8);
    const bool ok = Rak_Can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      Serial.print ("Sent: ") ;
      for (int i = 0; i < 8; i++)
      {
        Serial.print(frame.data[i]);
        Serial.print(" ");
      }
   
      Serial.print ("SentId: ") ;
      Serial.println(frame.id);

      Serial.println (gSentFrameCount) ;
    } else {
      Serial.println ("Send failure") ;
    }
  }
}
