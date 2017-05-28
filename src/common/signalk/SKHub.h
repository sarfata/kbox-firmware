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

#pragma once

class SKUpdate;
class SKPredicate;
class SKSubscriber;

// Defined in SKHub.h
class SKSubscription;

/** An instance of SKHub is used to concentrate SignalK updates and distributes
 * them to different subscribers.
 *
 * An instance of SKSubscriber can subscribe to updates with a filter defining
 * which updates to get.
 */
class SKHub {
  public:
    SKHub();
    ~SKHub();

    /**
     * Adds a new subscriber which will be notified when updates matching the
     * predicate are received by the hub.
     */
    void subscribe(const SKPredicate& predicate, SKSubscriber& subscriber);

    /**
     * Publish a new update on the hub. All the subscribers with matching
     * subscribptions will be notified. They should process the update as fast
     * as possible and return control so that calling publish should never take
     * "a long time".
     */
    void publish(const SKUpdate&);

  private:
    static const int maxSubscriptions = 20;
    int _countSubscriptions = 0;
    SKSubscription* _subscriptions[maxSubscriptions];
};
