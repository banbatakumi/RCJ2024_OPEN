#include "hardware.hpp"

int dir;
int debug;

Hardware::Hardware() {
}

void Hardware::Init() {
      // 諸々の初期化
      serial3.init();
      m1n_serial[0].init();
      m1n_serial[1].init();
      m1n_serial[2].init();
      m1n_serial[3].init();
      led1.init();
      led2.init();
      led3.init();
      led4.init();
}

void Hardware::MainUart() {
      while (serial3.available()) info.yaw = serial3.read() * 2 - 180;

      if (main_send_interval_timer.read_us() >= MAIN_SEND_PERIOD_US) {
            static const uint8_t HEADER = 0xFF;  // ヘッダ
            static const uint8_t FOOTER = 0xAA;  // ヘッダ

            static const uint8_t data_size = 13;
            uint8_t send_data[data_size];
            send_data[0] = HEADER;
            send_data[1] = (uint8_t)(((uint16_t)(info.ball_dir + 32768) & 0xFF00) >> 8);
            send_data[2] = (uint8_t)((uint16_t)(info.ball_dir + 32768) & 0x00FF);
            send_data[3] = info.ball_dis;
            send_data[4] = info.yellow_goal_dir * 0.5 + 90;
            send_data[5] = info.yellow_goal_dis;
            send_data[6] = info.blue_goal_dir * 0.5 + 90;
            send_data[7] = info.blue_goal_dis;
            send_data[8] = (info.Cam[2].is_goal_front << 1) | info.Cam[0].is_goal_front;
            send_data[9] = info.own_x * 0.5 + 127;
            send_data[10] = info.own_y * 0.5 + 127;
            send_data[11] = info.Cam[0].proximity;
            send_data[12] = FOOTER;
            serial3.write(send_data, data_size);

            main_send_interval_timer.reset();
      }
}

void Hardware::M1nUart() {
      static const uint8_t HEADER = 0xFF;      // ヘッダ
      static const uint8_t FOOTER = 0xAA;      // フッタ
      static const uint8_t data_size = 7;      // データのサイズ
      static uint8_t index[4] = {0, 0, 0, 0};  // 受信したデータのインデックスカウンター
      static uint8_t recv_data[4][data_size];  // 受信したデータ
      static uint8_t recv_byte[4];

      static const uint8_t filter_num = 10;
      static bool is_goal_front[4][filter_num];
      static uint8_t j[4] = {0, 0, 0, 0};
      for (uint8_t i = 0; i < 4; i++) {
            while (m1n_serial[i].available()) {
                  recv_byte[i] = m1n_serial[i].read();
                  if (index[i] == 0) {
                        if (recv_byte[i] == HEADER) {
                              index[i]++;
                        } else {
                              index[i] = 0;
                        }
                  } else if (index[i] == (data_size + 1)) {
                        if (recv_byte[i] == FOOTER) {
                              uint8_t goal_dir, goal_dis, is_goal_yellow;

                              info.Cam[i].ball_dir = recv_data[i][0];
                              info.Cam[i].ball_dis = recv_data[i][1];
                              goal_dir = recv_data[i][2];
                              goal_dis = recv_data[i][3];
                              info.Cam[i].wall_dis = recv_data[i][4];
                              is_goal_yellow = recv_data[i][5] & 1;
                              info.Cam[i].proximity = recv_data[i][6];

                              // is_goal_frontにフィルタ
                              if (j[i] >= filter_num) j[i] = 0;
                              is_goal_front[i][j[i]] = (recv_data[i][5] >> 1) & 1;
                              info.Cam[i].is_goal_front = 0;
                              uint8_t cnt = 0;
                              for (uint8_t k = 0; k < filter_num; k++) cnt += is_goal_front[i][k];
                              if (cnt == filter_num) info.Cam[i].is_goal_front = 1;
                              j[i]++;

                              info.Cam[i].yellow_goal_dir = 0;
                              info.Cam[i].yellow_goal_dis = 0;
                              info.Cam[i].blue_goal_dir = 0;
                              info.Cam[i].blue_goal_dis = 0;
                              if (is_goal_yellow) {
                                    info.Cam[i].yellow_goal_dir = goal_dir;
                                    info.Cam[i].yellow_goal_dis = goal_dis;
                              } else {
                                    info.Cam[i].blue_goal_dir = goal_dir;
                                    info.Cam[i].blue_goal_dis = goal_dis;
                              }

                              info.Cam[i].is_camera_initialized = 1;

                              m1n_serial[i].write(info.yaw * 0.5 + 90);
                        }
                        index[i] = 0;
                  } else {
                        recv_data[i][index[i] - 1] = recv_byte[i];
                        index[i]++;
                  }
            }
      }
}