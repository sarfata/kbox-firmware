/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  Copyright (c) 2018 Ronnie Zeiller ronnie@zeiller.eu
  Copyright (c) 2018 Thomas Sarlandie thomas@sarlandie.net

  The MIT License
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

#include <KBoxLogging.h>
//#include <KBoxHardware.h>
#include "host/config/KBoxConfig.h"
#include "common/signalk/SKUnits.h"
#include "Polar.h"

Polar::Polar(PerformanceConfig &config) : _config(config) {
/*
  if (KBox.isSdCardUsable()) {
    _cardReady = true;
  }
*/
}

bool Polar::readPolarDataFile(){
  /*
  if (!_cardReady) {
    return false;
  }
  */
  //static const char *polarDataFileName = "polarData.pol";
  String polarDataFileName = _config.polarDataFileName;

  DEBUG("Try to load polar datas from file %s on SD-Card", polarDataFileName.c_str());
  /*
  if (KBox.getSdFat().exists(polarDataFileName.c_str())) {
    //File polarDataFile = KBox.getSdFat().open(polarDataFileName);
    DEBUG("Polar data file found");
  } else {
    return false;
  }
  */

  
  return false;
}
