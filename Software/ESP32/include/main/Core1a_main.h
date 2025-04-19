#ifndef _CORE1A_MAIN_H_
#define _CORE1A_MAIN_H_

#include "setup.h"

#define CORE1A_CONTROL_FREQ 500  // Hz

void Core1a_setup() {
      Serial2.begin(115200, SERIAL_8N1, 16, 17);
}

void Core1a_loop() {
#ifdef bluetooth
      // UART送信
      const uint8_t send_byte_num = 6;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = yaw / 2 + 90;
      send_byte[2] = ir.GetDir() / 2 + 90;
      send_byte[3] = ir.GetDis();
      send_byte[4] = can_ally_get_pass << 4 | is_ally_catch_ball << 3 | is_ally_defense << 2 | is_ally_moving << 1 | is_connect;
      send_byte[5] = 0xAA;
      Serial2.write(send_byte, send_byte_num);
      if (Serial2.available()) {
            uint8_t recv_data = Serial2.read();
            do_rotate = (recv_data) & 1;
            on_ir_led = (recv_data >> 1) & 1;
            is_moving = (recv_data >> 2) & 1;
            is_defense = (recv_data >> 3) & 1;
            is_catch_ball = (recv_data >> 4) & 1;
            can_get_pass = (recv_data >> 5) & 1;
      }
#endif
#ifdef wifi
      // UART送信
      const uint8_t send_byte_num = 9;
      uint8_t send_byte[send_byte_num];
      send_byte[0] = 0xFF;
      send_byte[1] = yaw / 2 + 90;
      send_byte[2] = ir.GetDir() / 2 + 90;
      send_byte[3] = ir.GetDis();
      send_byte[4] = move_dir / 2 + 90;
      send_byte[5] = move_speed * 100;
      send_byte[6] = face_angle / 2 + 90;
      send_byte[7] = (stop << 2) | (do_dribble << 1) | do_kick;
      send_byte[8] = 0xAA;
      Serial2.write(send_byte, send_byte_num);

      static const uint8_t HEADER = 0xFF;   // ヘッダ
      static const uint8_t FOOTER = 0xAA;   // ヘッダ
      static const uint8_t data_size = 15;  // データのサイズ
      static uint8_t index = 0;             // 受信したデータのインデックスカウンター
      static uint8_t recv_data[data_size];  // 受信したデータ
      static uint8_t recv_byte;
      while (Serial2.available()) {
            recv_byte = Serial2.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        do_rotate = (recv_data[0]) & 1;
                        on_ir_led = (recv_data[0] >> 1) & 1;
                        is_on_line = (recv_data[0] >> 2) & 1;
                        is_hold_ball_front = (recv_data[0] >> 3) & 1;
                        is_hold_ball_back = (recv_data[0] >> 4) & 1;
                        voltage = recv_data[1] / 20.00;
                        moving_dir = recv_data[2] * 2 - 180;
                        moving_speed = recv_data[3] * 0.01;
                        own_x = (recv_data[4] - 127) * 2;
                        own_y = (recv_data[5] - 127) * 2;
                        under_yaw = recv_data[6] * 2 - 180;
                        ball_dir = recv_data[7] * 2 - 180;
                        ball_dis = recv_data[8];
                        yellow_goal_dir = recv_data[9] * 2 - 180;
                        yellow_goal_size = recv_data[10];
                        blue_goal_dir = recv_data[11] * 2 - 180;
                        blue_goal_size = recv_data[12];
                        line_inside_dir = recv_data[13] * 2 - 180;
                        line_depth = recv_data[14];
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }
#endif
}

#endif