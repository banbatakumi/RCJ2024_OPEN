
#include "mode.hpp"

static Timer out_line_timer;

static int16_t pre_line_dir;

static bool enable_shoot;

static bool is_pid_initialized = false;

static void LineTrace(Robot* robot) {
      int16_t vector_x, vector_y;
      int16_t ball_almost_angle;
      int16_t tmp_moving_dir;
      float tmp_moving_speed;
      if (robot->info.Line.dir > -90 && robot->info.Line.dir < 90) {
            ball_almost_angle = robot->info.Line.dir;
      } else if (robot->info.Line.dir < -90) {
            ball_almost_angle = robot->info.Line.dir + 180;
      } else if (robot->info.Line.dir > 90) {
            ball_almost_angle = robot->info.Line.dir - 180;
      }
      if (robot->info.Esp32.ir_dir < 0) {
            ball_almost_angle -= 90;
      } else {
            ball_almost_angle += 90;
      }

      vector_x = (12 - robot->info.Line.interval) * MyMath::sinDeg(robot->info.Line.dir) + robot->info.Line.interval * MyMath::sinDeg(ball_almost_angle);
      vector_y = (12 - robot->info.Line.interval) * MyMath::cosDeg(robot->info.Line.dir) + robot->info.Line.interval * MyMath::cosDeg(ball_almost_angle);

      if (abs(robot->info.Line.dir) > 45 && abs(robot->info.Line.dir) < 135) {
            robot->motor.Drive(0, 0.5, 1);
      } else if ((robot->info.Line.dir > 90 && robot->info.Line.dir < 135 && robot->info.Esp32.ir_dir < 0) || (robot->info.Line.dir < -90 && robot->info.Line.dir > -135 && robot->info.Esp32.ir_dir > 0)) {
            tmp_moving_speed = (12 - robot->info.Line.interval) * 0.1;
            if (tmp_moving_speed > robot->info.target_move_speed) tmp_moving_speed = robot->info.target_move_speed;
            robot->motor.Drive(robot->info.Line.dir, tmp_moving_speed, 1);
      } else {
            tmp_moving_dir = MyMath::NormalizeDeg180(MyMath::atan2(vector_x, vector_y));
            tmp_moving_speed = abs(robot->info.Esp32.ir_dir) * 0.1;
            if (abs(robot->info.Esp32.ir_dir) > 120) tmp_moving_speed = 0;
            if (tmp_moving_speed > robot->info.target_move_speed) tmp_moving_speed = robot->info.target_move_speed;
            robot->motor.Drive(tmp_moving_dir, tmp_moving_speed);
      }
}

void Mode::LightDefence() {
      if (robot->info.Line.is_on_line == 1) {
            LineTrace(robot);
            pre_line_dir = robot->info.Line.dir;
            out_line_timer.reset();
      } else {
            if (out_line_timer.read_ms() < 1000) {
                  robot->motor.Drive(pre_line_dir, 1, 1);
            } else {
                  robot->motor.Drive(robot->info.Cam.own_goal_dir, 0.5, 1);
            }
      }
}