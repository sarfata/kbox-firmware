#include <MFD.h>
#include <Adafruit_BMP280.h>

class BarometerPage : public Page {
  private:
    bool needsPainting;
    int status = -1;

    void fetchValues();

    Adafruit_BMP280 bmp280;
    float temperature;
    float pressure;

  public:
    BarometerPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

