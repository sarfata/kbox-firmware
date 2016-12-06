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

#include <KBox.h>
#include <WString.h>
#include <KBoxDebug.h>

typedef struct {
  int x;
  int y;
  int w;
  int h;
} PCRect;

#define makeRect(a, b, c, d) (PCRect) { .x = a, .y = b, .w = c, .h = d }

class PaintCodeContext {
  private:
    GC &gc;

  public:
    String fillStyle;
    Font font;
    String textAlign;

    PaintCodeContext(GC &gc) : gc(gc) {};

    void fillText(const String &s, float x, float y) {
        fillText(s.c_str(), x, y);
    };

    void fillText(const char *text, float x, float y, float maxWidth = 0) {
      DEBUG("fillText(\"%s\", %f, %f, %f", text, x, y, maxWidth);
      Font f = fontFromString(font);
      Color c = colorFromString(fillStyle);
      // TODO: textalign
      gc.drawText(Point(x + translateX, y + translateY), f, c, text);
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

    struct PathRect { float x; float y; float w; float h; };
    bool isRectInPath = false;
    struct PathRect rectInPath;

    int translateX = 0;
    int translateY = 0;

  public:
    String strokeStyle;
    int lineWidth;

    void beginPath() {
      DEBUG("beginPath()");
      pointsInPath = 0;
      isRectInPath = false;
    };

    void moveTo(float x, float y) {
      DEBUG("moveTo(%f, %f)", x, y);
      addPointToPath(x + translateX, y + translateY);
    };

    void lineTo(float x, float y) {
      DEBUG("lineTo(%f, %f)", x, y);
      addPointToPath(x + translateX, y + translateY);
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

    void fill() {
      if (isRectInPath) {
        gc.drawRectangle(Point(rectInPath.x, rectInPath.y), Size(rectInPath.w, rectInPath.h), colorFromString(strokeStyle));
      }
    }


    void restore() {
        translateX = translateY = 0;
    };
    void save() {};
    void clip() {};
    void translate(float x, float y) {
        translateX = x;
        translateY = y;
    };
    void rect(float x, float y, float w, float h) {
        isRectInPath = true;
        rectInPath = { x + translateX, y + translateY, w, h };
    };

  private:
    Font fontFromString(String fontString) {
      return FontDefault;
    };

    Color colorFromString(String colorString) {
      return ColorWhite;
    };
};

