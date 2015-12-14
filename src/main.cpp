#include <SPI.h>
#include <Encoder.h>
#include <ILI9341_t3.h>
#include <font_Nunito.h>
#include "hardware/board.h"

#define LED_PIN 13

ILI9341_t3 *tft;
Encoder knob(1, 2);

void setup() {
  pinMode(LED_PIN, OUTPUT);
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
}

void loop() {

  static int i = 10;

  tft->fillRect(0, 100, display_width, display_height - 100, ILI9341_BLUE);
  tft->setCursor(display_width / 2, display_height * 2 / 3);
  tft->print(i);

  if (i > 5) {
    tft->setTextColor(ILI9341_GREEN);
  }
  else if (i > 2) {
    tft->setTextColor(ILI9341_YELLOW);
  } else {
    tft->setTextColor(ILI9341_RED);
  }


  i--;
  if (i < 0) {
    i = 10;
  }


  tft->setCursor(display_width /2, display_height - 20);
  tft->print(knob.read());

  delay(1000);
}
