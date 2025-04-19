#include "dribbler_drive.hpp"

DribblerDrive::DribblerDrive(PwmSingleOut *motor_a, PwmSingleOut *motor_b)
    : motor_a_(motor_a), motor_b_(motor_b) {
}

void DribblerDrive::Init() {
      motor_a_->init();
      motor_b_->init();
}

void DribblerDrive::Hold(uint8_t power) {
      double dt = dt_timer.read();
      dt_timer.reset();
      if (dt < 0.1) {
            if (power > power_) {
                  power_ += (power - power_) * dt * 10;
            } else {
                  power_ -= (power_ - power) * dt * 10;
            }
      }
      motor_a_->write(power_ * 0.01f);
      motor_b_->write(0);
}

void DribblerDrive::Kick() {
      motor_a_->write(0);
      motor_b_->write(1);
      power_ = 0;
}

void DribblerDrive::Brake(uint16_t time) {
      motor_a_->write(1);
      motor_b_->write(1);
      power_ = 0;
      HAL_Delay(time);
}

void DribblerDrive::CheckConnection() {
      // 接続チェック
      motor_a_->sound(1000, CHECK_SPEED);
      motor_b_->sound(1500, CHECK_SPEED);
}