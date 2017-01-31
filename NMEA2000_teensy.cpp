/*
  The MIT License

  Copyright (c) 2015-2017 Thomas Sarlandie thomas@sarlandie.net

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

/*
  Inherited NMEA2000 object for Teensy internal CAN
  based setup. See also NMEA2000 library.
  Requires FlexCAN library.
*/

#include <NMEA2000_teensy.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

//*****************************************************************************
tNMEA2000_teensy::tNMEA2000_teensy(uint16_t _DefTimeOut) : tNMEA2000() {
  CANbus = new FlexCAN(250000);
  DefTimeOut=_DefTimeOut;
}

//*****************************************************************************
bool tNMEA2000_teensy::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent) {
  CAN_message_t out;
  out.id = id;
  out.ext = 1;
  out.len = len;
  out.timeout = (wait_sent?DefTimeOut:0);

  for (int i=0; i<len && i<8; i++) out.buf[i] = buf[i];

  return CANbus->write(out) == 1;

  // FIXME: The Arduino Due had a retry mechanism here. Check whether we need it on Teensy.
  /*
  do {
    if (TryCount>0) delay(3);
    result=CAN.sendFrame(output);
    TryCount++;
  } while (TryCount<2 && !result);
    */
}

//*****************************************************************************
bool tNMEA2000_teensy::CANOpen() {
  CANbus->begin();
  return true;
}

//*****************************************************************************
bool tNMEA2000_teensy::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf) {
  CAN_message_t incoming;

  if (CANbus->available() > 0) {
    CANbus->read(incoming);
    id = incoming.id;
    len = incoming.len;
    for (int i = 0; i < len && i < 8; i++) {
      buf[i] = incoming.buf[i];
    }
    return true;
  }
  else {
    return false;
  }
}

