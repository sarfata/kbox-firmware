#include <SPI.h>
#include <Encoder.h>
#include <ILI9341_t3.h>
#include <font_Nunito.h>
#include <Adafruit_NeoPixel.h>
#include "hardware/board.h"

#define LED_PIN 13

ILI9341_t3 *tft;

Encoder knob(encoder_a, encoder_b);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);

  tft = new ILI9341_t3(display_cs, display_dc, 255 /* rst unused */, display_mosi, display_sck, display_miso);
  tft->begin();

  tft->fillScreen(ILI9341_BLUE);
  tft->setRotation(3);

  tft->setCursor(10, 42);
  tft->setTextColor(ILI9341_WHITE);
  tft->setTextSize(2);
  tft->setFont(Nunito_18);
  tft->println("Blue Nexus");
  tft->setCursor(10, 60);
  tft->println("Preparing for self tests ...");

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

  tft->fillRect(0, 100, display_width, display_height - 100, ILI9341_BLUE);
  tft->setCursor(display_width / 2, display_height * 2 / 3);
  tft->print(i);

  if (i > 5) {
    tft->setTextColor(ILI9341_GREEN);
    strip.setPixelColor(0, strip.Color(0, 255, 0));
  }
  else if (i > 2) {
    tft->setTextColor(ILI9341_YELLOW);
    strip.setPixelColor(0, strip.Color(100, 100, 0));
  } else {
    tft->setTextColor(ILI9341_RED);
    strip.setPixelColor(0, strip.Color(255, 0, 0));
  }
  strip.show();


  i--;
  if (i < 0) {
    i = 10;
  }


  tft->setCursor(display_width /2, display_height - 20);
  tft->print(knob.read());
  analogWrite(display_pwm, knob.read());
  strip.setPixelColor(1, Wheel(knob.read() & 0xff));

  delay(1000);
}
