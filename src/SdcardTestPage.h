#include <MFD.h>
#include <SdFat.h>

class SdcardTestPage : public Page {
  private:
    bool needsPainting;
    int status;

    SdFat sd;

    void readCard();

  public:
    SdcardTestPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

