#include <Rak_CAN.h>

void setup () {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.
  delay(300);
  Serial.begin (115200) ;

  Rak_Can.init();
  //--- Configure ACAN2517
  Serial.print ("sizeof (RAK_CAN_Settings): ") ;
  Serial.print (sizeof (RAK_CAN_Settings)) ;
  Serial.println (" bytes") ;
  Serial.println ("Configure ACAN2518") ;
  RAK_CAN_Settings settings (RAK_CAN_Settings::OSC_40MHz, 125 * 1000) ; // CAN bit rate 125 kb/s
  settings.mRequestedMode = RAK_CAN_Settings::InternalLoopBack ; // Select loopback mode
  //--- Default values are too high for an Arduino Uno that contains 2048 bytes of RAM: reduce them
  settings.mDriverTransmitFIFOSize = 1 ;
  settings.mDriverReceiveFIFOSize = 1 ;
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
  delay(100);
}


static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;


void loop () {
  CANMessage frame ;
#if BOARDFLAG
  if (Rak_Can.IntrruptFlag == 1)
#endif
  {
    Rak_Can.isr_core();
    if (Rak_Can.available ()) {
      Rak_Can.receive (frame) ;
      gReceivedFrameCount ++ ;
      Serial.print ("Received: ") ;
      Serial.println (gReceivedFrameCount) ;
    }
#if BOARDFLAG
    Rak_Can.IntrruptFlag = 0;
#endif
  }
  const bool ok = Rak_Can.tryToSend (frame) ;
  if (ok) {
    gSentFrameCount += 1 ;
    Serial.print ("Sent: ") ;
    Serial.println (gSentFrameCount) ;
  } else {
    Serial.println ("Send failure") ;
  }
  delay(2000);
}
