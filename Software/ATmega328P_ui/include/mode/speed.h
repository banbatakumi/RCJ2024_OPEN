#ifndef _SPEED_H_
#define _SPEED_H_

#include "setup.h"

#define MAX_SPEED 150  // cm/s

void Speed() {
      if (sub_item == 0) {
            oled.PrintCenter("Speed", 64, 32);
      } else if (sub_item == 1) {
            oled.PrintCenter("Normal", 64, 16);
            oled.PrintCenter(String(moving_speed * 0.01).c_str(), 64, 32);
            oled.PrintCenter("m/s", 64, 48);

            moving_speed += set_val * -5;
            if (moving_speed > MAX_SPEED) moving_speed = MAX_SPEED;

            for (uint8_t i = 0; i < round(moving_speed / (MAX_SPEED / 16.00)); i++) led.SetPixelColor(i, 0, 0, 255);
      } else if (sub_item == 2) {
            oled.PrintCenter("Line", 64, 16);
            oled.PrintCenter(String(line_moving_speed * 0.01).c_str(), 64, 32);
            oled.PrintCenter("m/s", 64, 48);

            line_moving_speed += set_val * -5;
            if (line_moving_speed > MAX_SPEED) line_moving_speed = MAX_SPEED;

            for (uint8_t i = 0; i < round(line_moving_speed / (MAX_SPEED / 16.00)); i++) led.SetPixelColor(i, 0, 0, 255);
      } else {
            sub_item = 0;
      }
}

#endif