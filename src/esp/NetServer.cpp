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

NetServer::NetServer(int port) : server(port) {
  server.onClient([this](void *s, AsyncClient* c) {
      handleNewClient(c);
    }, 0);

  server.begin();
}

void NetServer::handleDisconnect(int clientIndex) {
  clients[clientIndex]->free();
  delete(clients[clientIndex]);
  clients[clientIndex] = 0;
  queues[clientIndex].clear();
}

void NetServer::handleNewClient(AsyncClient *client) {
  DEBUG("New connection");
  int i;
  for (i = 0; i < maxClients; i++) {
    if (!clients[i] || !clients[i]->connected()) {
      clients[i] = client;

      client->onAck([this, i](void *s, AsyncClient *c, size_t len, uint32_t time) {
          //DEBUG("Got ack for client %i len=%u time=%u", i, len, time);
        }, 0);
      client->onData([this, i](void *s, AsyncClient *c, void *data, size_t len) {
          DEBUG("Got data from client %i len=%i", i, len);
        }, 0);
      client->onDisconnect([this, i](void *s, AsyncClient *c) {
          DEBUG("Disconnect for client %i", i);
          handleDisconnect(i);
        }, 0);
      client->onError([this, i](void *s, AsyncClient *c, int8_t error) {
          DEBUG("Error %s (%i) on client %i", c->errorToString(error), error, i);
        }, 0);
      client->onTimeout([this, i](void *s, AsyncClient *c, uint32_t time) {
          DEBUG("timeout on client %i at %i", i, time);
          // FIXME: We could be a bit more tolerant here and not disconnect immediately.
          c->close();
        }, 0);
      return;
    }
  }
  DEBUG("Rejecting client - Too many connections already.");
  // We cannot accept this connection at the moment
  client->onDisconnect([](void *s, AsyncClient *c) {
      delete(c);
    });
  client->stop();
}

void NetServer::writeAll(uint8_t *bytes, int len) {
  for (int i = 0; i < maxClients; i++) {
    if (clients[i] && clients[i]->connected()) {
      queues[i].add(NetMessage(bytes, len));
    }
  }
}

int NetServer::clientsCount() {
  int count = 0;
  for (int i = 0; i < maxClients; i++) {
    //DEBUG("counting - i=%i count=%i clients[i]=%p clients[i]->connected()=%s", 
        //i, count, clients[i], clients[i] ? (clients[i]->connected() ? "connected" : "not connected") : "n/a");
    if (clients[i] && clients[i]->connected()) {
      count++;
    }
  }
  return count;
}

void NetServer::loop() {
  for (int i = 0; i < maxClients; i++) {
    if (clients[i] && clients[i]->connected()) {
      if (clients[i]->canSend() && queues[i].size() > 0) {
        DEBUG("Sending for clients[%i] queue len=%i", i, queues[i].size());

        LinkedList<NetMessage>::iterator it = queues[i].begin();
        if (clients[i]->write((const char*)it->bytes(), it->len()) > 0) {
          queues[i].removeFirst();
        }
      }
    }
    else {
      queues[i].clear();
    }
  }
  /*
    if (!clients[i]->canSend()) {
      DEBUG("client[%i]: BUSY in state %s", i, clients[i]->stateToString());
      continue;
    }
    size_t sent = clients[i]->write((char*)bytes, len);
    if (sent != len) {
      DEBUG("client[%i]: sent %i of %i bytes", i, sent, len);
    }
    */
}


