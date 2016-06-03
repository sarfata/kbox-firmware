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

#include <KBox.h>
#include <NMEA2000.h>
#include "MfgTest.h"

class N2KTest;

static N2KTest *handlerContext;
static void handler(const tN2kMsg &msg);


class N2KTest : public MfgTest {
  public:
    N2KTest(KBox& kbox) : MfgTest(kbox, "NMEA2000Test", 10000) {};
    tNMEA2000_teensy NMEA2000;
    int rxMessages = 0;

    void setup() {
      pinMode(can_standby, OUTPUT);
      digitalWrite(can_standby, 0);

      NMEA2000.SetProductInformation("2", // Manufacturer's Model serial code
                                     // Manufacturer's product code
                                     1,
                                     // Manufacturer's Model ID
                                     "KBox",
                                     // Manufacturer's Software version code
                                     "0.0.1 (2016-04-01)",
                                     // Manufacturer's Model version
                                     "v1 revB (2016-02-01)"
                                     );
      //// Set device information
      NMEA2000.SetDeviceInformation(2, // Unique number. Use e.g. Serial number.
        // Class 25, Function 130 => Register on network as a PC Gateway.
        // See: http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
        130, 25,
        // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
        42
      );

      handlerContext = this;
      NMEA2000.SetMsgHandler(handler);
      NMEA2000.EnableForward(false);
      NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 22);

      if (!NMEA2000.Open()) {
        fail("Something went wrong initializing NMEA2000 ... ");
      }
      setInstructions("Waiting for NMEA2000 packets");
    };

    void loop() {
      NMEA2000.ParseMessages();
      if (rxMessages > 5) {
        tN2kMsg msg;
        // creeate a fake barometer message
        SetN2kPGN130311(msg, 0, N2kts_OutsideTemperature, CToKelvin(23.0),
          N2khs_OutsideHumidity, 42, 1024.0);
        if (NMEA2000.SendMsg(msg)) {
          // Give some time to make sure the address claim is finished otherwise this will fail.
          delay(2000);
          pass();
        }
        else {
          fail("Error sending message");
        }
      }
      setMessage("Received " + String(rxMessages) + " NMEA2000 messages.");
    };

    void receivedN2kMessage(const tN2kMsg& msg) {
      rxMessages++;
    };
};

static void handler(const tN2kMsg &msg) {
  handlerContext->receivedN2kMessage(msg);
}
