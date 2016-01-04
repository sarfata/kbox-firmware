#include <MFD.h>

class EncoderTestPage: public Page {
  private:
    int clicked = 0;
    int counter = 0;
    unsigned long int timer = 0;
    bool needsPainting = true;

  public:
    bool processEvent(const ButtonEvent &e);
    bool processEvent(const EncoderEvent &e);
    bool processEvent(const TickEvent &e);
    void paint(GC &gc);
};
