/* 
NMEA2000_due.h

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

#ifndef _NMEA2000_TEENSY_H_
#define _NMEA2000_TEENSY_H_

#include <NMEA2000.h> 
#include <N2kMsg.h>

#include <FlexCAN.h>

class tNMEA2000_teensy : public tNMEA2000
{
protected:
  FlexCAN *CANbus;
  uint16_t DefTimeOut; 
  bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
  bool CANOpen();
  bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);
  
public:
  tNMEA2000_teensy(uint16_t _DefTimeOut=2);
};

#endif
