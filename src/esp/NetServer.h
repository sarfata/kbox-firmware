/*

    This file is part of KBox.

    Copyright (C) 2016 Thomas Sarlandie.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <List.h>
#include <Debug.h>

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
    NetMessage(uint8_t *bytes, int len) : _len(len) {
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
    void writeAll(uint8_t *bytes, int len);
    int clientsCount();

  private:
    static const int maxClients = 8;
    AsyncClient *clients[maxClients];
    AsyncServer server;

    LinkedList<NetMessage> queues[maxClients];

    void handleNewClient(AsyncClient *client);
    void handleDisconnect(int clientIndex);
};
