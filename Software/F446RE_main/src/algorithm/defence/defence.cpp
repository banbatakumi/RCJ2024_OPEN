#include "capture_ball.hpp"
#include "mode.hpp"

#define SHOOT_TIME 1000
#define SHOOT_WAIT_TIME 1500

static PID ball_pid;

static Timer shoot_wait_timer;
static Timer shoot_timer;

static Timer out_line_timer;

static int16_t pre_line_dir;

static bool enable_shoot;

static bool is_pid_initialized = false;

static void InitializePID(Robot* robot) {
      ball_pid.SetGain(0.15, 0, 0.03);
      ball_pid.SetLimit(-robot->info.target_move_speed, robot->info.target_move_speed);
      ball_pid.SetSamplingFreq(100);
      ball_pid.SetType(0);
}

static void LineTrace(Robot* robot) {
      if (!is_pid_initialized) {
            InitializePID(robot);
            is_pid_initialized = true;
      }
      ball_pid.Compute(0, robot->info.Cam.ball_x);
      float move_speed = ball_pid.Get();

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
      if (robot->info.Cam.ball_dir < 0) {
            ball_almost_angle -= 90;
      } else {
            ball_almost_angle += 90;
      }

      vector_x = (12 - robot->info.Line.interval) * MyMath::sinDeg(robot->info.Line.dir) + robot->info.Line.interval * MyMath::sinDeg(ball_almost_angle);
      vector_y = (12 - robot->info.Line.interval) * MyMath::cosDeg(robot->info.Line.dir) + robot->info.Line.interval * MyMath::cosDeg(ball_almost_angle);

      if (abs(robot->info.Line.dir) > 60 && abs(robot->info.Line.dir) < 120) {
            robot->motor.Drive(0, 0.5, 1);
      } else if ((robot->info.Line.dir > 90 && robot->info.Line.dir < 135 && robot->info.Cam.ball_dir < 0) || (robot->info.Line.dir < -90 && robot->info.Line.dir > -135 && robot->info.Cam.ball_dir > 0) || robot->info.Cam.ball_dis > 50) {
            tmp_moving_speed = (12 - robot->info.Line.interval) * 0.1;
            if (tmp_moving_speed > robot->info.target_move_speed) tmp_moving_speed = robot->info.target_move_speed;
            robot->motor.Drive(robot->info.Line.dir, tmp_moving_speed, 1);
      } else {
            tmp_moving_dir = MyMath::NormalizeDeg180(MyMath::atan2(vector_x, vector_y));
            tmp_moving_speed = abs(move_speed);
            if (abs(robot->info.Cam.ball_dir) > 120) tmp_moving_speed = 0;
            if (tmp_moving_speed > robot->info.target_move_speed) tmp_moving_speed = robot->info.target_move_speed;
            robot->motor.Drive(tmp_moving_dir, tmp_moving_speed);
      }
}

void Mode::Defence() {
      if (enable_shoot == false) {
            if (!(abs(robot->info.Cam.ball_dir) < 15 && robot->info.Cam.ball_dis < 50 && robot->info.Line.is_on_line == 1)) {
                  shoot_wait_timer.reset();  // 打つ条件
                  enable_shoot = false;
            }
            if (shoot_wait_timer.read_ms() > SHOOT_WAIT_TIME) {
                  enable_shoot = true;
                  shoot_timer.reset();
            }
      }
      if (robot->info.Catch.is_front) {
            robot->dribbler_front.Hold(HOLD_MAX_POWER);
      } else if (BALL_NEAR_FRONT) {
            robot->dribbler_front.Hold(HOLD_WAIT_POWER);
      } else {
            robot->dribbler_front.Hold(0);
      }

      if (enable_shoot == true) {
            if (shoot_timer.read_ms() > SHOOT_TIME) {
                  enable_shoot = false;
                  robot->kicker.Kick();
                  shoot_wait_timer.reset();
                  shoot_timer.reset();
            }
            if (robot->info.Catch.is_front == true) {
                  robot->motor.Drive(0, robot->info.target_move_speed, 1, robot->info.Cam.ops_goal_dir, PI * 2, FRONT);
            } else {
                  CaptureBall(robot, 0);
            }

      } else if (robot->info.Line.is_on_line == 1) {
            LineTrace(robot);
            pre_line_dir = robot->info.Line.dir;
            out_line_timer.reset();
      } else {
            if (out_line_timer.read_ms() < SHOOT_TIME) {
                  robot->motor.Drive(pre_line_dir, 1, 1);
            } else {
                  robot->motor.Drive(robot->info.Cam.own_goal_dir, 0.5, 1);
            }
      }
}