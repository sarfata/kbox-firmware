#include <MFD.h>

class NMEAPage : public Page {
  private:
    bool needsPainting;
    int status = -1;

  public:
    NMEAPage();

    void willAppear();
    bool processEvent(const TickEvent &e);
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

