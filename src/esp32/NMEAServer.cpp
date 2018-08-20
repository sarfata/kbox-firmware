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

#include "NMEAServer.h"

#include <cstring>
#include <string>
#include <cstdint>
#include <KBoxLogging.h>
#include <Seasmart.h>

static const int MAX_NMEA2000_MESSAGE_SEASMART_SIZE = 200;

NMEAServer::NMEAServer() :
  _server(10110) {

  _server.onClient([this](void *data, AsyncClient *client) {
    // set up some handlers to remove this when it gets disconnected.
    client->onDisconnect([this](void *, AsyncClient *c) {
      INFO("Client disconnected");
      _clients.remove(c);
    });
    client->onError([this](void *, AsyncClient *c, int8_t error) {
      INFO("Client error %d", error);
      c->close(true);
      _clients.remove(c);
    });
    client->onData([this](void *, AsyncClient *c, void *data, size_t len) {
      char *strData = static_cast<char*>(data);
      strData[len] = 0;
      INFO("Client data %i: '%s'", len, strData);
      _clients.remove(c);
    });

    INFO("Client connected");
    _clients.push_back(client);
  }, nullptr);
}

void NMEAServer::start() {
  _server.begin();
}

bool NMEAServer::write(const tN2kMsg &msg) {
  char buf[MAX_NMEA2000_MESSAGE_SEASMART_SIZE + 2];
  if (N2kToSeasmart(msg, xTaskGetTickCount(), buf, MAX_NMEA2000_MESSAGE_SEASMART_SIZE) == 0) {
    ERROR("n2k conversion error");
    return false;
  }
  else {
    for (auto client : _clients) {
      strncat(buf, "\n", sizeof(buf));

      // A call to write triggers immediate sending of the data. We could also call add() to add the data
      // to the TCP transmit buffer without immediately sending it. Food for thoughts.
      if (client->write(buf, strlen(buf), 0) == 0) {
        ERROR("Not enough space on socket to send data.");
      }
    }
    return true;
  }
}

