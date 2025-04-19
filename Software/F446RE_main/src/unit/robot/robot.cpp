#include "robot.hpp"

uint16_t adc_get_val[3];

Robot::Robot() {
      catch_front.SetLength(10);
      catch_back.SetLength(10);
      ball_vel_x_ave.SetLength(3);
      ball_vel_y_ave.SetLength(3);
}

void Robot::HardwareInit() {
      HAL_ADC_Start(&hadc1);
      HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_get_val, 3);
      for (uint8_t i = 0; i < 3; i++) {
            while (!(adc_get_val[i] > 0));
      }

      // 諸々の初期化
      serial1.init();
      serial2.init();
      serial3.init();
      serial4.init();
      serial5.init();
      ledh.init();
      motor1a.init();
      motor1b.init();
      motor2a.init();
      motor2b.init();
      motor3a.init();
      motor3b.init();
      motor4a.init();
      motor4b.init();
      dribbler_front_a.init();
      dribbler_front_b.init();
      dribbler_back_a.init();
      dribbler_back_b.init();

      // 接続確認
      motor.CheckConnection();
      HAL_Delay(200);
      dribbler_front.CheckConnection();
      dribbler_back.CheckConnection();
}

void Robot::GetSensors() {
      static float pre_voltage = 9.6;
      info.voltage = adc_get_val[0] * VOLTAGE_CONVERTION;
      info.voltage = info.voltage * (1 - VOLTAGE_RC) + pre_voltage * VOLTAGE_RC;
      if (info.voltage > 12.5) info.voltage = 12.5;
      pre_voltage = info.voltage;

      catch_front.Compute(adc_get_val[1]);
      catch_back.Compute(adc_get_val[2]);
      info.Catch.front_val = catch_front.Get();
      info.Catch.back_val = catch_back.Get();

      info.Catch.is_front = info.Catch.front_val < CATCH_FRONT_TH ? 1 : 0;
      info.Catch.is_back = info.Catch.back_val < CATCH_BACK_TH ? 1 : 0;
}

void Robot::ImuUart() {
      while (serial1.available()) {
            static const uint8_t HEADER = 0xFF;  // ヘッダ
            static const uint8_t FOOTER = 0xAA;  // ヘッダ
            static const uint8_t dataSize = 6;   // データのサイズ
            static uint8_t index = 0;            // 受信したデータのインデックスカウンター
            static uint8_t recv_data[dataSize];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial1.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (dataSize + 1)) {
                  if (recv_byte == FOOTER) {
                        if (info.Imu.is_initialized == 0) info.Imu.is_initialized = 1;
                        info.Imu.yaw = MyMath::NormalizeDeg180(((((uint16_t)recv_data[0] << 8) & 0xFF00) | ((int16_t)recv_data[1] & 0x00FF)) * 0.1 - 180 - info.Imu.yaw_correction_val);
                        info.Imu.pitch = ((((uint16_t)recv_data[2] << 8) & 0xFF00) | ((int16_t)recv_data[3] & 0x00FF)) * 0.1 - 180;
                        info.Imu.roll = ((((uint16_t)recv_data[4] << 8) & 0xFF00) | ((int16_t)recv_data[5] & 0x00FF)) * 0.1 - 180;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }
}

void Robot::LineUart() {
      while (serial2.available()) {
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // フッタ
            static const uint8_t data_size = 7;   // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial2.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        if (info.Line.is_initialized == 0) info.Line.is_initialized = 1;

                        bool is_half_out;
                        info.motor_rad_s[0] = recv_data[0] / TWO_PI;
                        info.motor_rad_s[1] = recv_data[1] / TWO_PI;
                        info.motor_rad_s[2] = recv_data[2] / TWO_PI;
                        info.motor_rad_s[3] = recv_data[3] / TWO_PI;
                        info.Line.interval = recv_data[4] >> 4;
                        info.Line.is_on_line = (recv_data[4] >> 3) & 1;
                        is_half_out = (recv_data[4] >> 2) & 1;
                        info.Line.is_leftside = (recv_data[4] >> 1) & 1;
                        info.Line.is_rightside = recv_data[4] & 1;
                        info.Ui.success_reset_line = recv_data[5] & 1;
                        info.Line.dir = recv_data[6] * 2 - 180;

                        if (is_half_out == 1) {
                              info.Line.inside_dir = info.Line.dir;
                              info.Line.depth = 24 - info.Line.interval;
                        } else {
                              info.Line.inside_dir = MyMath::NormalizeDeg180(info.Line.dir + 180);
                              info.Line.depth = info.Line.interval;
                        }
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }

      if (line_send_interval_timer.read_us() >= LINE_SEND_PERIOD_US) {
            static const uint8_t HEADER = 0xFF;  // ヘッダ
            static const uint8_t FOOTER = 0xAA;  // ヘッダ
            static const uint8_t data_size = 4;
            uint8_t send_data[data_size];
            send_data[0] = HEADER;
            send_data[1] = info.Line.on_led;
            send_data[2] = info.Ui.reset_line;
            send_data[3] = FOOTER;
            serial2.write(send_data, data_size);
            line_send_interval_timer.reset();
      }
}

void Robot::UiUart() {
      while (serial3.available()) {
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // ヘッダ
            static const uint8_t data_size = 4;   // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial3.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        info.Ui.item = ((recv_data[0] >> 4) & 0b00001111) - 8;
                        info.mode = recv_data[0] & 0b00001111;
                        info.Ui.sub_item = (recv_data[1] >> 5) & 0b00000111;
                        info.Ui.dribbler_sig = (recv_data[1] >> 2) & 0b0000111;
                        info.Ui.reset_yaw = (recv_data[1] >> 1) & 0b0000001;
                        info.Ui.reset_line = recv_data[1] & 0b0000001;
                        info.target_move_speed = recv_data[2] * 0.01;
                        info.target_line_move_speed = recv_data[3] * 0.01;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }

      if (info.mode == 0 || info.mode == 3 || info.mode == 4) {
            if (ui_send_interval_timer.read_us() >= UI_SEND_PERIOD_US) {
                  static const uint8_t HEADER = 0xFF;  // ヘッダ
                  static const uint8_t FOOTER = 0xAA;  // ヘッダ
                  if (info.Ui.item == -2) {
                        static const uint8_t data_size = 1;
                        uint8_t send_data[data_size];
                        send_data[0] = info.Catch.is_front << 1 | info.Catch.is_back;
                        serial3.write(send_data, data_size);
                  } else if (info.Ui.item == 0) {
                        static const uint8_t data_size = 10;
                        uint8_t send_data[data_size];
                        send_data[0] = HEADER;
                        send_data[1] = info.voltage * 20;
                        send_data[2] = (info.Ui.success_reset_yaw << 1) | (info.Imu.is_initialized & info.Line.is_initialized & info.Cam.is_initialized);
                        send_data[3] = (uint8_t)(((uint16_t)(info.Imu.yaw + 32768) & 0xFF00) >> 8);
                        send_data[4] = (uint8_t)((uint16_t)(info.Imu.yaw + 32768) & 0x00FF);
                        send_data[5] = (uint8_t)(((uint16_t)(info.Ui.debug[0] + 32768) & 0xFF00) >> 8);
                        send_data[6] = (uint8_t)((uint16_t)(info.Ui.debug[0] + 32768) & 0x00FF);
                        send_data[7] = (uint8_t)(((uint16_t)(info.Ui.debug[1] + 32768) & 0xFF00) >> 8);
                        send_data[8] = (uint8_t)((uint16_t)(info.Ui.debug[1] + 32768) & 0x00FF);
                        send_data[9] = FOOTER;
                        serial3.write(send_data, data_size);
                  } else if (info.Ui.item == 1) {
                        static const uint8_t data_size = 6;
                        uint8_t send_data[data_size];
                        send_data[0] = HEADER;
                        send_data[1] = info.Line.dir * 0.5 + 90;
                        send_data[2] = info.Line.inside_dir * 0.5 + 90;
                        send_data[3] = info.Line.interval;
                        send_data[4] = (info.Ui.success_reset_line << 3) | (info.Line.is_on_line << 2) | (info.Line.is_leftside << 1) | info.Line.is_rightside;
                        send_data[5] = FOOTER;
                        serial3.write(send_data, data_size);
                  } else if (info.Ui.item == 2) {
                        if (info.Ui.sub_item == 1) {
                              static const uint8_t data_size = 6;
                              uint8_t send_data[data_size];
                              send_data[0] = HEADER;
                              send_data[1] = info.Cam.ball_dir * 0.5 + 90;
                              send_data[2] = info.Cam.ball_dis;
                              send_data[3] = info.Cam.ball_vel_x * 100 + 127;
                              send_data[4] = info.Cam.ball_vel_y * 100 + 127;
                              send_data[5] = FOOTER;
                              serial3.write(send_data, data_size);
                        } else if (info.Ui.sub_item == 2) {
                              static const uint8_t data_size = 6;
                              uint8_t send_data[data_size];
                              send_data[0] = HEADER;
                              send_data[1] = info.Cam.yellow_goal_dir * 0.5 + 90;
                              send_data[2] = info.Cam.yellow_goal_dis;
                              send_data[3] = info.Cam.blue_goal_dir * 0.5 + 90;
                              send_data[4] = info.Cam.blue_goal_dis;
                              send_data[5] = FOOTER;
                              serial3.write(send_data, data_size);
                        } else if (info.Ui.sub_item == 3) {
                              static const uint8_t data_size = 7;
                              uint8_t send_data[data_size];
                              send_data[0] = HEADER;
                              send_data[1] = info.Cam.center_dir * 0.5 + 90;
                              send_data[2] = info.Cam.center_dis;
                              send_data[3] = info.Cam.own_x + 127;
                              send_data[4] = info.Cam.own_y + 127;
                              send_data[5] = info.Cam.proximity[0];
                              send_data[6] = FOOTER;
                              serial3.write(send_data, data_size);
                        }
                  }

                  ui_send_interval_timer.reset();
            }
      }
}

void Robot::CamUart() {
      while (serial4.available()) {
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // フッタ
            static const uint8_t data_size = 11;  // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            static int16_t pre_ball_x = 0;
            static int16_t pre_ball_y = 0;

            recv_byte = serial4.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        if (info.Cam.is_initialized == 0) info.Cam.is_initialized = 1;
                        int16_t wall_own_x, wall_own_y;
                        info.Cam.ball_dir = MyMath::NormalizeDeg180(((((uint16_t)recv_data[0] << 8) & 0xFF00) | ((int16_t)recv_data[1] & 0x00FF)) - 32768 + info.Esp32.top_yaw);
                        info.Cam.ball_dis = recv_data[2];
                        info.Cam.yellow_goal_dir = MyMath::NormalizeDeg180(recv_data[3] * 2 - 180 + info.Esp32.top_yaw);
                        info.Cam.yellow_goal_dis = recv_data[4];
                        info.Cam.blue_goal_dir = MyMath::NormalizeDeg180(recv_data[5] * 2 - 180 + info.Esp32.top_yaw);
                        info.Cam.blue_goal_dis = recv_data[6];
                        info.Cam.is_goal_front = recv_data[7] & 1;
                        info.Cam.is_goal_back = (recv_data[7] >> 1) & 1;
                        wall_own_x = (recv_data[8] - 127) * 2;
                        wall_own_y = (recv_data[9] - 127) * 2;
                        info.Cam.proximity[0] = recv_data[10];

                        // ボールをxy座標に変換
                        info.Cam.ball_x = info.Cam.ball_dis * MyMath::sinDeg(info.Cam.ball_dir);
                        info.Cam.ball_y = info.Cam.ball_dis * MyMath::cosDeg(info.Cam.ball_dir);

                        // ボール座標を微分して速度を出す
                        double elapsed_time = ball_vel_timer.read();
                        if (elapsed_time > 0.1) {
                              info.Cam.ball_vel_x = ((info.Cam.ball_x - pre_ball_x) / elapsed_time) * 0.01;
                              info.Cam.ball_vel_y = ((info.Cam.ball_y - pre_ball_y) / elapsed_time) * 0.01;
                              pre_ball_x = info.Cam.ball_x;
                              pre_ball_y = info.Cam.ball_y;
                              info.Cam.ball_vel_x *= 0.5;
                              info.Cam.ball_vel_y *= 0.5;
                              if (abs(info.Cam.ball_vel_x) > 2) info.Cam.ball_vel_x = 2 * (info.Cam.ball_vel_x / abs(info.Cam.ball_vel_x));
                              if (abs(info.Cam.ball_vel_y) > 2) info.Cam.ball_vel_y = 2 * (info.Cam.ball_vel_y / abs(info.Cam.ball_vel_y));
                              ball_vel_x_ave.Compute(info.Cam.ball_vel_x);
                              ball_vel_y_ave.Compute(info.Cam.ball_vel_y);
                              info.Cam.ball_vel_x = ball_vel_x_ave.Get();
                              info.Cam.ball_vel_y = ball_vel_y_ave.Get();
                              info.Cam.ball_vel = MyMath::sqrt(info.Cam.ball_vel_x * info.Cam.ball_vel_x + info.Cam.ball_vel_y * info.Cam.ball_vel_y);

                              ball_vel_timer.reset();
                        }

                        if (info.Cam.yellow_goal_dis != 0 || info.Cam.blue_goal_dis != 0) {
                              if (abs(info.Cam.yellow_goal_dir) <= 90 && (abs(info.Cam.blue_goal_dir) >= 90 || info.Cam.blue_goal_dis == 0)) {
                                    info.Cam.ops_goal_dir = info.Cam.yellow_goal_dir;
                                    info.Cam.ops_goal_dis = info.Cam.yellow_goal_dis;
                                    info.Cam.own_goal_dir = info.Cam.blue_goal_dir;
                                    info.Cam.own_goal_dis = info.Cam.blue_goal_dis;
                              } else if (abs(info.Cam.blue_goal_dir) <= 90 && (abs(info.Cam.yellow_goal_dir) >= 90 || info.Cam.yellow_goal_dis == 0)) {
                                    info.Cam.ops_goal_dir = info.Cam.blue_goal_dir;
                                    info.Cam.ops_goal_dis = info.Cam.blue_goal_dis;
                                    info.Cam.own_goal_dir = info.Cam.yellow_goal_dir;
                                    info.Cam.own_goal_dis = info.Cam.yellow_goal_dis;
                              }
                        }

                        // 自己位置
                        int16_t goal_own_x, goal_own_y;
                        goal_own_x = -info.Cam.ops_goal_dis * MyMath::sinDeg(info.Cam.ops_goal_dir) - info.Cam.own_goal_dis * MyMath::sinDeg(info.Cam.own_goal_dir);
                        goal_own_y = -info.Cam.ops_goal_dis * MyMath::cosDeg(info.Cam.ops_goal_dir) - info.Cam.own_goal_dis * MyMath::cosDeg(info.Cam.own_goal_dir);
                        if (info.Cam.yellow_goal_dis != 0 && info.Cam.blue_goal_dis != 0) {
                              info.Cam.own_x = (goal_own_x + wall_own_x) / 2;
                              info.Cam.own_y = (goal_own_y + wall_own_y) / 2;
                        } else {
                              info.Cam.own_x = wall_own_x;
                              info.Cam.own_y = wall_own_y;
                        }
                        info.Cam.center_dir = MyMath::atan2(info.Cam.own_x, info.Cam.own_y);
                        info.Cam.center_dis = MyMath::sqrt(info.Cam.own_x * info.Cam.own_x + info.Cam.own_y * info.Cam.own_y);

                        serial4.write(info.Imu.yaw * 0.5 + 90);
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }
}

void Robot::Esp32Uart() {
      while (serial5.available()) {
#ifdef bluetooth
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // フッタ
            static const uint8_t data_size = 4;   // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial5.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        info.Esp32.top_yaw = recv_data[0] * 2 - 180;
                        info.Esp32.ir_dir = recv_data[1] * 2 - 180;
                        info.Esp32.ir_dis = recv_data[2];
                        info.Esp32.Bluetooth.is_connected = recv_data[3] & 1;
                        info.Esp32.Bluetooth.Ally.is_moving = (recv_data[3] >> 1) & 1;
                        info.Esp32.Bluetooth.Ally.is_defence = (recv_data[3] >> 2) & 1;
                        info.Esp32.Bluetooth.Ally.is_holding_ball = (recv_data[3] >> 3) & 1;
                        info.Esp32.Bluetooth.Ally.can_get_pass = (recv_data[3] >> 4) & 1;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
#endif
#ifdef wifi
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // フッタ
            static const uint8_t data_size = 7;   // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial5.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        info.Esp32.top_yaw = recv_data[0] * 2 - 180;
                        info.Esp32.ir_dir = recv_data[1] * 2 - 180;
                        info.Esp32.ir_dis = recv_data[2];
                        info.Esp32.Wifi.move_dir = recv_data[3] * 2 - 180;
                        info.Esp32.Wifi.move_speed = recv_data[4] * 0.01;
                        info.Esp32.Wifi.face_angle = recv_data[5] * 2 - 180;
                        info.Esp32.Wifi.do_kick = recv_data[6] & 1;
                        info.Esp32.Wifi.do_dribble = (recv_data[6] >> 1) & 1;
                        info.Esp32.Wifi.stop = (recv_data[6] >> 2) & 1;
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
#endif
      }

      if (esp32_send_interval_timer.read_us() >= ESP32_SEND_PERIOD_US) {
#ifdef bluetooth
            static uint8_t send_data;
            if (abs(info.Imu.yaw) > 40 && info.mode == 0) info.Esp32.do_rotate = 0;
            send_data = (info.Esp32.Bluetooth.Me.can_get_pass << 5) | (info.Esp32.Bluetooth.Me.is_holding_ball << 4) | (info.Esp32.Bluetooth.Me.is_defence << 3) | (info.Esp32.Bluetooth.Me.is_moving << 2) | (info.Esp32.on_ir_led << 1) | info.Esp32.do_rotate;
            serial5.write(send_data);
#endif
#ifdef wifi
            static const uint8_t HEADER = 0xFF;  // ヘッダ
            static const uint8_t FOOTER = 0xAA;  // ヘッダ
            static const uint8_t data_size = 17;
            uint8_t send_data[data_size];
            send_data[0] = HEADER;
            send_data[1] = (info.Catch.is_front << 4) | (info.Catch.is_back << 3) | (info.Line.is_on_line << 2) | (info.Esp32.on_ir_led << 1) | info.Esp32.do_rotate;
            send_data[2] = info.voltage * 20;
            send_data[3] = motor.moving_dir * 0.5 + 90;
            send_data[4] = motor.moving_speed * 100;
            send_data[5] = info.Cam.own_x * 0.5 + 127;
            send_data[6] = info.Cam.own_y * 0.5 + 127;
            send_data[7] = info.Imu.yaw * 0.5 + 90;
            send_data[8] = info.Cam.ball_dir * 0.5 + 90;
            send_data[9] = info.Cam.ball_dis;
            send_data[10] = info.Cam.yellow_goal_dir * 0.5 + 90;
            send_data[11] = info.Cam.yellow_goal_dis;
            send_data[12] = info.Cam.blue_goal_dir * 0.5 + 90;
            send_data[13] = info.Cam.blue_goal_dis;
            send_data[14] = info.Line.inside_dir * 0.5 + 90;
            send_data[15] = info.Line.depth;
            send_data[16] = FOOTER;
            serial5.write(send_data, data_size);
#endif
            esp32_send_interval_timer.reset();
      }
}