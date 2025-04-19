#ifndef _LINE_H_
#define _LINE_H_

#include "setup.h"

int16_t line_dir;
int16_t line_inside_dir;
uint8_t line_interval;
bool is_on_line;
bool is_leftside;
bool is_rightside;

void Line() {
      static const uint8_t HEADER = 0xFF;   // ヘッダ
      static const uint8_t FOOTER = 0xAA;   // ヘッダ
      static const uint8_t data_size = 4;   // データのサイズ
      static uint8_t index = 0;             // 受信したデータのインデックスカウンター
      static uint8_t recv_data[data_size];  // 受信したデータ
      static uint8_t recv_byte;
      while (Serial.available()) {
            recv_byte = Serial.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        line_dir = recv_data[0] * 2 - 180;
                        line_inside_dir = recv_data[1] * 2 - 180;
                        line_interval = recv_data[2];
                        success_reset_line = (recv_data[3] >> 3) & 1;
                        is_on_line = (recv_data[3] >> 2) & 1;
                        is_leftside = (recv_data[3] >> 1) & 1;
                        is_rightside = recv_data[3] & 1;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }

      if (sub_item == 0) {
            oled.PrintCenter("Line", 64, 32);
      } else if (sub_item == 1) {
            oled.PrintCenterY("Dir: ", 0, 20);
            oled.Print(String(line_dir).c_str());
            oled.PrintCenterY("Inside: ", 0, 32);
            oled.Print(String(line_inside_dir).c_str());
            oled.PrintCenterY("Interval: ", 0, 44);
            oled.Print(String(line_interval).c_str());

            if (set_val != 0) reset_line = 1;
            if (success_reset_line == 1) reset_line = 0;
            if (success_reset_line == 0 && reset_line == 1) {
                  for (uint8_t i = 0; i < 16; i++) led.SetPixelColor(i, 255, 255, 255);
            }
      } else {
            sub_item = 0;
            reset_line = 0;
      }

      if (is_on_line == 1) led.SetDegree(line_inside_dir, 255, 255, 255);
      if (is_leftside) led.SetPixelColor(12, 0, 0, 255);
      if (is_rightside) led.SetPixelColor(4, 0, 0, 255);
}

#endif