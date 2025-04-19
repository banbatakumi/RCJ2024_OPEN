#ifndef _DRIBBLER_H_
#define _DRIBBLER_H_

#include "setup.h"

bool is_ball_catch_front;
bool is_ball_catch_back;

void Dribbler() {
      if (Serial.available() > 0) {
            uint8_t bool_data = Serial.read();
            is_ball_catch_front = (bool_data >> 1) & 1;
            is_ball_catch_back = bool_data & 1;
            while (Serial.available() > 0) Serial.read();
      }

      if (sub_item == 0) {
            oled.PrintCenter("Dribbler", 64, 32);
            dribbler_sig = 0;
      } else if (sub_item == 1) {
            static uint8_t cnt = 0;
            oled.PrintCenter("Front", 64, 32);
            if (set_val == -1) {
                  dribbler_sig = 1;
            } else if (set_val == 1) {
                  dribbler_sig = 2;
            }
            if (dribbler_sig == 2) cnt++;
            if (cnt > 10) {
                  dribbler_sig = 0;
                  cnt = 0;
            }
      } else if (sub_item == 2) {
            oled.PrintCenter("Back", 64, 32);
            if (set_val == -1) {
                  dribbler_sig = 3;
            } else if (set_val == 1) {
                  dribbler_sig = 0;
            }
      } else {
            sub_item = 0;
      }
      if (is_ball_catch_front == 1) led.SetPixelColor(0, 0, 0, 255);
      if (is_ball_catch_back == 1) led.SetPixelColor(8, 0, 0, 255);
}

#endif