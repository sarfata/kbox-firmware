/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <board.h>
#include "NMEAPage.h"

NMEAPage::NMEAPage() {
  // Make sure both NMEA transceivers are enabled.
  pinMode(nmea1_out_enable, OUTPUT);
  pinMode(nmea2_out_enable, OUTPUT);
  digitalWrite(nmea1_out_enable, 1);
  digitalWrite(nmea2_out_enable, 1);

  // Serial2 is nmea1
  Serial2.begin(4800);

  // Serial 3 is nmea2 or seatalk
#if SEATALK
#ifndef SERIAL_9N1
#error "To get Seatalk support, you must compile with -DSERIAL_9BIT_SUPPORT."
#endif
  // Configure Serial 3 in 9 bits per byte mode (Seatalk)
  // This requires compiling with -DSERIAL_9BIT_SUPPORT (cf HardwareSerial.h)
  Serial3.begin(4800, SERIAL_9N1);
#else
  Serial3.begin(4800);
#endif
  
}

void NMEAPage::willAppear() {
  needsPainting = true;
}

bool NMEAPage::processEvent(const TickEvent &e) {
  char buffer[256];
  static time_ms_t lastSent = 0;


  int available = Serial2.available();
  if (available > 0) {
    int read = Serial2.readBytes(buffer, available);
    buffer[read] = 0;
    DEBUG("NMEA1 %i: %s", read, buffer);
  }
  available = Serial3.available();
  if (available > 0) {
#if SEATALK // used as a nmea0183 port
    char hexBuffer[256];
    int written;

    // For more info on Seatalk format: http://www.thomasknauf.de/rap/seatalk2.htm
    // First byte should have the "cmd" flag set. (the 9th bit)
    // All other bytes should not have it.
    uint16_t commandByte = Serial3.read();
    if (commandByte & 0x100) {
      written = snprintf(hexBuffer, sizeof(hexBuffer), "Cmd: %x - Data: ", commandByte & 0xff);
    }
    else {
      written = snprintf(hexBuffer, sizeof(hexBuffer), "Invalid first byte. %x ", commandByte);
    }

    int read = Serial3.readBytes(buffer, available);
    for (int i = 0; i < read; i++) {
      written += snprintf(hexBuffer + written, sizeof(hexBuffer) - written, "%x ", buffer[i]);
    }
    DEBUG("NMEA2 / Seatalk: %s", hexBuffer);
#else
    int read = Serial3.readBytes(buffer, available);
    buffer[read] = 0;
    DEBUG("NMEA2 %i: %s", read, buffer);
#endif
  }


  if (e.getMillis() > lastSent + 500) {
    DEBUG("Pinging on NMEA1 and NMEA2");
    Serial2.println("NMEA1 PING");
    Serial3.println("NMEA2 PING");
    lastSent = e.getMillis();
  }
  return true;
}

bool NMEAPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    needsPainting = true;
  }
  return true;
}

void NMEAPage::paint(GC &gc) {
  if (!needsPainting)
    return;

  gc.fillRectangle(Point(0, 0), Size(320, 240), ColorBlue);
  gc.drawText(Point(2, 5), FontDefault, ColorWhite, "  NMEA Test Page");

  needsPainting = false;
}
