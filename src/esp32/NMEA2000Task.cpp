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

#define ESP32_CAN_TX_PIN GPIO_NUM_5
#define ESP32_CAN_RX_PIN GPIO_NUM_4
#include <NMEA2000_CAN.h>

#define GPIO_CAN_DISABLE ((gpio_num_t)16)

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

void NMEA2000Task::initializeNMEA2000() {
  // Configure the ENABLE pin of the CAN controller and pull it low
  pinMode(GPIO_CAN_DISABLE, OUTPUT);
  digitalWrite(GPIO_CAN_DISABLE, 0);

  // Initialize NMEA2000 library
  NMEA2000.SetN2kCANMsgBufSize(8);
  NMEA2000.SetN2kCANReceiveFrameBufSize(100);
  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 32);
  NMEA2000.AttachMsgHandler(this);

  NMEA2000.Open();
}

void NMEA2000Task::setup() {
  initializeNMEA2000();
}

void NMEA2000Task::loop(){
  NMEA2000.ParseMessages();
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
