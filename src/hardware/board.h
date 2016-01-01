#include "Arduino.h"

// FIXME: Should be provided by another module.
typedef uint32_t pin_t;

/* TFT Screen Definition */
static const uint32_t display_width = 320;
static const uint32_t display_height = 240;
static const pin_t display_miso = 12;
static const pin_t display_mosi = 11;
static const pin_t display_sck = 14;
static const pin_t display_cs = 20;
static const pin_t display_dc = 21;
static const pin_t display_pwm = 32;

static const pin_t encoder_a = 19;
static const pin_t encoder_b = 18;

static const pin_t neopixel_pin = 22;
