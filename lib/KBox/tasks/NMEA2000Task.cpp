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

#include <Debug.h>
#include "../drivers/board.h"
#include "NMEA2000Task.h"

static NMEA2000Task *handlerContext;

static void handler(const tN2kMsg &msg) {
  //DEBUG("Received N2K Message with pgn: %i", msg.PGN);
  handlerContext->publishN2kMessage(msg);
}

void NMEA2000Task::publishN2kMessage(const tN2kMsg& msg) {
  NMEA2000Message m(msg);
  sendMessage(m);
}

void NMEA2000Task::setup() {
  // Make sure the CAN transceiver is enabled.
  pinMode(can_standby, OUTPUT);
  digitalWrite(can_standby, 0);

  NMEA2000.SetProductInformation("00000002", // Manufacturer's Model serial code
                                 100, // Manufacturer's product code
                                 "Simple wind monitor",  // Manufacturer's Model ID
                                 "1.0.0.11 (2015-11-10)",  // Manufacturer's Software version code
                                 "1.0.0.0 (2015-11-10)" // Manufacturer's Model version
                                 );
  //// Det device information
  NMEA2000.SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                130, // Device function=Atmospheric. See codes on http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                55, // Device class=External Environment. See codes on  http://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                2046 // Just choosen free from code list on http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                               );

  handlerContext = this;
  NMEA2000.SetMsgHandler(handler);
  NMEA2000.EnableForward(false);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 22);

  if (NMEA2000.Open()) {
    DEBUG("Initialized NMEA2000");
  }
  else {
    DEBUG("Something went wrong initializing NMEA2000 ... ");
  }
}

void NMEA2000Task::loop() {
  NMEA2000.ParseMessages();

  // Send test message.
  //tN2kMsg N2kMsg;
  //SetN2kWindSpeed(N2kMsg, 1, 5.0, 10.0, N2kWind_Apprent);
  //NMEA2000.SendMsg(N2kMsg);
}
