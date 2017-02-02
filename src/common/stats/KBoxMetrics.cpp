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

#include "KBoxMetrics.h"

// Instantiate singleton
KBoxMetricsClass KBoxMetrics;

KBoxMetricsClass::KBoxMetricsClass() {
  reset();
}

void KBoxMetricsClass::reset() {
  for (int i = 0; i < KBoxEventCountDistinctEvents; i++) {
    eventOccurences[i] = 0;
  }

  for (int i = 0; i < KBoxMetricCountDistinctMetrics; i++) {
    metricLastValues[i] = 0;
    metricCounts[i] = 0;
    metricSums[i] = 0;
    metricMinimums[i] = 0;
    metricMaximums[i] = 0;
  }
}

void KBoxMetricsClass::event(enum KBoxEvent e) {
  eventOccurences[e]++;
}

uint32_t KBoxMetricsClass::countEvent(const enum KBoxEvent e) const {
  return eventOccurences[e];
}

void KBoxMetricsClass::metric(enum KBoxMetric m, double value) {
  metricLastValues[m] = value;
  metricSums[m] += value;
  metricCounts[m]++;

  if (value < metricMinimums[m]) {
    metricMinimums[m] = value;
  }
  if (value > metricMaximums[m]) {
    metricMaximums[m] = value;
  }
}

double KBoxMetricsClass::averageMetric(const KBoxMetric m) const {
  return metricSums[m] / metricCounts[m];
}
