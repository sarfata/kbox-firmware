target remote localhost:3333
monitor reset halt
set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4

file .pioenvs/host/firmware.elf

set history save on
set history size 1000
set history filename .gdb_history

define hook-quit
  set confirm off
end
