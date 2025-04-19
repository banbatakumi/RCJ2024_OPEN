#include "encoder.hpp"

Encoder::Encoder(DigitalOut *led_line, uint8_t *photo_val) : led_line_(led_line) {
      this->photo_val_ = photo_val;
      for (uint8_t i = 0; i < SENSOR_QTY; i++) ave[i].SetLength(3);
}

void Encoder::Read() {
      // 閾値の自動調整
      for (uint8_t i = 0; i < SENSOR_QTY; i++) {
            if (max_val_[i] < photo_val_[i]) max_val_[i] = photo_val_[i];
            if (min_val_[i] > photo_val_[i]) min_val_[i] = photo_val_[i];
            threshold_[i] = (max_val_[i] + min_val_[i]) * 0.5;
      }

      for (uint8_t i = 0; i < SENSOR_QTY; i++) {
            is_white_[i] = 0;
            if (photo_val_[i] > threshold_[i]) is_white_[i] = 1;
            if (is_white_[i] != pre_is_white_[i]) {
                  interval_time_s_[i] = interval_timer[i].read();

                  pre_interval_time_s_[i] = interval_time_s_[i];
                  interval_timer[i].reset();
            }

            if (interval_timer[i].read() > MAX_INTERVAL) interval_time_s_[i] = 0;

            if (interval_time_s_[i] > MIN_INTERVAL) {
                  ave[i].Compute(interval_time_s_[i]);
                  interval_time_s_[i] = ave[i].Get();
                  rps_[i] = 1.0f / (interval_time_s_[i] * 4);
            }

            if (interval_time_s_[i] == 0) rps_[i] = 0;

            pre_is_white_[i] = is_white_[i];
      }
}

uint8_t Encoder::GetMotorRPS(uint8_t encoder_num) {
      return rps_[encoder_num];
}