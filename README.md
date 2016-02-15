# Blue Nexus Firmware

## Using OpenOCD

Use a CMSIS-DAP compatible SWD/JTAG connector. The [Armstart
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

# About the libraries included in this project

Some libraries are included in the form of Git submodules. If you are not
changing them, then you do not need to worry about this, a copy is included in
this repository.

To pull or push to the submodules, start by defining remotes for all the
origins:

    git remote add pedvide-adc git@github.com:pedvide/ADC.git
    git remote add ttlappalainen-nmea2000 git@github.com:ttlappalainen/NMEA2000.git
    git remote add sarfata-nmea2000teensy git@github.com:sarfata/NMEA2000_teensy.git
    git remote add teachop-flexcan git@github.com:teachop/FlexCAN_Library.git
    git remote add paul-ili9341 git@github.com:PaulStoffregen/ILI9341_t3.git

To pull changes:

    git fetch <remote>
    git subtree pull --prefix=lib/<lib> <remote> master --squash

For more information, I recommend [reading
this](https://hpc.uni.lu/blog/2014/understanding-git-subtree/).
