#include <MFD.h>

class ClockPage : public Page {
  private:
    bool needsPainting;

  public:
    ClockPage();

    void willAppear();
    bool processEvent(const TickEvent &e);
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

