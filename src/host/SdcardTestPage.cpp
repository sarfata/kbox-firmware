#include <SdFat.h>
#include "board.h"
#include "SdcardTestPage.h"

SdcardTestPage::SdcardTestPage() {
  status = -1;

  if (!sd.begin(sdcard_cs)) {
    if (sd.card()->errorCode()) {
      DEBUG("Something went wrong ... SD card errorCode: %i errorData: %i", sd.card()->errorCode(), sd.card()->errorData());
      return;
    }
    else {
      DEBUG("SdCard successfully initialized.");
    }

    if (sd.vol()->fatType() == 0) {
      DEBUG("Can't find a valid FAT16/FAT32 partition. Try reformatting the card.");
      return;
    }

    if (!sd.vwd()->isOpen()) {
      DEBUG("Can't open root directory. Try reformatting the card.");
      return;
    }

    DEBUG("Unknown error while initializing card.");
    return;
  }

  uint32_t size = sd.card()->cardSize();
  if (size == 0) {
    DEBUG("Can't figure out card size :(");
    return;
  }
  uint32_t sizeMB = 0.000512 * size + 0.5;
  DEBUG("Card size: %luMB. Volume is FAT%i Cluster size: %i bytes", sizeMB, sd.vol()->fatType(), 512 * sd.vol()->blocksPerCluster());

 if ((sizeMB > 1100 && sd.vol()->blocksPerCluster() < 64)
      || (sizeMB < 2200 && sd.vol()->fatType() == 32)) {
    DEBUG("This card should be reformatted for best performance.");
    DEBUG("Use a cluster size of 32 KB for cards larger than 1 GB.");
    DEBUG("Only cards larger than 2 GB should be formatted FAT32.");
    return;
  }

  DEBUG("Listing files...");
  sd.ls(LS_R | LS_DATE | LS_SIZE);
}

void SdcardTestPage::readCard() {

  needsPainting = true;
}

void SdcardTestPage::willAppear() {
  readCard();
}

bool SdcardTestPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypePressed) {
    readCard();
  }
}

void SdcardTestPage::paint(GC &gc) {
  if (needsPainting) {

    needsPainting = false;
  }
}

