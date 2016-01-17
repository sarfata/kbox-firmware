#include <ADC.h>
#include <MFD.h>

class ADCPage : public Page {
  private:
    bool needsPainting, dirty;
    int status = -1;

    void fetchValues();

    ADC adc;
    float bat1, bat2, bat3, input;

  public:
    ADCPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    bool processEvent(const TickEvent &e);
    void paint(GC &gc);
};

