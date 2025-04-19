#include "MovingAve.hpp"

void MovingAve::SetLength(uint8_t length) {
      this->length_ = length;
      length_product_ = 1.0f / length_;
}

void MovingAve::Compute(float input) {
      if (cnt_ >= length_) cnt_ = 0;
      data_[cnt_] = input;
      result_ = 0;
      for (uint8_t i = 0; i < length_; i++) {
            result_ += data_[i];
      }
      result_ *= (double)length_product_;
      cnt_++;
}

double MovingAve::Get() {
      return result_;
}

void MovingAve::Reset() {
      for (uint8_t i = 0; i < length_; i++) {
            data_[i] = 0;
      }
}