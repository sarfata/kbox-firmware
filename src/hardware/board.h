#include <Arduino.h>
#include <pin.h>

/* TFT Screen Definition */
static const uint32_t display_width = 320;
static const uint32_t display_height = 240;
static const pin_t display_miso = 12;
static const pin_t display_mosi = 11;
static const pin_t display_sck = 14;
static const pin_t display_cs = 20;
static const pin_t display_dc = 21;
static const pin_t display_backlight= 32;

static const pin_t encoder_a = 19;
static const pin_t encoder_b = 18;
static const pin_t encoder_button = 17;

static const pin_t neopixel_pin = 22;

static const pin_t wifi_enable = 2;
static const pin_t wifi_program = 5;
static const pin_t wifi_rst = 23;
// This one was hijacked for display_dc - static const pin_t wifi_cs = 21;

static const uint8_t ina219_address = 0b1000000;

static const uint8_t bmp280_address = 0x76;
