#include "capture_ball.hpp"

#define CAPTURE_R 20  // cm

static PID ball_capture_pid;
static PID ball_dis_capture_pid;

static bool is_pid_initialized = false;
static void InitializePID(Robot* robot) {
      ball_capture_pid.SetGain(0.2, 0, 0.02);
      ball_capture_pid.SetLimit(-2, 2);
      ball_capture_pid.SetSamplingFreq(100);
      ball_capture_pid.SetType(0);

      ball_dis_capture_pid.SetGain(0.05, 0, 0.01);
      ball_dis_capture_pid.SetLimit(-robot->info.target_move_speed, robot->info.target_move_speed);
      ball_dis_capture_pid.SetSamplingFreq(100);
      ball_dis_capture_pid.SetType(0);
}

void CaptureBall(Robot* robot, int16_t target_dir) {
      if (!is_pid_initialized) {
            InitializePID(robot);
            is_pid_initialized = true;
      }
      ball_capture_pid.Compute(0, robot->info.Cam.ball_x);
      ball_dis_capture_pid.Compute(CAPTURE_R, robot->info.Cam.ball_dis);

      int16_t ball_dir = MyMath::NormalizeDeg180(robot->info.Cam.ball_dir + target_dir);
      int16_t move_dir;
      float move_speed;
      float theta;

      float move_vel_x, move_vel_y;

      if (abs(robot->info.Cam.ball_x) <= 16 && abs(ball_dir) < 30) {
            if (abs(target_dir) < 90) {
                  move_vel_x = ball_capture_pid.Get();
                  move_vel_y = (MyMath::sqrt(16) - MyMath::sqrt(abs(robot->info.Cam.ball_x))) * 0.1;
            } else {
                  move_vel_x = ball_capture_pid.Get() * -1;
                  move_vel_y = (MyMath::sqrt(16) - MyMath::sqrt(abs(robot->info.Cam.ball_x))) * 0.1;
            }
            if (robot->info.Cam.ball_dis > CAPTURE_R) move_vel_y += ball_dis_capture_pid.Get();
            move_dir = MyMath::atan2(move_vel_x, move_vel_y);
      } else if (robot->info.Cam.ball_dis < CAPTURE_R) {
            theta = 90 + float(robot->info.Cam.ball_dis / CAPTURE_R) * 30;
            move_dir = ball_dir + (ball_dir > 0 ? theta : -theta);

            if (abs(target_dir) < 90) {
                  move_vel_x = 0.4 * MyMath::sinDeg(move_dir);
                  move_vel_y = 0.4 * MyMath::cosDeg(move_dir);
            } else {
                  move_vel_x = 0.4 * MyMath::sinDeg(move_dir);
                  move_vel_y = 0.4 * MyMath::cosDeg(move_dir);
            }
      } else {
            float ratio = (float)CAPTURE_R / robot->info.Cam.ball_dis;
            if (ratio > 1.0f) ratio = 1.0f;
            theta = asin(ratio) * 180 / PI;
            move_dir = ball_dir + (ball_dir > 0 ? theta : -theta);

            if (abs(target_dir) < 90) {
                  move_vel_x = 0.5 * MyMath::sinDeg(move_dir);
                  move_vel_y = 0.5 * MyMath::cosDeg(move_dir);

            } else {
                  move_vel_x = 0.5 * MyMath::sinDeg(move_dir);
                  move_vel_y = 0.5 * MyMath::cosDeg(move_dir);
            }
      }
      // 速度にボールの速度を加算
      float add_vel_x = robot->info.Cam.ball_vel_x + robot->motor.moving_vel_x;
      float add_vel_y = robot->info.Cam.ball_vel_y + robot->motor.moving_vel_y;
      if (abs(add_vel_x) > 0.5) add_vel_x = 0.5 * (add_vel_x / abs(add_vel_x));
      if (abs(add_vel_y) > 0.5) add_vel_y = 0.5 * (add_vel_y / abs(add_vel_y));
      move_vel_x += add_vel_x;
      move_vel_y += add_vel_y;
      move_speed = MyMath::sqrt(move_vel_x * move_vel_x + move_vel_y * move_vel_y);

      if (abs(target_dir) > 90 && move_speed > 0.75) move_speed = 0.75;
      if (move_speed > robot->info.target_move_speed) move_speed = robot->info.target_move_speed;
      if (move_speed < 0) move_speed = 0;

      robot->motor.Drive(MyMath::NormalizeDeg180(move_dir + target_dir), move_speed);
}
