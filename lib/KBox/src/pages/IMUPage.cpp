/*
  The MIT License

  Copyright (c) 2016 Thomas Sarlandie thomas@sarlandie.net

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

#include "IMUPage.h"
#include "tasks/IMUTask.h"

IMUPage::IMUPage() {
  calibrationLayer = new TextLayer(Point(0, 220), Size(320, 20), "", ColorWhite, ColorBlack, FontDefault);

  courseLayer = new TextLayer(Point(20, 100), Size(40, 20), "---", ColorWhite, ColorBlack, FontLarge);
  addLayer(new TextLayer(Point(30, 160), Size(20, 20), "MAG", ColorWhite, ColorBlack, FontDefault));

  addLayer(new TextLayer(Point(200, 80), Size(20, 20), "Pitch", ColorWhite, ColorBlack, FontDefault));
  pitchLayer = new TextLayer(Point(200, 100), Size(40, 20), "---", ColorWhite, ColorBlack, FontDefault);

  addLayer(new TextLayer(Point(200, 180), Size(20, 20), "Heel", ColorWhite, ColorBlack, FontDefault));
  heelLayer = new TextLayer(Point(200, 200), Size(40, 20), "---", ColorWhite, ColorBlack, FontDefault);

  addLayer(calibrationLayer);
  addLayer(courseLayer);
  addLayer(pitchLayer);
  addLayer(heelLayer);
}

void IMUPage::processMessage(const KMessage &message) {
  if (message.getMessageType() == KMessageType::IMUMessage) {
    const IMUMessage *m = static_cast<const IMUMessage*>(&message);

    calibrationLayer->setText("Sys: " + String(m->getSysCalib())
        + " Accel: " + String(m->getAccelCalib())
        + " Gyro: " + String(m->getGyroCalib())
        + " Mag: " + String(m->getMagCalib()));

    courseLayer->setText(String(m->getCourse(), 1) + " º");
    pitchLayer->setText(String(m->getPitch(), 0) + " º");
    heelLayer->setText(String(m->getHeel(), 0) + " º");
  }
}

// TODO: REMOVE ME!
bool IMUPage::processEvent(const EncoderEvent &e) {
  if (e.rotation > 0) {
    imuTask->saveCalibration();
  }
  return true;
}

bool IMUPage::processEvent(const ButtonEvent &e) {
  if (e.clickType == ButtonEventTypeClick) {
    // Change page on single click.
    return false;
  }
  if (e.clickType == ButtonEventTypeLongClick) {
    if (imuTask) {
      imuTask->resetIMU();
    }
  }
  return true;
}
