#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "setup.h"

int16_t ball_dir;
uint8_t ball_dis;
int8_t ball_vel_x;
int8_t ball_vel_y;

int16_t yellow_goal_dir;
uint8_t yellow_goal_height;
int16_t blue_goal_dir;
uint8_t blue_goal_height;

int16_t center_dir;
uint8_t center_dis;
int8_t own_x;
int8_t own_y;
uint8_t front_proximity;

void Camera() {
      if (sub_item == 0) {
            oled.PrintCenter("Camera", 64, 32);
      } else if (sub_item == 1) {
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
                              ball_dir = recv_data[0] * 2 - 180;
                              ball_dis = recv_data[1];
                              ball_vel_x = recv_data[2] - 127;
                              ball_vel_y = recv_data[3] - 127;
                        }
                        index = 0;
                  } else {
                        recv_data[index - 1] = recv_byte;
                        index++;
                  }
            }

            oled.PrintCenterY("Dir:", 0, 5);
            oled.Print(String(ball_dir).c_str());
            oled.PrintCenterY("Dis:", 0, 20);
            oled.Print(String(ball_dis).c_str());
            oled.PrintCenterY("Vx:", 0, 35);
            oled.Print(String(ball_vel_x * 0.01).c_str());
            oled.PrintCenterY("Vy:", 0, 50);
            oled.Print(String(ball_vel_y * 0.01).c_str());
            oled.DrawFilledEllipse(96 + (ball_dis / 6 + 3) * cos((ball_dir - 90) * PI / 180.000), 32 + (ball_dis / 6 + 3) * sin((ball_dir - 90) * PI / 180.000), 2, 2);
            oled.DrawEllipse(96, 32, 30, 30);
            oled.DrawEllipse(96, 32, 15, 15);
            oled.DrawLine(64, 32, 128, 32);
            oled.DrawLine(96, 0, 96, 64);

            if (ball_dis != 0) led.SetDegree(ball_dir, 255, 127, 0);
      } else if (sub_item == 2) {
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
                              yellow_goal_dir = recv_data[0] * 2 - 180;
                              yellow_goal_height = recv_data[1];
                              blue_goal_dir = recv_data[2] * 2 - 180;
                              blue_goal_height = recv_data[3];
                        }
                        index = 0;
                  } else {
                        recv_data[index - 1] = recv_byte;
                        index++;
                  }
            }

            oled.PrintCenterY("Yellow", 0, 16);
            oled.PrintCenterY("D:", 0, 32);
            oled.Print(String(yellow_goal_dir).c_str());
            oled.PrintCenterY("S:", 0, 48);
            oled.Print(String(yellow_goal_height).c_str());

            oled.PrintCenterY("Blue", 65, 16);
            oled.PrintCenterY("D:", 65, 32);
            oled.Print(String(blue_goal_dir).c_str());
            oled.PrintCenterY("S:", 65, 48);
            oled.Print(String(blue_goal_height).c_str());

            if (yellow_goal_height != 0) led.SetPixelColor(round(yellow_goal_dir / 22.5) % 16, 255, 255, 0);
            if (blue_goal_height != 0) led.SetPixelColor(round(blue_goal_dir / 22.5) % 16, 0, 0, 255);
      } else if (sub_item == 3) {
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // ヘッダ
            static const uint8_t data_size = 5;   // データのサイズ
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
                              center_dir = recv_data[0] * 2 - 180;
                              center_dis = recv_data[1];
                              own_x = recv_data[2] - 127;
                              own_y = recv_data[3] - 127;
                              front_proximity = recv_data[4];
                        }
                        index = 0;
                  } else {
                        recv_data[index - 1] = recv_byte;
                        index++;
                  }
            }

            oled.PrintCenterY("x:", 0, 8);
            oled.Print(String(own_x).c_str());
            oled.PrintCenterY("y:", 0, 24);
            oled.Print(String(own_y).c_str());
            oled.PrintCenterY("dir:", 0, 40);
            oled.Print(String(center_dir).c_str());
            oled.PrintCenterY("dis:", 0, 54);
            oled.Print(String(center_dis).c_str());

            oled.DrawFrame(74, 0, 54, 64);
            oled.DrawFrame(88, 0, 26, 5);
            oled.DrawFrame(88, 59, 26, 5);
            oled.DrawFilledEllipse(101 + own_x / 2, 32 - own_y / 1.5, 3, 3);
            oled.DrawEllipse(101, 32, 9, 9);
            oled.DrawEllipse(101, 32, 1, 1);

            if ((front_proximity >> 6) & 1) led.SetPixelColor(13, 255, 0, 0);
            if ((front_proximity >> 5) & 1) led.SetPixelColor(14, 255, 0, 0);
            if ((front_proximity >> 4) & 1) led.SetPixelColor(15, 255, 0, 0);
            if ((front_proximity >> 3) & 1) led.SetPixelColor(0, 255, 0, 0);
            if ((front_proximity >> 2) & 1) led.SetPixelColor(1, 255, 0, 0);
            if ((front_proximity >> 1) & 1) led.SetPixelColor(2, 255, 0, 0);
            if (front_proximity & 1) led.SetPixelColor(3, 255, 0, 0);
            // if ((back_proximity >> 6) & 1) led.SetPixelColor(5, 255, 0, 0);
            // if ((back_proximity >> 5) & 1) led.SetPixelColor(6, 255, 0, 0);
            // if ((back_proximity >> 4) & 1) led.SetPixelColor(7, 255, 0, 0);
            // if ((back_proximity >> 3) & 1) led.SetPixelColor(8, 255, 0, 0);
            // if ((back_proximity >> 2) & 1) led.SetPixelColor(9, 255, 0, 0);
            // if ((back_proximity >> 1) & 1) led.SetPixelColor(10, 255, 0, 0);
            // if (back_proximity & 1) led.SetPixelColor(11, 255, 0, 0);
      } else {
            sub_item = 0;
      }
}

#endif