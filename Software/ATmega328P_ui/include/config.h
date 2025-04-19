#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#include "mode/camera.h"
#include "mode/dribbler.h"
#include "mode/home.h"
#include "mode/line.h"
#include "mode/speed.h"
#include "setup.h"

void ModeRun() {
      oled.FirstPage();
      do {
            led.Clear();  // NeoPixelを消す

            if (sub_item == 0) {
                  if (item > -2) oled.DrawTriangle(10, 53, 0, 58, 10, 63);
                  if (item < 2) oled.DrawTriangle(118, 53, 128, 58, 118, 63);
            }

            if (item == 0) {
                  Home();
            } else if (item == -1) {
                  Speed();
            } else if (item == -2) {
                  Dribbler();
            } else if (item == 1) {
                  Line();
            } else if (item == 2) {
                  Camera();
            }
            ButtonRead();

            led.Show();
            SendData();  // メインマイコンにデータを送る
      } while (oled.NextPage());
}

void SendData() {  // UART送信
      const uint8_t data_size = 6;
      const uint8_t HEADER = 0xFF;
      const uint8_t FOOTER = 0xAA;
      uint8_t send_byte[data_size];
      send_byte[0] = HEADER;
      send_byte[1] = (item + 8) << 4 | mode;
      send_byte[2] = sub_item << 5 | dribbler_sig << 2 | reset_yaw << 1 | reset_line;
      send_byte[3] = moving_speed;
      send_byte[4] = line_moving_speed;
      send_byte[5] = FOOTER;
      Serial.write(send_byte, data_size);
      Serial.flush();
}

void ButtonRead() {
      pre_sub_item = sub_item;

      for (uint8_t i = 0; i < 3; i++) {
            pre_is_button[i] = is_button[i];
            is_button[i] = digitalRead(button_pin[i]);
      }

      if (is_button[0] == 1 && pre_is_button[0] == 0) {
            if (mode == 0) {
                  tone(buzzer_pin, 1000, 50);
                  delay(50);
                  sub_item += 1;
            } else {
                  mode = 0;
            }
      }

      set_val = 0;
      if (is_button[1] == 1) {
            if (sub_item == 0 && item > -2) {
                  item -= 1;
            } else {
                  set_val -= 1;
            }
            if (pre_is_button[1] == 0) {
                  tone(buzzer_pin, 1500, 100);
                  delay(250);
            } else {
                  tone(buzzer_pin, 1500, 50);
                  delay(50);
            }
      }
      if (is_button[2] == 1) {
            if (sub_item == 0 && item < 2) {
                  item += 1;
            } else {
                  set_val += 1;
            }
            if (pre_is_button[2] == 0) {
                  tone(buzzer_pin, 1500, 100);
                  delay(250);
            } else {
                  tone(buzzer_pin, 1500, 50);
                  delay(50);
            }
      }
}

#endif