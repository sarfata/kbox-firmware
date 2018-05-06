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

#include <stdint.h>

enum KBoxEvent {
  KBoxEventNMEA1RX,
  // Happens when the serial buffer is overflowed
  KBoxEventNMEA1RXBufferOverflow,
  // Happens when sentences are received too fast and exceed the max queue length
  // FIXME: Not implemented yet.
  KBoxEventNMEA1RXOverflow,
  KBoxEventNMEA1RXError,
  KBoxEventNMEA1TX,
  KBoxEventNMEA1TXOverflow,

  KBoxEventNMEA2RX,
  KBoxEventNMEA2RXBufferOverflow,
  KBoxEventNMEA2RXOverflow,
  KBoxEventNMEA2RXError,
  KBoxEventNMEA2TX,
  KBoxEventNMEA2TXOverflow,

  KBoxEventNMEA2000MessageReceived,
  KBoxEventNMEA2000MessageSent,
  KBoxEventNMEA2000MessageSendError,

  KBoxEventUSBValidKommand,
  KBoxEventUSBInvalidKommand,

  KBoxEventWiFiRxValidKommand,
  KBoxEventWiFiRxInvalidKommand,
  KBoxEventWiFiTxFrame,
  KBoxEventWiFiRxErrorFrame,


  // Events used by the ESP module
  KBoxEventESPValidKommand,
  KBoxEventESPInvalidKommand,

  // Used to get a count of the number of events
  KBoxEventCountDistinctEvents
};

/**
 * List of all the metrics track in the system.
 *
 * IMPORTANT: All metrics need an explicit unit in their name!
 */
enum KBoxMetric {
  // Average time in us it takes to run through all the system tasks.
  KBoxMetricTaskManagerLoopUS,

  // Used to get a count of the number of metrics
  KBoxMetricCountDistinctMetrics
};

class KBoxMetricsClass {
  public:

  private:
    uint64_t eventOccurences[KBoxEventCountDistinctEvents];
    double metricLastValues[KBoxMetricCountDistinctMetrics];
    double metricSums[KBoxMetricCountDistinctMetrics];
    double metricCounts[KBoxMetricCountDistinctMetrics];
    double metricMinimums[KBoxMetricCountDistinctMetrics];
    double metricMaximums[KBoxMetricCountDistinctMetrics];

  public:
    KBoxMetricsClass();

    /** Reset all event counts to 0 and all metrics to unknown values.
     */
    void reset();

    /**
     * Record one event happening.
     */
    void event(enum KBoxEvent e);

    /**
     * Get the number of time an event happened.
     */
    uint32_t countEvent(const enum KBoxEvent e) const;

    /**
     * Record value of one metric (double).
     */
    void metric(enum KBoxMetric m, double value);

    /**
     * Get the average of a metric.
     */
    double averageMetric(const KBoxMetric m) const;
};

/**
 * Singleton instance of KBoxMetricsClass.
 */
extern KBoxMetricsClass KBoxMetrics;

