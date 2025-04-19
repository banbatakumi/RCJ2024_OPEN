#ifndef __MOTORDRIVE__
#define __MOTORDRIVE__

#include "MovingAve.hpp"
#include "MyMath.hpp"
#include "PWMSingle.hpp"
#include "Timer.hpp"
#include "config.h"
#include "pid.hpp"

#define MAX_POWER 1000  // MAX:100
#define MIN_POWER 1
#define MOVING_AVE_NUM 5

#define MAX_ROTATION_SPEED TWO_PI

#define MOTOR_QTY 4

#define CHECK_SPEED 100  // ms

#define WHEEL_R 0.025
#define ROBOT_R 0.085

class MotorDrive {
     public:
      MotorDrive(PwmSingleOut *motor1a, PwmSingleOut *motor1b,
                 PwmSingleOut *motor2a, PwmSingleOut *motor2b,
                 PwmSingleOut *motor3a, PwmSingleOut *motor3b,
                 PwmSingleOut *motor4a, PwmSingleOut *motor4b,
                 int16_t *yaw, float *motor_abs_rad_s_);

#define CENTER 0
#define FRONT 1
#define RIGHT 2
#define BACK 3
#define LEFT 4
      void Drive(int16_t target_move_dir = 0, float target_move_speed = 0, float move_acce = 0, int16_t rotation_dir = 0, float rotation_speed = 0, uint8_t rotation_center = CENTER);
      void Run(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
      void Brake(uint16_t time = 0);
      void Free();
      void Init();
      void CheckConnection();
      float moving_vel_x, moving_vel_y, moving_speed, moving_dir;

     private:
      PwmSingleOut *motor1a_;
      PwmSingleOut *motor1b_;
      PwmSingleOut *motor2a_;
      PwmSingleOut *motor2b_;
      PwmSingleOut *motor3a_;
      PwmSingleOut *motor3b_;
      PwmSingleOut *motor4a_;
      PwmSingleOut *motor4b_;

      MovingAve motor_ave[MOTOR_QTY];

      PID motor_pid[MOTOR_QTY];

      PID pid;
      int16_t *yaw_;
      float *motor_abs_rad_s_;
      float motor_rad_s_[MOTOR_QTY];

      Timer dt_timer;
};

#endif