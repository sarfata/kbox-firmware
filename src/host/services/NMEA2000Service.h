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

#pragma once

#include <N2kMsg.h>
#include <NMEA2000_teensy.h>
#include "common/os/Task.h"
#include "common/signalk/SKHub.h"
#include "common/signalk/SKSubscriber.h"
#include "common/signalk/SKNMEA2000Converter.h"
#include "host/config/NMEA2000Config.h"

class NMEA2000Service : public Task, public SKSubscriber,
  SKNMEA2000Output {
  private:
    const NMEA2000Config &_config;
    SKHub &_hub;
    tNMEA2000_teensy NMEA2000;
    unsigned int _imuSequence;
    LinkedList<SKNMEA2000Output*> _sentenceRepeaters;

    void sendN2kMessage(const tN2kMsg& msg);

    /**
     * Will initialize NMEA2000 and restore NMEA2000 parameters from persistent
     * storage if available.
     */
    void initializeNMEA2000forReceiveAndTransmit();

    /**
     * Initialize NMEA2000 for receiving messages only. No transmission
     * on bus will be possible.
     */
    void initializeNMEA2000forReceiveOnly();

    /**
     * Saves NMEA2000 parameters to persistent storage so they can be restored
     * on reboot.
     */
    void saveNMEA2000Parameters();

    elapsedMillis timeSinceLastParametersSave;

  public:
    NMEA2000Service(NMEA2000Config &config, SKHub &hub) :
      Task("NMEA2000"), _config(config), _hub(hub), _imuSequence(0) {};

    void setup();
    void loop();

    // Helper for the handler who is not a part of this class
    void publishN2kMessage(const tN2kMsg& msg);

    // SKNMEA2000Output
    bool write(const tN2kMsg&) override;

    void updateReceived(const SKUpdate& update);

    /**
     * All incoming NMEA2000 messages will be repeated to repeaters.
     *
     * @param repeater A reference to an object that implements SKNMEA2000Output.
     */
    void addSentenceRepeater(SKNMEA2000Output &repeater);
};
