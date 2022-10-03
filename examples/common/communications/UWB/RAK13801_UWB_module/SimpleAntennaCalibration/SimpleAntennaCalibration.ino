/**
   @file SimpleAntennaCalibration.ino
   @author rakwireless.com
   @brief Simple Antenna Calibration
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>

// connection pins
const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;

// messages used in the ranging protocol
// TODO replace by enum
#define POLL 0
#define POLL_ACK 1
#define RANGE 2
#define RANGE_REPORT 3
#define RANGE_FAILED 255

#define EXPECTED_RANGE 4.00 // Recommended value for default values, refer to chapter 8.3.1 of DW1000 User manual
#define EXPECTED_RANGE_EPSILON 0.05
#define ACCURACY_THRESHOLD 5
#define ANTENNA_DELAY_STEPS 1

// message flow state
volatile byte expectedMsgId = POLL;
// message sent/received state
volatile boolean sentAck = false;
volatile boolean receivedAck = false;
// protocol error state
boolean protocolFailed = false;

// Antenna calibration variables
int accuracyCounter = 0;
uint16_t antenna_delay = 16637;//16637  16436

// timestamps to remember
uint64_t timePollSent;
uint64_t timePollReceived;
uint64_t timePollAckSent;
uint64_t timePollAckReceived;
uint64_t timeRangeSent;
uint64_t timeRangeReceived;

uint64_t timeComputedRange;
// last computed range/time
// data buffer
#define LEN_DATA 16
byte data[LEN_DATA];
// watchdog and reset period
uint32_t lastActivity;
uint32_t resetPeriod = 250;
// reply times (same on both sides for symm. ranging)
uint16_t replyDelayTimeUS = 3000;
// ranging counter (per second)
uint16_t successRangingCount = 0;
uint32_t rangingCountPeriod = 0;
float samplingRate = 0;

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
  true
};

void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); //Turn on the power switch
  delay(300);
  time_t serial_timeout = millis();
  // DEBUG monitoring
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println(F("### DW1000Ng-arduino-ranging-anchor ###"));
  // initialize the driver
  DW1000Ng::initialize(PIN_SS, PIN_IRQ, PIN_RST);
  Serial.println(F("DW1000Ng initialized ..."));
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  DW1000Ng::setDeviceAddress(1);

  DW1000Ng::setAntennaDelay(antenna_delay);

  Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  Serial.print("Device ID: "); Serial.println(msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  Serial.print("Unique ID: "); Serial.println(msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  Serial.print("Network ID & Device Address: "); Serial.println(msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  Serial.print("Device mode: "); Serial.println(msg);
  // attach callback for (successfully) sent and received messages
  DW1000Ng::attachSentHandler(handleSent);
  DW1000Ng::attachReceivedHandler(handleReceived);
  // anchor starts in receiving mode, awaiting a ranging poll message
  receiver();
  noteActivity();
  // for first time ranging frequency computation
  rangingCountPeriod = millis();
}

void noteActivity() {
  // update activity timestamp, so that we do not reach "resetPeriod"
  lastActivity = millis();
}

void resetInactive() {
  // anchor listens for POLL
  expectedMsgId = POLL;
  receiver();
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

void transmitPollAck() {
  data[0] = POLL_ACK;
  DW1000Ng::setTransmitData(data, LEN_DATA);
  DW1000Ng::startTransmit();
}

void transmitRangeReport(float curRange) {
  data[0] = RANGE_REPORT;
  // write final ranging result
  memcpy(data + 1, &curRange, 4);
  DW1000Ng::setTransmitData(data, LEN_DATA);
  DW1000Ng::startTransmit();
}

void transmitRangeFailed() {
  data[0] = RANGE_FAILED;
  DW1000Ng::setTransmitData(data, LEN_DATA);
  DW1000Ng::startTransmit();
}

void receiver() {
  DW1000Ng::forceTRxOff();
  // so we don't need to restart the receiver manually
  DW1000Ng::startReceive();
}

void loop() {
  int32_t curMillis = millis();
  if (!sentAck && !receivedAck) {
    // check if inactive
    if (curMillis - lastActivity > resetPeriod) {
      resetInactive();
    }
    return;
  }
  // continue on any success confirmation
  if (sentAck) {
    sentAck = false;
    byte msgId = data[0];
    if (msgId == POLL_ACK) {
      timePollAckSent = DW1000Ng::getTransmitTimestamp();
      noteActivity();
    }
    DW1000Ng::startReceive();
  }
  if (receivedAck) {
    receivedAck = false;
    // get message and parse
    DW1000Ng::getReceivedData(data, LEN_DATA);
    byte msgId = data[0];
    if (msgId != expectedMsgId) {
      // unexpected message, start over again (except if already POLL)
      protocolFailed = true;
    }
    if (msgId == POLL) {
      // on POLL we (re-)start, so no protocol failure
      protocolFailed = false;
      timePollReceived = DW1000Ng::getReceiveTimestamp();
      expectedMsgId = RANGE;
      transmitPollAck();
      noteActivity();
    }
    else if (msgId == RANGE) {
      timeRangeReceived = DW1000Ng::getReceiveTimestamp();
      expectedMsgId = POLL;
      if (!protocolFailed) {
        timePollSent = DW1000NgUtils::bytesAsValue(data + 1, LENGTH_TIMESTAMP);
        timePollAckReceived = DW1000NgUtils::bytesAsValue(data + 6, LENGTH_TIMESTAMP);
        timeRangeSent = DW1000NgUtils::bytesAsValue(data + 11, LENGTH_TIMESTAMP);
        // (re-)compute range as two-way ranging is done
        double distance = DW1000NgRanging::computeRangeAsymmetric(timePollSent,
                          timePollReceived,
                          timePollAckSent,
                          timePollAckReceived,
                          timeRangeSent,
                          timeRangeReceived);

        distance = DW1000NgRanging::correctRange(distance);

        String rangeString = "Range: "; rangeString += distance; rangeString += " m";
        rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
        rangeString += "\t Sampling: "; rangeString += samplingRate; rangeString += " Hz";
        Serial.println(rangeString);

        // Antenna delay script
        if (distance >= (EXPECTED_RANGE - EXPECTED_RANGE_EPSILON) && distance <= (EXPECTED_RANGE + EXPECTED_RANGE_EPSILON)) {
          accuracyCounter++;
        } else {
          accuracyCounter = 0;
          antenna_delay += (distance > EXPECTED_RANGE) ? ANTENNA_DELAY_STEPS : -ANTENNA_DELAY_STEPS;
          DW1000Ng::setAntennaDelay(antenna_delay);
        }

        if (accuracyCounter == ACCURACY_THRESHOLD) {
          Serial.print("Found Antenna Delay value (Divide by two if one antenna is set to 0): ");
          Serial.println(antenna_delay);
          delay(10000);
        }
        //Serial.print("FP power is [dBm]: "); Serial.print(DW1000Ng::getFirstPathPower());
        //Serial.print("RX power is [dBm]: "); Serial.println(DW1000Ng::getReceivePower());
        //Serial.print("Receive quality: "); Serial.println(DW1000Ng::getReceiveQuality());
        // update sampling rate (each second)
        transmitRangeReport(distance * DISTANCE_OF_RADIO_INV);
        successRangingCount++;
        if (curMillis - rangingCountPeriod > 1000) {
          samplingRate = (1000.0f * successRangingCount) / (curMillis - rangingCountPeriod);
          rangingCountPeriod = curMillis;
          successRangingCount = 0;
        }
      }
      else {
        transmitRangeFailed();
      }

      noteActivity();
    }
  }
}
