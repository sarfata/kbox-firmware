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

#include "Debug.h"
#include "NetServer.h"

NetServer::NetServer(int port) {
  server = new WiFiServer(port);
  server->begin();
  server->setNoDelay(true);
}

void NetServer::handleNewClients() {
  if (server->hasClient()) {
    DEBUG("Handling new client");
    int i;
    for (i = 0; i < maxClients; i++) {
      if (!clients[i] || !clients[i].connected()) {
        DEBUG("Assigning slot %i", i);
        if (clients[i]) {
          clients[i].stop();
        }
        clients[i] = server->available();
        break;
      }
    }
    if (i >= maxClients) {
      DEBUG("Rejecting client - Too many connections already.");
      // We cannot accept this connection at the moment
      WiFiClient c = server->available();
      c.stop();
    }
  }
}

void NetServer::writeAll(uint8_t *bytes, int len) {
  for (int i = 0; i < maxClients; i++) {
    if (clients[i] && clients[i].connected()) {
      clients[i].write((char*)bytes, len);
    }
  }
}

int NetServer::clientsCount() {
  int count = 0;
  for (int i = 0; i < maxClients; i++) {
    if (clients[i] && clients[i].connected()) {
      count++;
    }
  }
  return count;
}

void NetServer::loop() {
  handleNewClients();
}


