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
  DEBUG("Disconnect for client %i", clientIndex);
  // AsyncPrinter will delete the client object in the client->onDisconnect handler.
}

void NetServer::handleNewClient(AsyncClient *client) {
  DEBUG("New connection");
  int i;
  for (i = 0; i < maxClients; i++) {
    if (!clients[i]) {
      clients[i] = AsyncPrinter(client, 2048);

      clients[i].onData([this, i](void *s, AsyncPrinter *c, uint8_t *data, size_t len) {
          DEBUG("Got data from client %i len=%i", i, len);
        }, 0);
      clients[i].onClose([this, i](void *s, AsyncPrinter *c) {
          handleDisconnect(i);
        }, 0);

      // Attach some debug messages directly to the client for information.
      // These handlers are not set by AsyncPrinter.
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
  // We cannot accept this connection at the moment. Set a handler to free the object when disconnected
  // and start disconnection.
  client->onDisconnect([](void *s, AsyncClient *c) {
      delete(c);
    });
  client->stop();
}

void NetServer::writeAll(const uint8_t *bytes, int len) {
  for (int i = 0; i < maxClients; i++) {
    if (clients[i]) {
      clients[i].write(bytes, len);
    }
  }
}

int NetServer::clientsCount() {
  int count = 0;
  for (int i = 0; i < maxClients; i++) {
    if (clients[i]) {
      count++;
    }
  }
  return count;
}

void NetServer::loop() {
  // Writes are performed asynchronously.
}


