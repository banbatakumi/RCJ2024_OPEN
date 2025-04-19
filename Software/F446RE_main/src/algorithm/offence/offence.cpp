#include "capture_ball.hpp"
#include "mode.hpp"
#include "on_line.hpp"

// ボール保持中
static Timer do_back_curve_shoot_timer;
static Timer do_front_curve_shoot_timer;
static Timer hold_timer;

static bool do_back_curve_shoot;

static bool do_front_curve_shoot;

static void HoldBallFront(Robot* robot) {
      if (do_front_curve_shoot == true) {
            if (robot->info.Catch.is_front == true) hold_timer.reset();
            if (hold_timer.read_ms() > 500) do_front_curve_shoot = false;
            if (do_front_curve_shoot_timer.read_ms() < 500) {
                  robot->motor.Drive(0, 0);
            } else {
                  if (abs(robot->info.Cam.ops_goal_dir) < 60 || robot->info.Cam.is_goal_front == true) {
                        do_front_curve_shoot = false;
                        robot->motor.Brake(200);
                        robot->dribbler_front.Brake();
                        robot->kicker.Kick();
                  }

                  int16_t rotation_dir;
                  rotation_dir = robot->info.Cam.ops_goal_dir;
                  if (abs(rotation_dir) > 45) rotation_dir = 45 * (abs(robot->info.Cam.ops_goal_dir) / robot->info.Cam.ops_goal_dir);
                  robot->motor.Drive(180 - robot->info.Imu.yaw, 0.5, 0.5, rotation_dir, PI * 0.25, FRONT);
            }
      } else if (abs(robot->info.Cam.ops_goal_dir) > 60) {
            do_front_curve_shoot = true;
            do_front_curve_shoot_timer.reset();
      } else {
            if (robot->info.Cam.is_goal_front == true) {
                  robot->dribbler_front.Brake();
                  robot->kicker.Kick();
            }
            robot->motor.Drive(0, robot->info.target_move_speed, 1, robot->info.Cam.ops_goal_dir, PI, FRONT);
      }
}

static void HoldBallBack(Robot* robot) {
      static int16_t shoot_dir;
      static bool enable_shoot;
      if (robot->info.Catch.is_back == true) hold_timer.reset();
      if (hold_timer.read_ms() > 500) do_back_curve_shoot = 0;
      if (do_back_curve_shoot_timer.read_ms() < 500) {  // 蹴る条件
            robot->motor.Drive(0, 0);
            shoot_dir = 45 * (abs(robot->info.Cam.ops_goal_dir) / robot->info.Cam.ops_goal_dir);
            enable_shoot = false;
      } else {
            if (enable_shoot == true) {
                  if (abs(robot->info.Imu.yaw) > 90) {
                        robot->motor.Brake(500);
                        do_back_curve_shoot = false;
                  } else {
                        int16_t shoot_power = -1000 * (abs(shoot_dir) / shoot_dir);
                        robot->motor.Run(shoot_power, shoot_power, shoot_power, shoot_power);
                  }
            } else if (abs(robot->info.Imu.yaw) > abs(shoot_dir)) {
                  enable_shoot = true;
            } else {
                  robot->motor.Drive(0, 0, 0, -90 * (abs(shoot_dir) / shoot_dir), PI * 0.5, BACK);
            }
      }
}

void Mode::Offence() {
      if (robot->info.Catch.is_front) {
            robot->dribbler_front.Hold(HOLD_MAX_POWER);
      } else if (BALL_NEAR_FRONT) {
            robot->dribbler_front.Hold(HOLD_WAIT_POWER);
      } else {
            robot->dribbler_front.Hold(0);
      }

      if (robot->info.Catch.is_back) {
            robot->dribbler_back.Hold(HOLD_MAX_POWER);
      } else if (BALL_NEAR_BACK) {
            robot->dribbler_back.Hold(HOLD_WAIT_POWER);
      } else {
            robot->dribbler_back.Hold(0);
      }
      // robot->info.Line.is_leftside || robot->info.Line.is_rightside ||

      if (robot->info.Line.is_on_line || back_to_inside_timer.read_ms() < (pre_speed * 1000)) {
            OnLine(robot);
      } else {
            is_pre_on_line = false;
            // is_pre_leftside = false;
            // is_pre_rightside = false;

            if (robot->info.Catch.is_front == true || do_front_curve_shoot == true) {
                  HoldBallFront(robot);
            } else if (robot->info.Catch.is_back == true || do_back_curve_shoot == true) {
                  if (do_back_curve_shoot == false) do_back_curve_shoot_timer.reset();
                  do_back_curve_shoot = true;

                  HoldBallBack(robot);
            } else {
                  if (abs(robot->info.Cam.ball_dir) < 120) {
                        CaptureBall(robot, 0);
                  } else {
                        CaptureBall(robot, 180);
                  }
            }
      }
}