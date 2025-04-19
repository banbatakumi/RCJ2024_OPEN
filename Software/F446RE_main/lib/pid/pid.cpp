#include "pid.hpp"

PID::PID() {
      type_ = 0;
}

void PID::SetGain(float kp, float ki, float kd) {
      this->kp_ = kp;
      this->ki_ = ki;
      this->kd_ = kd;
}

void PID::SetSamplingFreq(uint16_t sampling_freq) {
      this->sampling_period_ = 1.0f / sampling_freq;
}

void PID::SetLimit(double min_limit, double max_limit) {
      this->min_limit_ = min_limit;
      this->max_limit_ = max_limit;
}

void PID::SetType(uint8_t type) {
      type_ = type;
}

void PID::Compute(float input, float target) {
      if (type_ == 0) {
            p_ = target - input;  // 比例
            if (sampling_timer.read() > sampling_period_) {
                  d_ = (p_ - pre_p_) / sampling_timer.read();          // 微分
                  i_ += (p_ + pre_p_) * sampling_timer.read() * 0.5f;  // 台形積分
                  if (i_ > max_limit_) i_ = max_limit_;
                  if (i_ < min_limit_) i_ = min_limit_;
                  pre_p_ = p_;
                  sampling_timer.reset();
            }

            pid_ = p_ * kp_ + i_ * ki_ + d_ * kd_;
      } else if (type_ == 1) {
            p_ = target - input;  // 比例
            if (sampling_timer.read() > sampling_period_) {
                  d_ = (input - pre_input_) / sampling_timer.read();   // 微分
                  i_ += (p_ + pre_p_) * sampling_timer.read() * 0.5f;  // 台形積分
                  if (i_ > max_limit_) i_ = max_limit_;
                  if (i_ < min_limit_) i_ = min_limit_;
                  pre_p_ = p_;
                  pre_input_ = input;
                  sampling_timer.reset();
            }

            pid_ = p_ * kp_ + i_ * ki_ - d_ * kd_;
      } else if (type_ == 2) {
            p_ = target - input;  // 比例
            if (sampling_timer.read() > sampling_period_) {
                  d_ = (input - pre_input_) / sampling_timer.read();   // 微分
                  i_ += (p_ + pre_p_) * sampling_timer.read() * 0.5f;  // 台形積分
                  if (i_ > max_limit_) i_ = max_limit_;
                  if (i_ < min_limit_) i_ = min_limit_;
                  pre_p_ = p_;
                  p_ = input - pre_input_;
                  pre_input_ = input;
                  sampling_timer.reset();
            }

            pid_ = 0 - p_ * kp_ + i_ * ki_ - d_ * kd_;
      }
      if (pid_ > max_limit_) pid_ = max_limit_;
      if (pid_ < min_limit_) pid_ = min_limit_;
}

float PID::Get() {
      return pid_;
}

void PID::ResetI() {
      i_ = 0;
}