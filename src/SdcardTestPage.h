#include <MFD.h>

class SdcardTestPage : public Page {
  private:
    bool needsPainting;
    int status;

  public:
    SdcardTestPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    bool paint(GC &gc);
};

