#include "Arduino.h"
#include "pin.h"

/* TFT Screen Definition */
static const uint32_t display_width = 320;
static const uint32_t display_height = 240;
static const pin_t display_miso = 12;
static const pin_t display_mosi = 7;
static const pin_t display_sck = 14;
static const pin_t display_cs = 21;
static const pin_t display_dc = 20;
static const pin_t display_backlight = 0;
