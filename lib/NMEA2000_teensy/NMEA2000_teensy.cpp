/* 
NMEA2000_teensy.cpp

2015 Copyright (c) Thomas Sarlandie  All right reserved.
  
Author: Thomas Sarlandie

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
  1301  USA
  
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

