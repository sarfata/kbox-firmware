/*
     __  __     ______     ______     __  __
    /\ \/ /    /\  == \   /\  __ \   /\_\_\_\
    \ \  _"-.  \ \  __<   \ \ \/\ \  \/_/\_\/_
     \ \_\ \_\  \ \_____\  \ \_____\   /\_\/\_\
       \/_/\/_/   \/_____/   \/_____/   \/_/\/_/

  The MIT License

  Copyright (c) 2017 Thomas Sarlandie thomas@sarlandie.net

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

#include "KommandHandlerWiFiStatus.h"

#include <IPAddress.h>
#include <KBoxLogging.h>
#include <comms/ESPState.h>


bool KommandHandlerWiFiStatus::handleKommand(KommandReader &kreader,
                                             SlipStream &replyStream) {
  if (kreader.getKommandIdentifier() != KommandWiFiStatus) {
    return false;
  }

  auto state = static_cast<ESPState>(kreader.read16());
  auto dhcpClients = kreader.read16();
  auto tcpClients = kreader.read16();
  auto signalkClients = kreader.read16();
  auto ipAddress = static_cast<IPAddress>(kreader.read32());

  DEBUG("WiFiStatus: state: %i dhcpClients: %i tcpClients: %i signalkClients: "
          "%i ip: %u.%u.%u.%u",
        static_cast<int>(state), dhcpClients, tcpClients, signalkClients,
        ipAddress[0], ipAddress[1], ipAddress[2], ipAddress[3]);

  _observer.wiFiStatusUpdated(state, dhcpClients, tcpClients, signalkClients,
                              ipAddress);
  return true;
}
