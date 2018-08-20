/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include "NMEA2000Task.h"

#include <cstring>
#include <NMEA2000_esp32.h>


NMEA2000Task::NMEA2000Task(): _rxCounter(0) {
  _rxCounterMutex = xSemaphoreCreateMutex();

}

uint64_t NMEA2000Task::getRXCounter() const {
  uint64_t counterValue = 0;

  xSemaphoreTake(_rxCounterMutex, portMAX_DELAY );
  counterValue = _rxCounter;
  xSemaphoreGive(_rxCounterMutex);

  return counterValue;
}

void NMEA2000Task::resetRXCounter() {
  xSemaphoreTake(_rxCounterMutex, portMAX_DELAY );
  _rxCounter = 0;
  xSemaphoreGive(_rxCounterMutex);
}

void NMEA2000Task::initializeNMEA2000(gpio_num_t txPin, gpio_num_t rxPin) {
  // Configure the ENABLE pin of the CAN controller and pull it low
  //pinMode(GPIO_CAN_DISABLE, OUTPUT);
  //digitalWrite(GPIO_CAN_DISABLE, 0);

  _nmea2000 = new tNMEA2000_esp32(txPin, rxPin);

  // Initialize NMEA2000 library
  _nmea2000->SetN2kCANMsgBufSize(8);
  _nmea2000->SetN2kCANReceiveFrameBufSize(100);
  _nmea2000->SetMode(tNMEA2000::N2km_ListenAndNode, 32);
  _nmea2000->AttachMsgHandler(this);

  _nmea2000->Open();
}

void NMEA2000Task::setup(gpio_num_t txPin, gpio_num_t rxPin) {
  initializeNMEA2000(txPin, rxPin);
}

void NMEA2000Task::loop(){
  _nmea2000->ParseMessages();
}

void NMEA2000Task::HandleMsg(const tN2kMsg &msg) {
  ESP_LOGV(TAG, "Handling message PGN %lu", msg.PGN);

  xSemaphoreTake(_rxCounterMutex, portMAX_DELAY );
  _rxCounter++;
  xSemaphoreGive(_rxCounterMutex);

  for (auto writer : _writers) {
    writer->write(msg);
  }
}

bool NMEA2000Task::write(const tN2kMsg &msg) {
  return _nmea2000->SendMsg(msg);
}
