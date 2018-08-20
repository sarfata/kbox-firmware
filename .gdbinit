file .pioenvs/esp32/firmware.elf

target remote :3333
mon reset halt
flushregs
thb app_main
c

set history save on
set history size 1000
set history filename .gdb_history

define hook-quit
  set confirm off
end

