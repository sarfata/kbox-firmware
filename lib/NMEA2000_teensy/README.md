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
