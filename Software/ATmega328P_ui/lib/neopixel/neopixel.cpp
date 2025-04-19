#include "neopixel.h"

NeoPixel::NeoPixel() : led(LED_QTY, 2, NEO_GRB + NEO_KHZ800) {
      led.begin();
}

void NeoPixel::SetPixelColor(int8_t led_num, uint8_t red, uint8_t green, uint8_t blue) {
      int8_t led_num_tmp = 20 - led_num;
      while (led_num_tmp > LED_QTY - 1) led_num_tmp -= LED_QTY;
      while (led_num_tmp < 0) led_num_tmp += LED_QTY;
      led.setPixelColor(led_num_tmp, led.Color(red * (brightness_ / 255.0), green * (brightness_ / 255.0), blue * (brightness_ / 255.0)));
}

void NeoPixel::SetDegree(int16_t degree, uint8_t red, uint8_t green, uint8_t blue) {
      const float offsets[] = {0, 22.5, 45, 67.5, 90, 112.5, 135, 157.5, -157.5, -135, -112.5, -90, -67.5, -45, -22.5, 0};

      for (int i = 0; i < 16; i++) {
            int16_t lower_bound = offsets[i] - 45;
            int16_t upper_bound = offsets[i] + 45;

            if ((degree >= lower_bound && degree <= upper_bound) ||
                (lower_bound < -180 && (degree >= lower_bound + 360 || degree <= upper_bound)) ||
                (upper_bound > 180 && (degree >= lower_bound || degree <= upper_bound - 360))) {
                  int16_t adjusted_degree = SimplifyDeg(degree - offsets[i]);
                  float intensity = (45 - abs(adjusted_degree)) / 45.0f;
                  SetPixelColor(i, intensity * red, intensity * green, intensity * blue);
            }
      }
}

void NeoPixel::SetBrightness(uint8_t brightness) {
      this->brightness_ = brightness;
}

void NeoPixel::Clear() {
      led.clear();
}

void NeoPixel::Show() {
      led.show();
}

void NeoPixel::Rainbow() {
      static uint16_t step_num = 0;
      for (int i = 0; i < LED_QTY; i++) {
            // ストリップの長さに沿って色相環（65536の範囲）を1回転させる量だけピクセルの色相をオフセットします。
            int pixelHue = step_num + (i * 65536L / led.numPixels());
            // ColorHSV関数に色相(0 to 65535)を渡し、その結果をgamma32()でガンマ補正します。
            led.setPixelColor(i, led.gamma32(led.ColorHSV(pixelHue)));
      }
      led.show();
      step_num += 256;
      if (step_num == 65536) {
            step_num = 0;
      }
}