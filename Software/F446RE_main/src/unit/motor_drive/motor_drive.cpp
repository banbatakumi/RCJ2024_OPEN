#include "motor_drive.hpp"

MotorDrive::MotorDrive(PwmSingleOut *motor1a, PwmSingleOut *motor1b,
                       PwmSingleOut *motor2a, PwmSingleOut *motor2b,
                       PwmSingleOut *motor3a, PwmSingleOut *motor3b,
                       PwmSingleOut *motor4a, PwmSingleOut *motor4b,
                       int16_t *yaw, float *motor_abs_rad_s)
    : motor1a_(motor1a), motor1b_(motor1b), motor2a_(motor2a), motor2b_(motor2b), motor3a_(motor3a), motor3b_(motor3b), motor4a_(motor4a), motor4b_(motor4b) {
      this->yaw_ = yaw;
      this->motor_abs_rad_s_ = motor_abs_rad_s;
      for (uint8_t i = 0; i < MOTOR_QTY; i++) {
            motor_ave[i].SetLength(MOVING_AVE_NUM);
            motor_pid[i].SetGain(50, 150, 0.25);
            motor_pid[i].SetLimit(MIN_POWER, MAX_POWER);
            motor_pid[i].SetSamplingFreq(500);
            motor_pid[i].SetType(1);
      }

      pid.SetGain(0.1, 0, 0.01);
      pid.SetLimit(-MAX_ROTATION_SPEED, MAX_ROTATION_SPEED);
      pid.SetSamplingFreq(500);
      pid.SetType(0);
}

void MotorDrive::Init() {
      motor1a_->init();
      motor1b_->init();
      motor2a_->init();
      motor2b_->init();
      motor3a_->init();
      motor3b_->init();
      motor4a_->init();
      motor4b_->init();
}

void MotorDrive::Drive(int16_t target_move_dir, float target_move_speed, float move_acce, int16_t rotation_dir, float rotation_speed, uint8_t rotation_center) {
      float target_rad_s[MOTOR_QTY];
      static float pre_target_rad_s[MOTOR_QTY];

      int16_t power[MOTOR_QTY];
      static float move_speed = 0;

      double dt = dt_timer.read();
      dt_timer.reset();

      if (dt > 0.1) {
            move_speed = 0;
            for (uint8_t i = 0; i < MOTOR_QTY; i++) motor_pid[i].ResetI();
      }
      double speed_change = move_acce * dt;
      if (move_acce == 0) {
            move_speed = target_move_speed;
      } else if (target_move_speed > move_speed) {
            move_speed += speed_change;
            if (move_speed > target_move_speed) move_speed = target_move_speed;
      } else if (target_move_speed < move_speed) {
            move_speed -= speed_change;
            if (move_speed < target_move_speed) move_speed = target_move_speed;
      }

      float vel_x = move_speed * MyMath::cosDeg(target_move_dir);
      float vel_y = move_speed * MyMath::sinDeg(target_move_dir);

      pid.Compute(*yaw_, rotation_dir);  // 姿勢制御用PID
      for (uint8_t i = 0; i < MOTOR_QTY; i++) {
            float robot_rotation_speed = pid.Get();
            if (abs(robot_rotation_speed) > rotation_speed && rotation_speed != 0) robot_rotation_speed = rotation_speed * (abs(robot_rotation_speed) / robot_rotation_speed);
            if (rotation_center == FRONT) {
                  robot_rotation_speed *= 2;
                  if (i == 0 || i == 3) robot_rotation_speed = 0;
            } else if (rotation_center == RIGHT) {
                  robot_rotation_speed *= 2;
                  if (i == 0 || i == 1) robot_rotation_speed = 0;
            } else if (rotation_center == BACK) {
                  robot_rotation_speed *= 2;
                  if (i == 1 || i == 2) robot_rotation_speed = 0;
            } else if (rotation_center == LEFT) {
                  robot_rotation_speed *= 2;
                  if (i == 2 || i == 3) robot_rotation_speed = 0;
            }
            target_rad_s[i] = (1.0f / WHEEL_R) * ((-1 * MyMath::sinDeg(45 + (90 * i)) * MyMath::cosDeg(45) * vel_x) + (MyMath::cosDeg(45 + (90 * i)) * MyMath::cosDeg(45) * vel_y) + ROBOT_R * robot_rotation_speed);  // 各ホイールの目標角速度
      }

      // 速度制御
      for (uint8_t i = 0; i < MOTOR_QTY; i++) {
            if (target_rad_s[i] == 0 || ((pre_target_rad_s[i] / abs(pre_target_rad_s[i])) != (target_rad_s[i] / abs(target_rad_s[i])))) motor_pid[i].ResetI();
            pre_target_rad_s[i] = target_rad_s[i];
            motor_pid[i].Compute(motor_abs_rad_s_[i], abs(target_rad_s[i]));
            if (target_rad_s[i] > 0) {
                  power[i] = motor_pid[i].Get();
                  motor_rad_s_[i] = motor_abs_rad_s_[i];
            } else {
                  power[i] = -motor_pid[i].Get();
                  motor_rad_s_[i] = -motor_abs_rad_s_[i];
            }
      }

      // 移動平均
      for (uint8_t i = 0; i < MOTOR_QTY; i++) {
            motor_ave[i].Compute(power[i]);
            power[i] = motor_ave[i].Get();
      }

      moving_vel_x = (WHEEL_R / 2.0f) * (motor_rad_s_[0] - motor_rad_s_[1] - motor_rad_s_[2] + motor_rad_s_[3]);
      moving_vel_y = (WHEEL_R / 2.0f) * (-motor_rad_s_[0] - motor_rad_s_[1] + motor_rad_s_[2] + motor_rad_s_[3]);
      moving_speed = MyMath::sqrt(moving_vel_x * moving_vel_x + moving_vel_y * moving_vel_y);
      moving_dir = MyMath::atan2(moving_vel_x, moving_vel_y);

      Run(power[0], power[1], power[2], power[3]);
}

void MotorDrive::Run(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4) {
      // Init();
      if (abs(motor1) > 1000) motor1 = motor1 * (abs(motor1) / motor1);
      if (abs(motor2) > 1000) motor2 = motor2 * (abs(motor2) / motor2);
      if (abs(motor3) > 1000) motor3 = motor3 * (abs(motor3) / motor3);
      if (abs(motor4) > 1000) motor4 = motor4 * (abs(motor4) / motor4);
      if (motor1 > 0) {
            motor1a_->write(motor1 * 0.001f);
            motor1b_->write(0);
      } else {
            motor1a_->write(0);
            motor1b_->write(motor1 * -0.001f);
      }
      if (motor2 > 0) {
            motor2a_->write(motor2 * 0.001f);
            motor2b_->write(0);
      } else {
            motor2a_->write(0);
            motor2b_->write(motor2 * -0.001f);
      }
      if (motor3 > 0) {
            motor3a_->write(motor3 * 0.001f);
            motor3b_->write(0);
      } else {
            motor3a_->write(0);
            motor3b_->write(motor3 * -0.001f);
      }
      if (motor4 > 0) {
            motor4a_->write(motor4 * 0.001f);
            motor4b_->write(0);
      } else {
            motor4a_->write(0);
            motor4b_->write(motor4 * -0.001f);
      }
}

void MotorDrive::Brake(uint16_t time) {
      motor1a_->write(1);
      motor1b_->write(1);
      motor2a_->write(1);
      motor2b_->write(1);
      motor3a_->write(1);
      motor3b_->write(1);
      motor4a_->write(1);
      motor4b_->write(1);
      HAL_Delay(time);
}

void MotorDrive::Free() {
      motor1a_->write(0);
      motor1b_->write(0);
      motor2a_->write(0);
      motor2b_->write(0);
      motor3a_->write(0);
      motor3b_->write(0);
      motor4a_->write(0);
      motor4b_->write(0);
}

void MotorDrive::CheckConnection() {
      // 接続チェック
      motor1a_->sound(700, CHECK_SPEED);
      motor2a_->sound(800, CHECK_SPEED);
      motor3a_->sound(900, CHECK_SPEED);
      motor4a_->sound(1000, CHECK_SPEED);
      motor1b_->sound(700, CHECK_SPEED);
      motor2b_->sound(800, CHECK_SPEED);
      motor3b_->sound(900, CHECK_SPEED);
      motor4b_->sound(1000, CHECK_SPEED);
}