#include "on_line.hpp"

Timer back_to_inside_timer;

static int16_t pre_line_inside_dir;
float pre_speed;
bool is_pre_on_line;
// bool is_pre_leftside;
// bool is_pre_rightside;

static void LineTrace(Robot* robot) {
      float vector_x, vector_y;
      float vector_mag, vector_dir;
      float line_vector_gain, ball_vector_gain;
      float moving_speed;
      int16_t rotation_dir;

      if (abs(robot->info.Cam.ball_dir) < 90 && abs(robot->info.Line.inside_dir + robot->info.Imu.yaw) > 135) {  // ラインを割る
            line_vector_gain = 0.4;
            ball_vector_gain = 0.6;
      } else {  // 通常待機
            line_vector_gain = 0.6;
            ball_vector_gain = 0.4;
      }

      vector_x = robot->info.Line.depth * MyMath::sinDeg(robot->info.Line.inside_dir) * line_vector_gain;
      vector_y = robot->info.Line.depth * MyMath::cosDeg(robot->info.Line.inside_dir) * line_vector_gain;
      vector_x += ((24 - robot->info.Line.depth) * MyMath::sinDeg(robot->info.Cam.ball_dir)) * ball_vector_gain;
      vector_y += ((24 - robot->info.Line.depth) * MyMath::cosDeg(robot->info.Cam.ball_dir)) * ball_vector_gain;
      vector_dir = MyMath::atan2(vector_x, vector_y);
      vector_mag = abs(MyMath::sqrt(vector_x * vector_x + vector_y * vector_y)) * 0.1;
      if (vector_mag > robot->info.target_move_speed) vector_mag = robot->info.target_move_speed;

      if (abs(robot->info.Cam.ball_dir) < 90) {
            rotation_dir = robot->info.Cam.ball_dir;
      } else {
            rotation_dir = MyMath::NormalizeDeg180(robot->info.Cam.ball_dir + 180);
      }

      moving_speed = vector_mag * 0.4;
      if (moving_speed > 0.5) moving_speed = 0.5;
      robot->motor.Drive(vector_dir, moving_speed, 0, rotation_dir, PI);
}

void OnLine(Robot* robot) {
      if (robot->info.Catch.is_front == true || robot->info.Catch.is_back == true) {
            robot->motor.Drive(robot->info.Line.inside_dir, 0.5, 0.5, 0, PI * 0.5, FRONT);
      } else {
            if (back_to_inside_timer.read_ms() > (pre_speed * 1000) && is_pre_on_line == false) {
                  robot->motor.Brake(10);
                  back_to_inside_timer.reset();
                  is_pre_on_line = true;
                  pre_speed = robot->motor.moving_speed;
                  pre_line_inside_dir = robot->info.Line.inside_dir;
            }
            if (back_to_inside_timer.read_ms() < (pre_speed * BACK_TO_INSIDE_TIME)) {
                  robot->motor.Drive(pre_line_inside_dir, robot->info.target_line_move_speed, 20);
            } else {
                  if (MyMath::GapDeg(robot->info.Line.inside_dir, robot->info.Cam.ball_dir) < 90) {
                        robot->motor.Drive(robot->info.Line.inside_dir, robot->info.target_line_move_speed, 1);
                  } else {
                        LineTrace(robot);
                  }
            }
      }
      //  else if (robot->info.Line.is_leftside == true || (is_pre_leftside == true && back_to_inside_timer.read_ms() < BACK_TO_INSIDE_TIME)) {
      //       if (back_to_inside_timer.read_ms() > 500 && is_pre_leftside == false) {
      //             back_to_inside_timer.reset();
      //             is_pre_leftside = true;
      //       }
      //       if (back_to_inside_timer.read_ms() < BACK_TO_INSIDE_TIME) {
      //             robot->motor.Drive(90, robot->info.target_line_move_speed);
      //       } else {
      //             if (robot->info.Cam.ball_dir > 0) {
      //                   robot->motor.Drive(90, 1, 1);
      //             } else {
      //                   robot->motor.Drive(-90, 0.5, 0.5);
      //             }
      //       }
      // } else if (robot->info.Line.is_rightside == true || (is_pre_rightside == true && back_to_inside_timer.read_ms() < BACK_TO_INSIDE_TIME)) {
      //       if (back_to_inside_timer.read_ms() > 500 && is_pre_rightside == false) {
      //             back_to_inside_timer.reset();
      //             is_pre_rightside = true;
      //       }
      //       if (back_to_inside_timer.read_ms() < BACK_TO_INSIDE_TIME) {
      //             robot->motor.Drive(-90, robot->info.target_line_move_speed);
      //       } else {
      //             if (robot->info.Cam.ball_dir < 0) {
      //                   robot->motor.Drive(-90, 1, 1);
      //             } else {
      //                   robot->motor.Drive(90, 0.5, 0.5);
      //             }
      //       }
      // }
}