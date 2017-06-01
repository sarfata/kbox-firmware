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

define raminfo
  # Teensy RAM: 0 -> Static data -> Heap -> (unallocated memory) -> Stack -> end of memory
  printf "Tables  %p -> %p   % 5i\n", 0x1fff8000, &_sdata, (int)&_sdata - 0x1fff8000
  printf "Data    %p -> %p   % 5i\n", &_sdata, &_edata, (int)&_edata-(int)&_sdata
  printf "BSS     %p -> %p   % 5i\n", &_sbss, &_ebss, (int)&_ebss-(int)&_sbss
  printf "Heap    %p -> %p   % 5i\n", &_ebss, __brkval, (int)__brkval - (int)&_ebss
  printf "Free    %p -> %p   % 5i\n", __brkval, (int)$sp, (int)$sp - (int)__brkval
  printf "Stack   %p -> %p   % 5i\n", $sp, &_estack, (int)&_estack - (int)$sp
end

