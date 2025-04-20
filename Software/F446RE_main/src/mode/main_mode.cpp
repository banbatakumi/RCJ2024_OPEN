#include "mode.hpp"

// cortex-debug用グローバル変数
int yaw;
float voltage;
int process_freq;
int line_dir;
int is_on_line;
int encoder[4];

void Mode::MainMode() {
      process_timer.reset();
      // メイン処理

      // UART通信
      robot->ImuUart();
      robot->LineUart();
      robot->UiUart();
      robot->CamUart();
      robot->Esp32Uart();

      // センサー情報取得
      robot->GetSensors();

      // 動いている時かデバッグモードの時ラインをつける
      robot->info.Line.on_led = false;
      if (robot->info.mode != 0 || robot->info.Ui.item == 1) robot->info.Line.on_led = true;

      // 　yawのリセット
      if (robot->info.Ui.reset_yaw == false) robot->info.Ui.success_reset_yaw = false;
      if (robot->info.Ui.reset_yaw == true && robot->info.Ui.success_reset_yaw == false) {
            robot->info.Imu.yaw_correction_val = robot->info.Imu.yaw + robot->info.Imu.yaw_correction_val;
            robot->info.Ui.success_reset_yaw = true;
      }

      robot->info.Esp32.do_rotate = false;
      if (robot->info.Ui.item == 0 && robot->info.Ui.sub_item != 0) robot->info.Esp32.do_rotate = true;

      // モード切り替え
      if (robot->info.mode == 0) {
            robot->motor.Free();

            if (robot->info.Ui.dribbler_sig == 0) {
                  robot->dribbler_front.Hold(0);
                  robot->dribbler_back.Hold(0);
            } else if (robot->info.Ui.dribbler_sig == 1) {
                  if (robot->info.Catch.is_front) {
                        robot->dribbler_front.Hold(HOLD_MAX_POWER);
                  } else {
                        robot->dribbler_front.Hold(HOLD_WAIT_POWER);
                  }
            } else if (robot->info.Ui.dribbler_sig == 2) {
                  robot->kicker.Kick();
            } else if (robot->info.Ui.dribbler_sig == 3) {
                  if (robot->info.Catch.is_back) {
                        robot->dribbler_back.Hold(HOLD_MAX_POWER);
                  } else {
                        robot->dribbler_back.Hold(HOLD_WAIT_POWER);
                  }
            }
      } else if (robot->info.mode == 1) {
            Offence();
      } else if (robot->info.mode == 2) {
            Defence();
      } else if (robot->info.mode == 3) {  // debug mode
            if (robot->info.Esp32.Wifi.stop) {
                  robot->motor.Drive(0, 0, 5, robot->info.Esp32.Wifi.face_angle, PI, FRONT);
            } else {
                  robot->motor.Drive(robot->info.Esp32.Wifi.move_dir, robot->info.Esp32.Wifi.move_speed, 5, robot->info.Esp32.Wifi.face_angle, PI, FRONT);
            }
            if (robot->info.Esp32.Wifi.do_kick) {
                  robot->dribbler_front.Brake(1);
                  robot->kicker.Kick();
            }
            if (robot->info.Esp32.Wifi.do_dribble) {
                  robot->dribbler_front.Hold(HOLD_MAX_POWER);
            } else {
                  robot->dribbler_front.Hold(0);
            }
            robot->info.Ui.debug[0] = robot->info.Esp32.Wifi.move_dir;
            robot->info.Ui.debug[1] = robot->info.Esp32.Wifi.move_speed;
      } else if (robot->info.mode == 4) {  // debug mode
            robot->motor.Drive(0);
            robot->info.Ui.debug[0] = robot->info.motor_rad_s[1] * 100;
            robot->info.Ui.debug[1] = 0;
      }

      // cortex-debug
      // yaw = robot->info.Imu.yaw;
      // voltage = robot->info.voltage;
      // line_dir = robot->info.Line.dir;
      // is_on_line = robot->info.Line.is_on_line;

      // encoder[0] = robot->info.motor_rad_s[0];
      // encoder[1] = robot->info.motor_rad_s[1];
      // encoder[2] = robot->info.motor_rad_s[2];
      // encoder[3] = robot->info.motor_rad_s[3];

      //  定周期処理
      process_time_ = process_timer.read_us();
      if (process_time_ < PERIOD_US) {
            while (process_timer.read_us() < PERIOD_US);  // 余剰時間を待つ
      }
      process_freq = 1.0f / (process_timer.read_us() * 0.000001f);
}