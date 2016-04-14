# KBox Firmware

## What is KBox?

KBox connects your boat networks together and translates message from
one bus to the other. KBox also includes multiple sensors that publish
information on your boat networks. Finally KBox has a screen that can be used to
repeat some of your boat parameters.

![KBox picture](kbox.jpg)

Of course KBox is designed with the [SignalK specification](http://signalk.org)
in mind and will be compatible with it.


KBox hardware includes:

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

KBox is an open-source hardware project. You can review the schematics and build
it yourself. We also intend to get a few batches manufactured by a professional
board assembly house. If you are interested, put your name in our form!

[I am interested in an early manufacturing batch](http://goo.gl/forms/y78AeACvyr).

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

However, because KBox is open-source, you can do a lot more things with it:

 - Use KBox as a basic NMEA2000 to USB or Network gateway (for example, for
   SignalK development)
 - Build an open-source autopilot and use KBox hardware as the main sensor and
   computer (we have extension ports to connect to the actuator)
 - etc ...

## KBox firmware overview

KBox is based on the [Teensy 3.2](https://www.pjrc.com/teensy/) architecture and
is compatible with the Arduino development environment. The WiFi module is an
ESP8266-13 and is also programmed with an Arduino compatible SDK.

This project contains the source code for the firmware running on the host
micro-controller (teensy-like Cortex M4 micro-controller) and the firmware
running on the WiFi module.

## Building and flashing KBox

To build the project, we use the [platformio](http://platformio.org) tool. It is
compatible with Linux, Mac OS and Windows.

To compile both firmwares, install platformio and then run:

    $ platformio run

To flash the wifi module you will first need to load a special program in the
teensy:

    $ platformio run -e program-esp

At this point, the top led of the board will go blue, signaling that it is ready
to be programmed.

Compile the ESP8266 firmware with:

    $ platformio run -e esp

And use [esptool.py](https://github.com/themadinventor/esptool) to flash it:

    $ esptool.py --port /dev/ttyACM0 write_flash 0x0 .pioens/esp/firmware.bin

While flashing, the top led of KBox will go red and the running light will blink
as data is transmitted.

Finally, flash the real KBox firmware to the teensy:

    $ platfomio run -e host

If you have `make` available (Linux/Mac) you can do all of this in one command:

    $ make wifi host


## Reporting problems and Sharing suggestions

If you run into problems or would like to suggest new features for this project,
please use the [GitHub issue tracker](https://github.com/sarfata/kbox-firmware).

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

**[Fair winds and following seas](https://en.wikipedia.org/wiki/Following_sea)
to the authors of these libraries! Without them, this project would not have
been possible!**

# Advanced topics

## Using OpenOCD to debug KBox

KBox includes a 10 pin standardized Cortex debug connector. This connector
allows us to connect a debugger and truly debug the code running on the board.

We recommend using a CMSIS-DAP compatible SWD/JTAG connector. The [Armstart
IBDAP-CMSIS-DAP](ibdap) is only $19 and widely available.

Follow the [Armstart instructions](ibdap) to compile OpenOCD with CMSIS-DAP
support.

To run OpenOCD, simply type:

    $ openocd -f openocd.cfg
    Open On-Chip Debugger 0.10.0-dev-00191-gae8cdc1 (2016-01-16-20:05)
    Licensed under GNU GPL v2
    For bug reports, read
            http://openocd.org/doc/doxygen/bugs.html
    Info : only one transport option; autoselect 'swd'
    Info : add flash_bank kinetis k40.flash
    adapter speed: 1000 kHz
    none separate
    cortex_m reset_config sysresetreq
    adapter speed: 1000 kHz
    srst_only separate srst_nogate srst_open_drain connect_deassert_srst
    Info : add flash_bank kinetis pflash.0
    Info : CMSIS-DAP: SWD  Supported
    Info : CMSIS-DAP: JTAG Supported
    Info : CMSIS-DAP: Interface Initialised (SWD)
    Info : CMSIS-DAP: FW Version = 1.0
    Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 1 TDO = 1 nTRST = 0 nRESET = 1
    Info : CMSIS-DAP: Interface ready
    Info : clock speed 1000 kHz
    Info : SWD IDCODE 0x2ba01477
    Info : k40.cpu: hardware has 6 breakpoints, 4 watchpoints
    Info : MDM: Chip is unsecured. Continuing.

Now open a different terminal to flash or debug the program.

### Flashing a firmware

    $ platformio run
    [ ... building the .elf file ... ]
    $ arm-none-eabi-objcopy -O binary .pioenvs/teensy31/firmware.elf
    firmware.bin
    $ telnet localhost 4444
    Trying 127.0.0.1...
    Connected to localhost.
    Escape character is '^]'.
    Open On-Chip Debugger
    > reset halt
    MDM: Chip is unsecured. Continuing.
    k40.cpu: target state: halted
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x000001bc msp: 0x20008000
    > flash write_image erase firmware.bin 0x0
    auto erase enabled
    Probing flash info for bank 0
    flash configuration field erased, please reset the device
    wrote 79872 bytes from file firmware.bin in 4.921926s (15.847 KiB/s)
    >
    >
    > reset run
    MDM: Chip is unsecured. Continuing.

### Debugging a firmware

    $ arm-none-eabi-gdb .pioenvs/teensy31/firmware.elf
    GNU gdb (GNU Tools for ARM Embedded Processors) 7.10.1.20151217-cvs
    Copyright (C) 2015 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "--host=x86_64-unknown-linux-gnu --target=arm-none-eabi".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
    <http://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.
    For help, type "help".
    Type "apropos word" to search for commands related to "word"...
    Reading symbols from .pioenvs/teensy31/firmware.elf...done.
    (gdb) target remote localhost:3333
    Remote debugging using localhost:3333
    0x0000a534 in ILI9341_t3::waitTransmitComplete (this=0x1fff9d88, mcr=2149515264)
        at .pioenvs/teensy31/ILI9341_t3/ILI9341_t3.h:254
    254                             uint32_t sr = KINETISK_SPI0.SR;
    (gdb) break EncoderTestPage.cpp:25
    Breakpoint 1 at 0x958: file src/EncoderTestPage.cpp, line 25.
    (gdb) c
    Continuing.
    Note: automatically using hardware breakpoints for read-only addresses.

    Breakpoint 1, EncoderTestPage::processEvent (this=0x1fff9e20, e=...) at src/EncoderTestPage.cpp:26
    26        if (longClickTrigger && millis() > longClickTrigger) {
    (gdb) p longClickTrigger
    $1 = 0
    (gdb) n
    25      bool EncoderTestPage::processEvent(const TickEvent &e) {
    (gdb) step
    26        if (longClickTrigger && millis() > longClickTrigger) {
    (gdb)
    31        timer = e.getMillis();
    (gdb) p timer
    $2 = 5834
    (gdb)

To load a firmware with GDB, simply use the `load` command:

    (gdb) load
    Loading section .text, size 0x125ac lma 0x0
    Loading section .fini, size 0x4 lma 0x125ac
    Loading section .ARM.exidx, size 0x8 lma 0x125b0
    Loading section .data, size 0xe4c lma 0x125b8
    Start address 0x0, load size 78852
    Transfer rate: 14 KB/sec, 9856 bytes/write.
    (gdb)

ibdap: https://github.com/Armstart-com/IBDAP-CMSIS-DAP

