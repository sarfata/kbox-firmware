# NMEA2000_teensy

This library provides a Teensy driver for the NMEA2000 library.

See https://github.com/ttlappalainen/NMEA2000.


## Usage

This library requires the [FlexCAN](https://github.com/teachop/FlexCAN_Library). Make sure you have it installed.


    #include <NMEA2000.h>
    #include <N2kMessages.h>
    #include <NMEA2000_teensy.h>

    tNMEA2000_teensy NMEA2000;

    void setup() {
      NMEA2000.open();
    }

    void loop() {
    }

See the [NMEA2000](https://github.com/ttlappalainen/NMEA2000) for more examples. They are all compatible with this library.

## Hardware

This driver only works with Teensy 3.1/3.2.

You will need a CAN transceiver like the MCP2551 (for a list of other tested CAN controllers, refer to the [FlexCAN library README](https://github.com/teachop/FlexCAN_Library).

|      Teensy       |       MCP2551       | NMEA2000 Cable |
| ----------------- | ------------------- | -------------- |
|        TX         |         TxD         |                |
|        RX         |         RxD         |                |
|                   |        CANH         |      Blue      |
|                   |        CANL         |     White      |
|        Gnd        |         Gnd         |     Black      |
| Vin (5v when usb) |         VDD         |                |
|        Gnd        |         Rs          |                |
|                   | Vref (disconnected) |                |

## License

    2015 Copyright (c) Thomas Sarlandie. All right reserved.

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
