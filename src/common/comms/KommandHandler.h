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

#include "SlipStream.h"
#include "KommandReader.h"
#include "Kommand.h"

class KommandHandler {
  public:
    KommandHandler() {};

    /**
     * KommandHandler will be called with a Kommand.
     *
     * It should return true if it successfully processed the command
     * or false to pass the Kommand to the next handler.
     */
    virtual bool handleKommand(KommandReader &kreader, SlipStream &replyStream) = 0;

    /**
     * Static helper function to go through a list of KommandHandler and stop when the Kommand
     * has been processed.
     *
     * @param handlers a null-terminated list of KommandHandler
     * @param kreader a KommandReader initialized with the Kommand to process
     * @param replyStream a SlipStream to use to reply to that message
     * @param sendError indicates whether function should send an error on replyStream if no
     *    appropriate handler is found.
     * @return true if the kommand was processed by an handler, false otherwise
     */
    static bool handleKommandWithHandlers(KommandHandler *handlers[], KommandReader &kreader, SlipStream &replyStream, bool sendError = true);
};

