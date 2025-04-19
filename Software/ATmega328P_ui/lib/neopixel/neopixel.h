#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "Adafruit_NeoPixel.h"
#include "Arduino.h"
#include "simplify_deg.h"

#define LED_QTY 16
class NeoPixel {
     public:
      NeoPixel();
      Adafruit_NeoPixel led;
      void SetPixelColor(int8_t led_num, uint8_t red, uint8_t green, uint8_t blue);
      void SetDegree(int16_t degree, uint8_t red, uint8_t green, uint8_t blue);
      void Clear();
      void Show();
      void Rainbow();
      void SetBrightness(uint8_t brightness);

     private:
      uint8_t brightness_;
};

#endif