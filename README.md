# KBox Firmware

[![Travis status](https://api.travis-ci.org/sarfata/kbox-firmware.svg)](https://travis-ci.org/sarfata/kbox-firmware)

## What is KBox?

KBox connects your boat networks together and translates message from
one bus to the other. KBox also includes multiple sensors that publish
information on your boat networks. Finally KBox has a screen that can be used to
repeat some of your boat parameters.

![KBox picture](kbox.jpg)

Of course KBox is designed with the [SignalK specification](http://signalk.org)
in mind and will be compatible with it.


[KBox hardware](https://github.com/sarfata/kbox-hardware) includes:

 - One NMEA2000 interface,
 - Two NMEA0183 inputs (one of them can be used as a Seatalk input),
 - Two NMEA0183 outputs,
 - One WiFi interface that can connect to an existing network or create a new
   network on your boat,
 - Three analog inputs to measure voltages between 0 and 23V (batteries, solar
   panels, generators, etc),
 - One shunt input to measure current consumption of your main house battery,
 - One barometer,
 - One inertial measurement unit providing a magnetic compass, an accelerometer
   to measure roll, pitch and yaw as well as a gyroscope,
 - An SDCard to record data.

[KBox is an open-source hardware project](https://github.com/sarfata/kbox-hardware). You can review the
schematics and build it yourself. We also intend to get a few batches
manufactured by a professional board assembly house. If you are interested, put
your name in our form!

[I am interested in an early manufacturing batch](http://goo.gl/forms/y78AeACvyr).

[Find more news about KBox on
hackaday.io](https://hackaday.io/project/11055-kbox).

## What can I do with KBox?

We are still working hard on the software but we anticipate that the first and
most useful use-cases will be:

 - Relay GPS and AIS information to your mobile devices (for example, an iPad
   running [iNavX](http://inavx.com)).
 - Monitor your energy and display voltages and currents on your existing
   NMEA2000 equipment
 - Record your boat position, and all its parameters (speed over ground, speed
   over water, heeling angle, apparent and true wind, etc) to an SDCard to
   replay the race at a later point.

Because KBox is open-source, you can do a lot more things with it:

 - Use KBox as a basic NMEA2000 to USB or Network gateway (for example, for
   SignalK development)
 - Build an open-source autopilot and use KBox hardware as the main sensor and
   computer (we have extension ports to connect to the actuator)
 - etc ...

### Current software status

As of April 2016, the KBox firmware supports:

 - General functionnality
   - Creating a new WiFi network (KBox) or joining an existing network (via simple
     code modification and recompiling)
   - Support up to 8 simultaneous TCP clients connected on port 4242
   - Very basic clock on the screen
 - NMEA0183 reception and forwarding
   - Receiving NMEA0183 sentence from an external source and forwarding them to
     WiFi clients via a TCP socket
 - NMEA2000:
   - Receiving NMEA2000 packets and translating them to `$PCDIN` nmea sentences to
     all WiFi clients. iNavX will decode this just fine as well as a few other
     programs.
 - Sensors
   - Sending nmea2000 bus voltage as well as all three battery banks voltage to
     WiFi clients via $PCDIN nmea 2000 messages, and to the NMEA2000 network.
   - Sending current pressure via nmea 2000 $PCDIN messages to wifi clients and
     to the NMEA2000 network.

Current focus is on improving the translation of key sentences from one network
to the other:

 - Translate NMEA0183 GPS and AIS messages to NMEA2000 messages
 - Translate NMEA0183 messages received via WiFi to NMEA2000 messages (so that
   you can set a waypoint on the iPad and get bearing, distance, time to reach
   information on a NMEA2000 display)
 - Logging everything to SDCard so we can enjoy sailing, collect a lot of data,
   go home and continue working on new conversions from the dry comfort of home.

## KBox firmware overview

KBox is based on the [Teensy 3.2](https://www.pjrc.com/teensy/) architecture and
is compatible with the Arduino development environment. The WiFi module is an
ESP8266-13 and is also programmed with an Arduino compatible SDK.

This project contains the source code for the firmware running on the host
micro-controller (teensy-like Cortex M4 micro-controller) and the firmware
running on the WiFi module.

## Building and flashing KBox

Read the [Developer setup](https://github.com/sarfata/kbox-firmware/wiki/Developer-Setup)
page of the Wiki to learn how to install and run the tools required to program KBox.

## Reporting problems and Sharing suggestions

If you run into problems or would like to suggest new features for this project,
please use the [GitHub issue tracker](https://github.com/sarfata/kbox-firmware).

You can also find me (@sarfata) on the [SignalK Slack
server](http://slack-invite.signalk.org/).

## Contributing

Please post contributions on GitHub, in the form of pull-requests and add your
name to the list of contributors below. We kindly ask that all contributors
share their code under the MIT license. If you wish to share your code under a
different release, please make it a library and post a pull-request to include
your library.

List of contributors:

 - Thomas Sarlandie

## License

The original code of this project is distributed under the MIT license.

Please note that most of the libraries have their own license. Some are under
the GPL. [Because of License compatibility
rules](https://en.wikipedia.org/wiki/License_compatibility#GPL_compatibility),
this means that the full package of the KBox code + these libraries currently
falls under the GPL license.

Contact me if you need a distribution of only the MIT code to re-use it
separately.

 - [Adafruit Sensor](https://github.com/adafruit/Adafruit_BMP280_Library) is
 under the Apache license
 - [Adafruit BMP280](https://github.com/adafruit/Adafruit_BMP280_Library) is under the BSD license
 - [Adafruit BNO055](https://github.com/adafruit/Adafruit_BNO055/) is under the MIT license
 - [ADC library](https://github.com/pedvide/ADC) is a permissive custom license
 - [FlexCAN](https://github.com/teachop/FlexCAN_Library) does not seem to have a
 license [yet](https://github.com/teachop/FlexCAN_Library/issues/12)
 - [i2c_t3](https://github.com/nox771/i2c_t3) is under the LGPL
 - [ILI9341_t3](https://github.com/PaulStoffregen/ILI9341_t3) is under the MIT
   license
 - [NMEA2000](https://github.com/ttlappalainen/NMEA2000/) is under the LGPL
 - [NMEA2000_teensy](https://github.com/sarfata/NMEA2000_teensy) is under the
   LGPL
 - [Time](https://github.com/PaulStoffregen/Time) is under the LGPL
 - [SdFat](https://github.com/greiman/SdFat) is under the GPL
 - [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) is under the LGPL
 - The Teensy core files are distributed under the MIT license with [a specific
   clause](https://forum.pjrc.com/threads/25996-Question-about-licensing-of-core-files).
   The KBox hardware includes [a bootloader chip sold by pjrc.com](http://www.pjrc.com/store/ic_mkl02.html)
   and supports the Teensy project.
 - [ESPTool](https://github.com/themadinventor/esptool) is under the GPL

**[Fair winds and following seas](https://en.wikipedia.org/wiki/Following_sea)
to the authors of these libraries! Without them, this project would not have
been possible!**
