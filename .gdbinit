target remote localhost:3333
monitor reset halt
set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 4

define hook-quit
  set confirm off
end
