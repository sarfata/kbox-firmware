#pragma once

#include <MFD.h>

class WifiTestPage : public Page {
  private:
    bool needsPainting;
    bool needsFullPainting;
    static const int bufferSize = 1024;
    int bufferIndex = 0;
    char buffer[bufferSize];

  public:
    WifiTestPage();

    bool processEvent(const ButtonEvent &e);
    bool processEvent(const TickEvent &e);
    void willAppear();
    void paint(GC &gc);
};
