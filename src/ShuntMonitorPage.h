#include <MFD.h>

class ShuntMonitorPage : public Page {
  private:
    static const int shuntVoltageRegisterAddress = 0x01;
    static const int busVoltageRegisterAddress = 0x02;
    int shuntVoltage;
    int busVoltage;

    static const int StatusOk = 0;
    static const int StatusNotReady = -1;
    int status = StatusNotReady;

    bool needsPainting;

    bool readRegister(uint8_t address, uint16_t *value);
    void fetchValues();

  public:
    ShuntMonitorPage();

    void willAppear();
    bool processEvent(const ButtonEvent &e);
    void paint(GC &gc);
};

