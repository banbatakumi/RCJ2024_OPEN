#ifndef OLED_H
#define OLED_H

#include "Arduino.h"
#include "U8g2lib.h"

#define CenterX(x_, charnum_) (x_ - ((charnum_ * 9) / 2))
#define CenterY(y_) (y_ + 6)

class Oled {
     public:
      Oled();
      U8G2_SSD1306_128X64_NONAME_1_HW_I2C oled;

      void Init();

      void FirstPage() {
            oled.firstPage();
      }

      uint8_t NextPage() {
            return oled.nextPage();
      }

      void SetCursor(int x, int y) {
            oled.setCursor(x, y);
      }

      void Print(const char* str) {
            oled.print(str);
      }

      void PrintCenter(const char* str, int x, int y) {
            oled.setCursor(CenterX(x, String(str).length()), CenterY(y));
            oled.print(str);
      }

      void PrintCenterY(const char* str, int x, int y) {
            oled.setCursor(x, CenterY(y));
            oled.print(str);
      }

      void DrawFrame(int x, int y, int w, int h) {
            oled.drawFrame(x, y, w, h);
      }
      void DrawFilledEllipse(int x, int y, int w, int h) {
            oled.drawFilledEllipse(x, y, w, h);
      }
      void DrawEllipse(int x, int y, int w, int h) {
            oled.drawEllipse(x, y, w, h);
      }
      void DrawCircle(int x, int y, int r) {
            oled.drawCircle(x, y, r);
      }
      void DrawLine(int x1, int y1, int x2, int y2) {
            oled.drawLine(x1, y1, x2, y2);
      }
      void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
            oled.drawTriangle(x1, y1, x2, y2, x3, y3);
      }

     private:
};

#endif