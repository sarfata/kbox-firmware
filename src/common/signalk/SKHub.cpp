/*
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

#include <KBoxLogging.h>
#include "SKHub.h"
#include "SKPredicate.h"
#include "SKSubscriber.h"


class SKSubscription {
  public:
    const SKPredicate& predicate;
    SKSubscriber& subscriber;

    SKSubscription(const SKPredicate& p, SKSubscriber& s) : predicate(p), subscriber(s) {};
};

SKHub::SKHub() {
}

SKHub::~SKHub() {
  for (int i = 0; i < _countSubscriptions; i++) {
    delete _subscriptions[i];
  }
}

void SKHub::subscribe(const SKPredicate& predicate, SKSubscriber& subscriber) {
  if (_countSubscriptions >= maxSubscriptions) {
    ERROR("New subscription requested but maxSubscriptions reached. Ignoring.");
    return;
  }

  _subscriptions[_countSubscriptions] = new SKSubscription(predicate, subscriber);
  _countSubscriptions++;
}

void SKHub::publish(const SKUpdate& update) {
  for (int i = 0; i < _countSubscriptions; i++) {
    if (_subscriptions[i]->predicate.evaluate(update)) {
      _subscriptions[i]->subscriber.updateReceived(update);
    }
  }
}
