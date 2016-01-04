#include <SPI.h>
#include <Encoder.h>
#include <Bounce.h>
#include <Adafruit_NeoPixel.h>
#include <MFD.h>
#include <ili9341display.h>
#include <Debug.h>
#include "EncoderTestPage.h"
#include "hardware/board.h"

#define LED_PIN 13

Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, neopixel_pin, NEO_GRB + NEO_KHZ800);

MFD *mfd;

void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("Starting...");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 1);

  ILI9341Display *display = new ILI9341Display(display_mosi, display_miso, display_sck, display_cs, display_dc, display_backlight, Size(display_width, display_height));
  Encoder *encoder = new Encoder(encoder_a, encoder_b);
  pinMode(encoder_button, INPUT_PULLUP);
  Bounce *button = new Bounce(encoder_button, 10 /* ms */);

  mfd = new MFD(*display, *encoder, *button);

  EncoderTestPage *encPage = new EncoderTestPage();
  mfd->addPage(encPage);

  strip.begin();
  strip.show();
}

void loop() {
  mfd->loop();
  delay(1);
}
