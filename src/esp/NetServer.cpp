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

#include <KBoxLogging.h>
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
        //DEBUG("Sending for clients[%i] queue len=%i", i, queues[i].size());

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


