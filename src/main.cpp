#include <SPI.h>
#include <Encoder.h>
#include <Adafruit_NeoPixel.h>
#include "hardware/board.h"
#include "mfd/mfd.h"
#include "mfd/ili9341display.h"

#define LED_PIN 13

Encoder knob(encoder_a, encoder_b);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);

MFD *mfd;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);

  ILI9341Display *display = new ILI9341Display(display_mosi, display_miso, display_sck, display_cs, display_dc, display_backlight, Size(display_width, display_height));
  mfd = new MFD(*display);

  strip.begin();
  strip.show();
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void loop() {
  static int i = 10;

  if (i > 5) {
    strip.setPixelColor(0, strip.Color(0, 255, 0));
  }
  else if (i > 2) {
    strip.setPixelColor(0, strip.Color(100, 100, 0));
  } else {
    strip.setPixelColor(0, strip.Color(255, 0, 0));
  }
  strip.show();


  i--;
  if (i < 0) {
    i = 10;
  }

  //analogWrite(display_pwm, knob.read());
  //strip.setPixelColor(1, Wheel(knob.read() & 0xff));

  delay(1000);
}
