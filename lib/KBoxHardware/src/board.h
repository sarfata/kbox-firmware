/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>
#include <stdint.h>
typedef uint16_t pin_t;

/* "Running Light" led */
static const pin_t led_pin = 13;

/* TFT Screen Definition */
static const uint32_t display_width = 320;
static const uint32_t display_height = 240;
static const pin_t display_miso = 12;
static const pin_t display_mosi = 11;
static const pin_t display_sck = 14;
static const pin_t display_cs = 20;
static const pin_t display_dc = 21;
static const pin_t display_backlight= 32;
#ifdef BOARD_v1_revA
static const uint8_t display_rotation = 3;
#else
static const uint8_t display_rotation = 1;
#endif

/* Encoder */
static const pin_t encoder_a = 19;
static const pin_t encoder_b = 18;
static const pin_t encoder_button = 17;

/* NeoPixels */
static const pin_t neopixel_pin = 22;

/* ESP8266 */
static const pin_t wifi_enable = 2;
static const pin_t wifi_program = 5;
static const pin_t wifi_rst = 23;
#ifdef BOARD_v1_revA
// On v1 revA, the CS line was supposed to be 21 but was hijacked to be display_dc.
// So wifi_cs is *not* available on revA and SPI cannot be used to talk to the ESP module.
static const pin_t wifi_cs = 0;
#else
static const pin_t wifi_cs = 31;
#endif
#define WiFiSerial Serial1

/* CAN Transceiver */
static const pin_t can_standby = 33;

/* INA219 Battery Monitor */
static const uint8_t ina219_address = 0x40;

/* BMP280 Barometer */
static const uint8_t bmp280_address = 0x76;

/* Serial Transceiver */
static const pin_t nmea1_out_enable = 24;
static const pin_t nmea2_out_enable = 16;
#define NMEA1_SERIAL Serial2
#define NMEA2_SERIAL Serial3

/* Analog Inputs */
#ifdef BOARD_v1_revA
static const pin_t adc1_analog = A10;
static const pin_t adc2_analog = A11;
static const pin_t adc3_analog = A12;
static const float analog_max_voltage = 3.3 / (10000/(10000+56000.0));
#else
static const pin_t adc1_analog = A12;
static const pin_t adc2_analog = A11;
static const pin_t adc3_analog = A10;
static const float analog_max_voltage = 3.0 / (10000/(10000+56000.0));
#endif
static const pin_t supply_analog = A14;

/* SDCard Interface */
static const pin_t sdcard_cs = 15;
