/**
   @file TwoWayRangingInitiator.ino
   @author rakwireless.com
   @brief TwoWay Ranging Initiator
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>

int antenna_delay = 16436;
// connection pins
const uint8_t PIN_SS = SS;  // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;

// messages used in the ranging protocol
// TODO replace by enum
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255
// message flow state
volatile byte expectedMsgId = POLL_ACK;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// timestamps to remember
uint64_t timePollSent;
uint64_t timePollAckReceived;
uint64_t timeRangeSent;
// data buffer
#define LEN_DATA 16
byte data[LEN_DATA];
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 250;
// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 5000;

device_configuration_t DEFAULT_CONFIG = {
  false,
  true,
  true,
  true,
  false,
  SFDMode::STANDARD_SFD,
  Channel::CHANNEL_5,
  DataRate::RATE_850KBPS,
  PulseFrequency::FREQ_16MHZ,
  PreambleLength::LEN_256,
  PreambleCode::CODE_3
};

interrupt_configuration_t DEFAULT_INTERRUPT_CONFIG = {
  true,
  true,
  true,
  false,
  true,
  true
};

void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  //Turn on the power switch
  delay(300);
  time_t serial_timeout = millis();
  // DEBUG monitoring
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - serial_timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  Serial.println(F("### DW1000Ng-arduino-ranging-tag ###"));
  // initialize the driver
  DW1000Ng::initialize(PIN_SS, PIN_IRQ, PIN_RST);
  //  DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
  Serial.println("DW1000Ng initialized ...");
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  //  DW1000Ng::setDeviceAddress(1);
  DW1000Ng::setNetworkId(10);

  DW1000Ng::setAntennaDelay(antenna_delay);  //16417   16551   16436

  Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  Serial.print("Device ID: ");
  Serial.println(msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  Serial.print("Unique ID: ");
  Serial.println(msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  Serial.print("Network ID & Device Address: ");
  Serial.println(msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  Serial.print("Device mode: ");
  Serial.println(msg);
  // attach callback for (successfully) sent and received messages
  DW1000Ng::attachSentHandler(handleSent);
  DW1000Ng::attachReceivedHandler(handleReceived);
  // anchor starts by transmitting a POLL message
  transmitPoll();
  noteActivity();
}

void noteActivity() {
  // update activity timestamp, so that we do not reach "resetPeriod"
  lastActivity = millis();
}

void resetInactive() {
  // tag sends POLL and listens for POLL_ACK
  expectedMsgId = POLL_ACK;
  DW1000Ng::forceTRxOff();
  usb_reset();
  transmitPoll();
  noteActivity();
}

void handleSent() {
  // status change on sent success
  sentAck = true;
}

void handleReceived() {
  // status change on received success
  receivedAck = true;
}

void transmitPoll() {
  data[0] = POLL;
  DW1000Ng::setTransmitData(data, LEN_DATA);
  DW1000Ng::startTransmit();
}

void transmitRange() {
  data[0] = RANGE;
  /* Calculation of future time */
  byte futureTimeBytes[LENGTH_TIMESTAMP];

  timeRangeSent = DW1000Ng::getSystemTimestamp();
  timeRangeSent += DW1000NgTime::microsecondsToUWBTime(replyDelayTimeUS);
  DW1000NgUtils::writeValueToBytes(futureTimeBytes, timeRangeSent, LENGTH_TIMESTAMP);
  DW1000Ng::setDelayedTRX(futureTimeBytes);
  timeRangeSent += DW1000Ng::getTxAntennaDelay();

  DW1000NgUtils::writeValueToBytes(data + 1, timePollSent, LENGTH_TIMESTAMP);
  DW1000NgUtils::writeValueToBytes(data + 6, timePollAckReceived, LENGTH_TIMESTAMP);
  DW1000NgUtils::writeValueToBytes(data + 11, timeRangeSent, LENGTH_TIMESTAMP);
  DW1000Ng::setTransmitData(data, LEN_DATA);
  DW1000Ng::startTransmit(TransmitMode::DELAYED);
  //Serial.print("Expect RANGE to be sent @ "); Serial.println(timeRangeSent.getAsFloat());
}

void loop() {
  if (!sentAck && !receivedAck) {
    // check if inactive
    if (millis() - lastActivity > resetPeriod) {
      resetInactive();
    }
    return;
  }
  // continue on any success confirmation
  if (sentAck) {
    sentAck = false;
    DW1000Ng::startReceive();
  }

  if (receivedAck) {
    receivedAck = false;
    // get message and parse
    DW1000Ng::getReceivedData(data, LEN_DATA);
    byte msgId = data[0];
    //    Serial.printf("data[0]=%d--expectedMsgId=%d\r\n", data[0], expectedMsgId);
    if (msgId != expectedMsgId) {
      // unexpected message, start over again
      Serial.print("Received wrong message # ");
      Serial.println(msgId);
      expectedMsgId = POLL_ACK;
      transmitPoll();
      return;
    }
    if (msgId == POLL_ACK) {
      timePollSent = DW1000Ng::getTransmitTimestamp();
      timePollAckReceived = DW1000Ng::getReceiveTimestamp();
      expectedMsgId = RANGE_REPORT;
      transmitRange();
      noteActivity();
    } else if (msgId == RANGE_REPORT) {
      expectedMsgId = POLL_ACK;
      float curRange;
      memcpy(&curRange, data + 1, 4);
      transmitPoll();
      noteActivity();
    } else if (msgId == RANGE_FAILED) {
      expectedMsgId = POLL_ACK;
      transmitPoll();
      noteActivity();
    }
  }
}


void usb_reset(void) {
  //  DW1000Ng::clearReceiveStatus();
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  DW1000Ng::setAntennaDelay(antenna_delay);

}
