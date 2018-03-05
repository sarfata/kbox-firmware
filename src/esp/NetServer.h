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
#pragma once

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include "common/algo/List.h"

/*
 * Used to build a buffer (with a LinkedList<NetMessage>) of 
 * messages that are waiting for each client.
 * Using a LinkedList here is pretty inefficient.
 * FIXME: Use a circular buffer or dynamic buffer (with a max size) instead of
 * this.
 */
class NetMessage {
  private:
    uint8_t* _bytes;
    int _len;

  public:
    NetMessage(const uint8_t *bytes, int len) : _len(len) {
      _bytes = (uint8_t*)malloc(len);
      memcpy(_bytes, bytes, len);
    };

    NetMessage(const NetMessage &m) {
      _bytes = (uint8_t*)malloc(m._len);
      memcpy(_bytes, m._bytes, m._len);
      _len = m._len;
    };

    ~NetMessage() {
      free(_bytes);
    };

    int len() const {
      return _len;
    };

    const uint8_t* bytes() const {
      return _bytes;
    };
};

class NetServer {
  public:
    NetServer(int port);

    void loop();
    void writeAll(const uint8_t *bytes, int len);
    int clientsCount();

  private:
    static const int maxClients = 8;
    AsyncClient *clients[maxClients];
    AsyncServer server;

    LinkedList<NetMessage> queues[maxClients];

    void handleNewClient(AsyncClient *client);
    void handleDisconnect(int clientIndex);
};
