#include "SdcardTestPage.h"

SdcardTestPage::SdcardTestPage() {
  status = -1;
}

void SdcardTestPage::willAppear() {
  readCard();
}

void SdcardTestPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    readCard();
  }
}

void SdcardTestPage::readCard() {

  needsPainting = true;
}

void SdcardTestPage::paint(GC &gc) {
  if (needsPainting) {

    needsPainting = false;
  }
}

