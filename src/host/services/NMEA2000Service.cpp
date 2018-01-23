/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "NMEA2000Service.h"

#include <KBoxLogging.h>
#include <KBoxHardware.h>
#include <TimeLib.h>
#include "common/stats/KBoxMetrics.h"
#include "common/algo/crc.h"
#include "common/version/KBoxVersion.h"
#include "common/signalk/SKNMEA2000Parser.h"
#include "host/util/PersistentStorage.h"

static NMEA2000Service *handlerContext;

static void handler(const tN2kMsg &msg) {
  DEBUG("Received N2K Message with pgn: %i", msg.PGN);
  handlerContext->publishN2kMessage(msg);
}

void NMEA2000Service::publishN2kMessage(const tN2kMsg& msg) {
  if (_config.rxEnabled) {
    KBoxMetrics.event(KBoxEventNMEA2000MessageReceived);
    NMEA2000Message m(msg, now());
    sendMessage(m);

    SKNMEA2000Parser p(_config.nmea2000Parser);
    //FIXME: Get the time properly here!
    const SKUpdate &update = p.parse(SKSourceInputNMEA2000, msg, SKTime(0));
    if (update.getSize() > 0) {
      _hub.publish(update);
    }
  }
}

void NMEA2000Service::setup() {
  if (_config.rxEnabled || _config.txEnabled) {
    // Make sure the CAN transceiver is enabled.
    digitalWrite(can_standby, 0);

    if (_config.txEnabled) {
      initializeNMEA2000forReceiveAndTransmit();
    }
    else {
      initializeNMEA2000forReceiveOnly();
    }
    _hub.subscribe(this);
  }
}

bool NMEA2000Service::write(const tN2kMsg& msg) {
  bool result = NMEA2000.SendMsg(msg);
  /*
  DEBUG("Sending message on n2k bus - pgn=%i prio=%i src=%i dst=%i len=%i result=%s", msg.PGN, msg.Priority,
      msg.Source,
      msg.Destination, msg.DataLen, result ? "success":"fail");
  */
  if (result) {
    KBoxMetrics.event(KBoxEventNMEA2000MessageSent);
    return true;
  }
  else {
    KBoxMetrics.event(KBoxEventNMEA2000MessageSendError);
    return false;
  }
}

void NMEA2000Service::loop() {
  // Both incoming and outgoing messages are handled by interrupts.

  // Incoming messages are stored in a circular buffer by the NMEA2000 library
  // Right now this buffer is set to 32 messages. At 100% NMEA2000 bus
  // capacity, that would mean the buffer would fill in about 10ms so we should
  // call ParseMessages() at least every 10ms.
  NMEA2000.ParseMessages();

  if (timeSinceLastParametersSave > 1000) {
    saveNMEA2000Parameters();
    timeSinceLastParametersSave = 0;
  }
}

void NMEA2000Service::updateReceived(const SKUpdate& update) {
  if (_config.txEnabled) {
    if (update.getSource().getInput() != SKSourceInputNMEA2000) {
      SKNMEA2000Converter converter;
      converter.convert(update, *this);
    }
  }
}

void NMEA2000Service::initializeNMEA2000forReceiveOnly() {
  NMEA2000.SetMode(tNMEA2000::N2km_ListenOnly);

  handlerContext = this;
  NMEA2000.SetMsgHandler(handler);
  NMEA2000.EnableForward(false);

  if (NMEA2000.Open()) {
    INFO("Initialized NMEA2000 (receive only)");
  }
  else {
    ERROR("Something went wrong initializing NMEA2000 ... ");
  }
}

void NMEA2000Service::initializeNMEA2000forReceiveAndTransmit() {
  uint32_t serialNumber[4] = { SIM_UIDH, SIM_UIDMH, SIM_UIDML, SIM_UIDL };

  char serialNumberString[33];
  snprintf(serialNumberString, sizeof(serialNumberString), "%08lX%08lX%08lX%08lX",
      serialNumber[0], serialNumber[1], serialNumber[2], serialNumber[3]);

  NMEA2000.SetProductInformation(serialNumberString, 1, "KBox", KBOX_VERSION, "KBox");

  // Generate a unique identifier using a CRC32 of the 128bits unique MCU
  // identifier. Only 21 bits will be used.
  uint32_t uniqueId = rc_crc32(0, (char*)&serialNumber, sizeof(serialNumber));

  // Class 25 and Function 130 => Register on network as a PC Gateway.
  // See: http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
  // 42 (manufacturer code) Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
  NMEA2000.SetDeviceInformation(uniqueId, 130, 25, 42);

  struct PersistentStorage::NMEA2000Parameters p;
  if (PersistentStorage::readNMEA2000Parameters(p)) {
    INFO("Reloading NMEA2000Parameters di=%x si=%x source=%x", p.deviceInstance, p.systemInstance, p.n2kSource);

    NMEA2000.SetDeviceInformationInstances(p.deviceInstance & 0x07, p.deviceInstance >> 3, p.systemInstance);
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, p.n2kSource);
  }
  else {
    NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode);
  }

  handlerContext = this;
  NMEA2000.SetMsgHandler(handler);
  NMEA2000.EnableForward(false);

  if (NMEA2000.Open()) {
    INFO("Initialized NMEA2000");
  }
  else {
    ERROR("Something went wrong initializing NMEA2000 ... ");
  }
}

void NMEA2000Service::saveNMEA2000Parameters() {
  if (NMEA2000.ReadResetAddressChanged() || NMEA2000.ReadResetDeviceInformationChanged()) {
    tNMEA2000::tDeviceInformation di = NMEA2000.GetDeviceInformation();

    struct PersistentStorage::NMEA2000Parameters p;
    p.n2kSource = NMEA2000.GetN2kSource();
    p.deviceInstance = di.GetDeviceInstance();
    p.systemInstance = di.GetSystemInstance();

    DEBUG("NMEA2000Parameters have changed. Saving address=%u di=%i si=%i", p.n2kSource, p.deviceInstance, p.systemInstance);
    if (!PersistentStorage::writeNMEA2000Parameters(p)) {
      ERROR("Error saving NMEA2000 parameters to flash.");
    }
  }
}
