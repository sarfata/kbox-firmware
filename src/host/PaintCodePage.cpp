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

#include "PaintCodePage.h"
#include <WString.h>
#include <KBoxDebug.h>

class PaintCodeContext {
  private:
    GC &gc;

  public:
    String fillStyle;
    String font;
    String textAlign;

    PaintCodeContext(GC &gc) : gc(gc) {};

    void fillText(const char *text, float x, float y, float maxWidth = 0) {
      DEBUG("fillText(\"%s\", %f, %f, %f", text, x, y, maxWidth);
      Font f = fontFromString(font);
      Color c = colorFromString(fillStyle);
      // TODO: textalign
      gc.drawText(Point((int)x, (int)y), f, c, text);
    };


  private:

    struct PathPoint { float x; float y; };
    static const int MAX_POINTS = 10;
    PathPoint path[MAX_POINTS];
    int pointsInPath = 0;

    void addPointToPath(float x, float y) {
      if (pointsInPath + 1 < MAX_POINTS) {
        path[pointsInPath].x = x;
        path[pointsInPath].y = y;
        pointsInPath++;
      }
    };

  public:
    String strokeStyle;
    int lineWidth;

    void beginPath() {
      DEBUG("beginPath()");
      pointsInPath = 0;
    };

    void moveTo(float x, float y) {
      DEBUG("moveTo(%f, %f)", x, y);
      addPointToPath(x, y);
    };

    void lineTo(float x, float y) {
      DEBUG("lineTo(%f, %f)", x, y);
      addPointToPath(x, y);
    };

    void stroke() {
      if (pointsInPath < 2) {
        return;
      }

      for (int i = 1; i < pointsInPath; i++) {
        Point a = Point(path[i - 1].x, path[i - 1].y);
        Point b = Point(path[i].x, path[i].y);
        gc.drawLine(a, b, colorFromString(strokeStyle));
      }
    };

  private:
    Font fontFromString(String fontString) {
      return FontDefault;
    };

    Color colorFromString(String colorString) {
      return ColorWhite;
    };
};

typedef struct {
  int x;
  int y;
  int w;
  int h;
} PCRect;

#define makeRect(a, b, c, d) (PCRect) { .x = a, .y = b, .w = c, .h = d }

void PaintCodePage::paint(GC &originalContext) {
  if (!needsPainting) {
    return;
  }
  needsPainting = false;

  PaintCodeContext context = PaintCodeContext(originalContext);

  /// Color Declarations
  auto color = "rgba(255, 255, 255, 1)";

  //// Text Drawing
  auto textRect = makeRect(15, 22, 196, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "left";
  auto textTotalHeight = 12 * 1.3;
  context.fillText("House", textRect.x, textRect.y + 11 + textRect.h / 2 - textTotalHeight / 2);


  //// Text 2 Drawing
  auto text2Rect = makeRect(223, 46, 53, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "right";
  auto text2TotalHeight = 12 * 1.3;
  context.fillText("12.3 V", text2Rect.x + text2Rect.w, text2Rect.y + 11 + text2Rect.h / 2 - text2TotalHeight / 2);


  //// Bezier 2 Drawing
  context.beginPath();
  context.strokeStyle = "rgb(0, 0, 0)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 4 Drawing
  context.beginPath();
  context.moveTo(154.5, 66.5);
  context.lineTo(154.5, 49.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();

  //// Bezier Drawing
  context.beginPath();
  context.moveTo(15.5, 47.5);
  context.lineTo(15.5, 64.5);
  context.lineTo(220.5, 64.5);
  context.lineTo(220.5, 47.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();


  //// Text 4 Drawing
  auto text4Rect = makeRect(223, 77, 53, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "right";
  auto text4TotalHeight = 12 * 1.3;
  context.fillText("4.5 A", text4Rect.x + text4Rect.w, text4Rect.y + 11 + text4Rect.h / 2 - text4TotalHeight / 2);


  //// Bezier 3 Drawing
  context.beginPath();
  context.strokeStyle = "rgb(0, 0, 0)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 5 Drawing
  context.beginPath();
  context.moveTo(119.5, 97.5);
  context.lineTo(119.5, 80.5);
  context.strokeStyle = "rgb(255, 255, 255)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 6 Drawing
  context.beginPath();
  context.moveTo(15.5, 80.5);
  context.lineTo(15.5, 97.5);
  context.lineTo(220.5, 97.5);
  context.lineTo(220.5, 80.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();


  //// Text 5 Drawing
  auto text5Rect = makeRect(16, 115, 196, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "left";
  auto text5TotalHeight = 12 * 1.3;
  context.fillText("Engine", text5Rect.x, text5Rect.y + 11 + text5Rect.h / 2 - text5TotalHeight / 2);


  //// Text 6 Drawing
  auto text6Rect = makeRect(224, 135, 53, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "right";
  auto text6TotalHeight = 12 * 1.3;
  context.fillText("12.3 V", text6Rect.x + text6Rect.w, text6Rect.y + 11 + text6Rect.h / 2 - text6TotalHeight / 2);


  //// Bezier 7 Drawing
  context.beginPath();
  context.strokeStyle = "rgb(0, 0, 0)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 8 Drawing
  context.beginPath();
  context.moveTo(154.5, 155.5);
  context.lineTo(154.5, 138.5);
  context.strokeStyle = "rgb(255, 255, 255)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 9 Drawing
  context.beginPath();
  context.moveTo(16.5, 136.5);
  context.lineTo(16.5, 153.5);
  context.lineTo(221.5, 153.5);
  context.lineTo(221.5, 136.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();


  //// Text 7 Drawing
  auto text7Rect = makeRect(17, 156, 196, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "left";
  auto text7TotalHeight = 12 * 1.3;
  context.fillText("Solar", text7Rect.x, text7Rect.y + 11 + text7Rect.h / 2 - text7TotalHeight / 2);


  //// Text 8 Drawing
  auto text8Rect = makeRect(225, 176, 53, 21);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "12px Menlo-Regular, Menlo, monospace";
  context.textAlign = "right";
  auto text8TotalHeight = 12 * 1.3;
  context.fillText("12.3 V", text8Rect.x + text8Rect.w, text8Rect.y + 11 + text8Rect.h / 2 - text8TotalHeight / 2);


  //// Bezier 10 Drawing
  context.beginPath();
  context.strokeStyle = "rgb(0, 0, 0)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 11 Drawing
  context.beginPath();
  context.moveTo(155.5, 196.5);
  context.lineTo(155.5, 179.5);
  context.strokeStyle = "rgb(255, 255, 255)";
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 12 Drawing
  context.beginPath();
  context.moveTo(17.5, 177.5);
  context.lineTo(17.5, 194.5);
  context.lineTo(222.5, 194.5);
  context.lineTo(222.5, 177.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();


  //// Bezier 13 Drawing
  context.beginPath();
  context.moveTo(161.5, 100.5);
  context.lineTo(161.5, 83.5);
  context.strokeStyle = color;
  context.lineWidth = 1;
  context.stroke();


  //// Text 3 Drawing
  auto text3Rect = makeRect(197, 39, 24, 10);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "7px Menlo-Regular, Menlo, monospace";
  context.textAlign = "right";
  auto text3TotalHeight = 7 * 1.3;
  context.fillText("16", text3Rect.x + text3Rect.w, text3Rect.y + 6 + text3Rect.h / 2 - text3TotalHeight / 2);


  //// Text 9 Drawing
  auto text9Rect = makeRect(15, 39, 24, 10);
  context.fillStyle = "rgb(255, 255, 255)";
  context.font = "7px Menlo-Regular, Menlo, monospace";
  context.textAlign = "left";
  auto text9TotalHeight = 7 * 1.3;
  context.fillText("9", text9Rect.x, text9Rect.y + 6 + text9Rect.h / 2 - text9TotalHeight / 2);
}
