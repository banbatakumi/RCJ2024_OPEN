#ifndef _HOME_H_
#define _HOME_H_

#include "setup.h"

#define LOW_VOLTAGE 8
#define HIGH_VOLTAGE 11

int16_t debug_val[2];
int16_t yaw;
float voltage;
bool is_connect_to_ally;
bool is_ally_catch_ball;
bool is_ally_moving;
bool is_robot_initialized;

void Home() {
      static const uint8_t HEADER = 0xFF;   // ヘッダ
      static const uint8_t FOOTER = 0xAA;   // ヘッダ
      static const uint8_t data_size = 8;   // データのサイズ
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
                        voltage = recv_data[0] / 20.00;
                        is_robot_initialized = recv_data[1] & 1;
                        success_reset_yaw = (recv_data[1] >> 1) & 1;
                        yaw = ((((uint16_t)recv_data[2] << 8) & 0xFF00) | ((int16_t)recv_data[3] & 0x00FF)) - 32768;
                        debug_val[0] = ((((uint16_t)recv_data[4] << 8) & 0xFF00) | ((int16_t)recv_data[5] & 0x00FF)) - 32768;
                        debug_val[1] = ((((uint16_t)recv_data[6] << 8) & 0xFF00) | ((int16_t)recv_data[7] & 0x00FF)) - 32768;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }
      if (mode == 0) {
            if (voltage > 6) {
                  oled.PrintCenterY("Battery: ", 0, 5);
            } else {
                  oled.PrintCenterY("USB: ", 0, 5);
            }
            oled.Print(String(voltage).c_str());
            oled.Print("v");
            oled.DrawLine(0, 15, 128, 15);

            if (voltage < LOW_VOLTAGE && (sub_item == 1 || sub_item == 2)) {
                  oled.DrawFrame(13, 42, 102, 15);
                  oled.PrintCenter("Can't start", 64, 48);
            }

            float adjusted_voltage = voltage;
            if (voltage < LOW_VOLTAGE) adjusted_voltage = LOW_VOLTAGE;
            if (voltage > HIGH_VOLTAGE) adjusted_voltage = HIGH_VOLTAGE;
            if (is_robot_initialized) {
                  for (uint8_t i = 0; i < 16; i++) led.SetPixelColor(i, 255 * (1 - (float(adjusted_voltage - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE))), 255 * (float(adjusted_voltage - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE)), 0);
            } else {
                  static uint16_t i = 0;
                  i += 20;
                  led.SetDegree(SimplifyDeg(i), 255 * (1 - (float(adjusted_voltage - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE))), 255 * (float(adjusted_voltage - LOW_VOLTAGE) / (HIGH_VOLTAGE - LOW_VOLTAGE)), 0);
            }

            if (is_connect_to_ally) {
                  oled.DrawLine(5, 25, 15, 35);
                  oled.DrawLine(5, 35, 15, 25);
                  oled.DrawLine(10, 20, 10, 40);
                  oled.DrawLine(10, 20, 15, 25);
                  oled.DrawLine(10, 40, 15, 35);
            }
            if (is_ally_moving) {
                  oled.DrawLine(123, 20, 118, 20);
                  oled.DrawLine(123, 25, 118, 25);
                  oled.DrawLine(123, 30, 118, 30);
                  oled.DrawFrame(105, 20, 11, 11);
            }
            if (is_ally_catch_ball) {
                  oled.DrawLine(105, 40, 105, 45);
                  oled.DrawLine(105, 45, 123, 45);
                  oled.DrawLine(123, 40, 123, 45);
                  oled.DrawCircle(114, 40, 4);
            }
      }
      if (sub_item == 0) {
            oled.PrintCenter("Home", 64, 32);
            oled.PrintCenter(String(yaw).c_str(), 64, 48);
            mode = 0;
      } else if (sub_item == 1) {
            if (mode == 0) {
                  oled.PrintCenter("OF", 40, 32);
                  oled.PrintCenter("DF", 88, 32);
            }
            if (set_val == -1 && voltage > LOW_VOLTAGE) mode = 1;
            if (set_val == 1 && voltage > LOW_VOLTAGE) mode = 2;
            if (pre_sub_item == 0) reset_yaw = 1;
            if (success_reset_yaw == 1) reset_yaw = 0;
            if (success_reset_yaw == 0 && reset_yaw == 1) {
                  for (uint8_t i = 0; i < 16; i++) led.SetPixelColor(i, 255, 255, 255);
            }

      } else if (sub_item == 2) {
            if (mode == 0) {
                  oled.PrintCenter("DEV", 40, 32);
                  oled.PrintCenter("PID", 88, 32);
            } else {
                  oled.PrintCenterY("0: ", 0, 14);
                  oled.Print(String(debug_val[0]).c_str());
                  oled.PrintCenterY("1: ", 0, 26);
                  oled.Print(String(debug_val[1]).c_str());
            }
            if (set_val == -1 && voltage > LOW_VOLTAGE) mode = 3;
            if (set_val == 1 && voltage > LOW_VOLTAGE) mode = 4;
      } else {
            sub_item = 0;
            reset_yaw = 0;
      }
}

#endif