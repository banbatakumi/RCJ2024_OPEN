#include "app.h"

#include "MyMath.hpp"
#include "Timer.hpp"
#include "config.h"
#include "hardware.hpp"

Hardware hardware;
Timer process_timer;

Timer ball_miss_timer;
Timer yellow_goal_miss_timer;
Timer blue_goal_miss_timer;

uint16_t process_time;  // us

// cortex-debug
int process_freq;

void setup() {
      hardware.Init();

      for (uint8_t i = 0; i < 10; i++) {
            hardware.led1 = i % 2;
            HAL_Delay(100);
      }
      hardware.led1 = 1;
}

void BallConversion() {
      uint8_t min_dis = 200;
      uint8_t min_dis_num = 0;
      static int16_t pre_ball_dir, pre_ball_dis;

      for (int i = 0; i < 4; i++) {  // ロボットから一番近いボールを捉えているカメラを特定
            if (hardware.info.Cam[i].ball_dis > 0 && hardware.info.Cam[i].ball_dis < min_dis) {
                  min_dis = hardware.info.Cam[i].ball_dis;
                  min_dis_num = i;
            }
      }
      if (min_dis == 200) min_dis = 0;
      hardware.info.ball_dis = min_dis;

      if (hardware.info.ball_dis != 0) ball_miss_timer.reset();

      if (hardware.info.ball_dis == 0) {  // ボールが見つからない
            if (ball_miss_timer.read_ms() > BALL_MISS_TIME) {
                  hardware.info.ball_dir = 0;
                  hardware.info.ball_dis = 0;
            } else {
                  hardware.info.ball_dir = pre_ball_dir;
                  hardware.info.ball_dis = pre_ball_dis;
            }
      } else if (min_dis_num == 0) {
            hardware.info.ball_dir = hardware.info.Cam[0].ball_dir + 315;
      } else if (min_dis_num == 1) {
            hardware.info.ball_dir = hardware.info.Cam[1].ball_dir + 45;
      } else if (min_dis_num == 2) {
            hardware.info.ball_dir = hardware.info.Cam[2].ball_dir + 135;
      } else if (min_dis_num == 3) {
            hardware.info.ball_dir = hardware.info.Cam[3].ball_dir + 225;
      }
      hardware.info.ball_dir = MyMath::NormalizeDeg180(hardware.info.ball_dir);

      pre_ball_dir = hardware.info.ball_dir;
      pre_ball_dis = hardware.info.ball_dis;
}

void YellowGoalConversion() {
      uint8_t max_height = 0;
      uint8_t max_height_num = 0;
      static uint8_t pre_yellow_goal_dis, pre_yellow_goal_dir;
      for (int i = 0; i < 4; i++) {  // ゴールの縦サイズが一番大きいカメラを特定
            if (hardware.info.Cam[i].yellow_goal_dis > max_height) {
                  max_height = hardware.info.Cam[i].yellow_goal_dis;
                  max_height_num = i;
            }
      }
      hardware.info.yellow_goal_dis = max_height;

      if (hardware.info.yellow_goal_dis != 0) {
            yellow_goal_miss_timer.reset();

            if (hardware.info.yellow_goal_dis == 0) {  // ゴールが見つからない
                  if (yellow_goal_miss_timer.read_ms() > GOAL_MISS_TIME) {
                        hardware.info.yellow_goal_dir = 0;
                        hardware.info.yellow_goal_dis = 0;
                  } else {
                        hardware.info.yellow_goal_dir = pre_yellow_goal_dir;
                        hardware.info.yellow_goal_dis = pre_yellow_goal_dis;
                  }
            } else if (max_height_num == 0) {
                  hardware.info.yellow_goal_dir = hardware.info.Cam[0].yellow_goal_dir + 315;
            } else if (max_height_num == 1) {
                  hardware.info.yellow_goal_dir = hardware.info.Cam[1].yellow_goal_dir + 45;
            } else if (max_height_num == 2) {
                  hardware.info.yellow_goal_dir = hardware.info.Cam[2].yellow_goal_dir + 135;
            } else if (max_height_num == 3) {
                  hardware.info.yellow_goal_dir = hardware.info.Cam[3].yellow_goal_dir + 225;
            }
            hardware.info.yellow_goal_dir = MyMath::NormalizeDeg180(hardware.info.yellow_goal_dir);

            pre_yellow_goal_dir = hardware.info.yellow_goal_dir;
            pre_yellow_goal_dis = hardware.info.yellow_goal_dis;
      }
}

void BlueGoalConversion() {
      uint8_t max_height = 0;
      uint8_t max_height_num = 0;
      static uint8_t pre_blue_goal_dis, pre_blue_goal_dir;
      for (int i = 0; i < 4; i++) {  // ゴールの縦サイズが一番大きいカメラを特定
            if (hardware.info.Cam[i].blue_goal_dis > max_height) {
                  max_height = hardware.info.Cam[i].blue_goal_dis;
                  max_height_num = i;
            }
      }
      hardware.info.blue_goal_dis = max_height;

      if (hardware.info.blue_goal_dis != 0) {
            blue_goal_miss_timer.reset();

            if (hardware.info.blue_goal_dis == 0) {  // ゴールが見つからない
                  if (blue_goal_miss_timer.read_ms() > GOAL_MISS_TIME) {
                        hardware.info.blue_goal_dir = 0;
                        hardware.info.blue_goal_dis = 0;
                  } else {
                        hardware.info.blue_goal_dir = pre_blue_goal_dir;
                        hardware.info.blue_goal_dis = pre_blue_goal_dis;
                  }
            } else if (max_height_num == 0) {
                  hardware.info.blue_goal_dir = hardware.info.Cam[0].blue_goal_dir + 315;
            } else if (max_height_num == 1) {
                  hardware.info.blue_goal_dir = hardware.info.Cam[1].blue_goal_dir + 45;
            } else if (max_height_num == 2) {
                  hardware.info.blue_goal_dir = hardware.info.Cam[2].blue_goal_dir + 135;
            } else if (max_height_num == 3) {
                  hardware.info.blue_goal_dir = hardware.info.Cam[3].blue_goal_dir + 225;
            }
            hardware.info.blue_goal_dir = MyMath::NormalizeDeg180(hardware.info.blue_goal_dir);

            pre_blue_goal_dir = hardware.info.blue_goal_dir;
            pre_blue_goal_dis = hardware.info.blue_goal_dis;
      }
}

void main_app() {
      while (1) {
            process_timer.reset();
            hardware.MainUart();
            hardware.M1nUart();

            if (hardware.info.Cam[0].is_camera_initialized && hardware.info.Cam[1].is_camera_initialized && hardware.info.Cam[2].is_camera_initialized && hardware.info.Cam[3].is_camera_initialized) {
                  hardware.led2 = 1;
                  hardware.led1 = 0;
            }

            BallConversion();
            YellowGoalConversion();
            BlueGoalConversion();

            hardware.info.own_x = hardware.info.Cam[3].wall_dis - hardware.info.Cam[1].wall_dis;
            hardware.info.own_y = hardware.info.Cam[2].wall_dis - hardware.info.Cam[0].wall_dis;

            //  定周期処理
            process_time = process_timer.read_us();
            if (process_time < PERIOD_US) {
                  while (process_timer.read_us() < PERIOD_US);
            }
            process_freq = 1.0f / (process_timer.read_us() * 0.000001f);
      }
}