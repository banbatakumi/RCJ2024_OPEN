#include "pid.h"

PID::PID() {
}

void PID::SetGain(float kp_, float ki_, float kd_) {
      this->kp = kp_;
      this->ki = ki_;
      this->kd = kd_;
}

void PID::SelectType(uint8_t type_) {
      this->type = type_;
}

void PID::Compute(float input_, float target_) {
      dt = micros() - pre_time;                         // 現在経過時間を取得
      if (type == 0) {                                  // 普通のやつ
            p = target_ - input_;                       // 比例
            i += (p + pre_p) * (dt * 0.000001f) * 0.5;  // 台形積分
            if (abs(i) > i_limit) i = i_limit * (i / abs(i));
            d = (p - pre_p) / (dt * 0.000001f);  // 微分
            pre_p = p;

            pid = p * kp + i * ki + d * kd;
      } else if (type == 1) {                           // 微分先行型
            p = target_ - input_;                       // 比例
            i += (p + pre_p) * (dt * 0.000001f) * 0.5;  // 台形積分
            if (abs(i) > i_limit) i = i_limit * (i / abs(i));
            d = (input_ - pre_input) / (dt * 0.000001f);  // 微分
            pre_p = p;
            pre_input = input_;

            pid = p * kp + i * ki + d * kd * -1;
      } else if (type == 2) {                            // 比例微分先行型
            p = target_ - input_;                        // 積分用の目標値との差
            i += (p + pre_p) * (dt / 1000000.0f) * 0.5;  // 台形積分
            if (abs(i) > i_limit) i = i_limit * (i / abs(i));
            d = (input_ - pre_input) / (dt / 1000000.0f);  // 微分
            pre_p = p;
            pre_input = input_;
            p = input_;  // 比例

            pid = p * kp * -1 + i * ki + d * kd * -1;
      }
      pre_time = micros();
}

void PID::ResetPID() {
      p = 0;
      i = 0;
      d = 0;
      pre_p = 0;
      pre_input = 0;
      pid = 0;
}

void PID::SetILimit(float i_limit_) {
      i_limit = i_limit_;
}

float PID::Get() {
      return pid;
}