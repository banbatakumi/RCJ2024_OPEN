#include "line.hpp"

Line::Line(DigitalOut *led_line,
           DigitalOut *mux1a, DigitalOut *mux1b,
           DigitalOut *mux2a, DigitalOut *mux2b,
           DigitalOut *mux3a, DigitalOut *mux3b,
           uint8_t *leftside_val, uint8_t *rightside_val,
           uint8_t *mux1x_val, uint8_t *mux1y_val,
           uint8_t *mux2x_val, uint8_t *mux2y_val,
           uint8_t *mux3x_val, uint8_t *mux3y_val)
    : led_line_(led_line), mux1a_(mux1a), mux1b_(mux1b), mux2a_(mux2a), mux2b_(mux2b), mux3a_(mux3a), mux3b_(mux3b) {
      this->leftside_val_ = leftside_val;
      this->rightside_val_ = rightside_val;
      this->mux1x_val_ = mux1x_val;
      this->mux1y_val_ = mux1y_val;
      this->mux2x_val_ = mux2x_val;
      this->mux2y_val_ = mux2y_val;
      this->mux3x_val_ = mux3x_val;
      this->mux3y_val_ = mux3y_val;
}

void Line::Read() {
      // アナログ値の取得
      if (read_sensors_group_ == 0 && read_interval_timer.read_us() > READ_INTERVAL_TIME_US) {
            if (*leftside_val_ > leftside_th_) {
                  is_leftside_white = 1;
            } else {
                  is_leftside_white = 0;
            }
            if (*rightside_val_ > rightside_th_) {
                  is_rightside_white = 1;
            } else {
                  is_rightside_white = 0;
            }
            read_sensors_group_++;
            read_interval_timer.reset();
      } else if (read_sensors_group_ == 1 && read_interval_timer.read_us() > READ_INTERVAL_TIME_US) {
            val_[22] = *mux1x_val_;
            val_[19] = *mux1y_val_;
            val_[15] = *mux2x_val_;
            val_[11] = *mux2y_val_;
            val_[2] = *mux3x_val_;
            val_[5] = *mux3y_val_;
            mux1a_->write(1);
            mux1b_->write(0);
            mux2a_->write(1);
            mux2b_->write(0);
            mux3a_->write(1);
            mux3b_->write(0);
            read_sensors_group_++;
            read_interval_timer.reset();
      } else if (read_sensors_group_ == 2 && read_interval_timer.read_us() > READ_INTERVAL_TIME_US) {
            val_[21] = *mux1x_val_;
            val_[16] = *mux1y_val_;
            val_[14] = *mux2x_val_;
            val_[8] = *mux2y_val_;
            val_[3] = *mux3x_val_;
            val_[0] = *mux3y_val_;
            mux1a_->write(0);
            mux1b_->write(1);
            mux2a_->write(0);
            mux2b_->write(1);
            mux3a_->write(0);
            mux3b_->write(1);
            read_sensors_group_++;
            read_interval_timer.reset();
      } else if (read_sensors_group_ == 3 && read_interval_timer.read_us() > READ_INTERVAL_TIME_US) {
            val_[20] = *mux1x_val_;
            val_[18] = *mux1y_val_;
            val_[13] = *mux2x_val_;
            val_[10] = *mux2y_val_;
            val_[6] = *mux3x_val_;
            val_[4] = *mux3y_val_;
            mux1a_->write(1);
            mux1b_->write(1);
            mux2a_->write(1);
            mux2b_->write(1);
            mux3a_->write(1);
            mux3b_->write(1);
            read_sensors_group_++;
            read_interval_timer.reset();
      } else if (read_sensors_group_ == 4 && read_interval_timer.read_us() > READ_INTERVAL_TIME_US) {
            val_[23] = *mux1x_val_;
            val_[17] = *mux1y_val_;
            val_[12] = *mux2x_val_;
            val_[9] = *mux2y_val_;
            val_[7] = *mux3x_val_;
            val_[1] = *mux3y_val_;
            mux1a_->write(0);
            mux1b_->write(0);
            mux2a_->write(0);
            mux2b_->write(0);
            mux3a_->write(0);
            mux3b_->write(0);
            read_sensors_group_ = 0;
            read_interval_timer.reset();
      }

      // それぞれのセンサが反応しているかの判定
      for (uint16_t i = 0; i < LINE_QTY; i++) {
            val_[i] = (val_[i] + pre_val_[i]) * 0.5;
            pre_val_[i] = val_[i];
            if (val_[i] > th_val_[i]) {
                  is_white_[i] = 1;
            } else {
                  is_white_[i] = 0;
            }
      }
}

bool Line::SetTh() {
      if (set_th_cnt_ == 0) {
            OnLed();
            HAL_Delay(100);
            leftside_th_ = 0;
            rightside_th_ = 0;
            for (uint16_t i = 0; i < LINE_QTY; i++) th_val_[i] = 0;
      }
      set_th_cnt_++;

      Read();
      leftside_th_ += *leftside_val_;
      rightside_th_ += *rightside_val_;

      for (uint16_t j = 0; j < LINE_QTY; j++) {
            th_val_[j] += val_[j];
      }

      if (set_th_cnt_ == SET_TH_NUM) {
            leftside_th_ /= SET_TH_NUM;
            rightside_th_ /= SET_TH_NUM;
            leftside_th_ += TH_VALUE;
            rightside_th_ += TH_VALUE;
            for (uint16_t i = 0; i < LINE_QTY; i++) {
                  th_val_[i] /= SET_TH_NUM;
                  th_val_[i] += TH_VALUE;
            }

            OffLed();
            set_th_cnt_ = 0;
            return 1;
      }
      return 0;
}

void Line::Compute() {
      // 反応してるライン
      white_qty_ = 0;
      for (uint8_t i = 0; i < LINE_QTY; i++) {
            white_qty_ += is_white_[i];
      }

      if (white_qty_ != 0) {
            // ラインの間隔
            uint8_t pos_white_num[white_qty_];
            uint8_t white_num = 0;

            for (uint8_t i = 0; i < LINE_QTY; i++) {
                  if (is_white_[i]) {
                        pos_white_num[white_num] = i;
                        white_num++;
                  }
            }

            uint8_t interval[white_qty_];
            for (uint8_t i = 0; i < white_qty_ - 1; i++) {
                  interval[i] = pos_white_num[i + 1] - pos_white_num[i];
            }
            interval[white_qty_ - 1] = pos_white_num[0] - pos_white_num[white_qty_ - 1] + LINE_QTY;

            // ラインの最大の間隔
            max_interval = 0;
            uint8_t pos_max_interval = 0;
            for (uint8_t i = 0; i < white_qty_; i++) {
                  if (max_interval < interval[i]) {
                        max_interval = interval[i];
                        pos_max_interval = i;
                  }
            }

            // ラインの方向
            float line_dir_num = pos_white_num[pos_max_interval] + max_interval * 0.5;
            dir = MyMath::NormalizeDeg180((line_dir_num / LINE_QTY * 360) + 180);

            if (max_interval > 12) max_interval = 24 - max_interval;  // 間隔を１２までにする

            // ラインから戻る方向
            if (pre_white_qty_ != 0) {
                  int16_t dir_difference = abs(dir - pre_dir_);
                  if (dir_difference > 180) dir_difference = 360 - dir_difference;
                  if (dir_difference > 120) is_half_out = 1 - is_half_out;
            }

            pre_dir_ = dir;
            pre_white_qty_ = white_qty_;
      } else {
            is_half_out = 0;
            max_interval = 0;
            dir = 0;

            pre_dir_ = 0;
            pre_white_qty_ = 0;
      }
}

bool Line::IsOnLine() {
      bool is_on_line = 0;
      if (white_qty_ > 0) is_on_line = 1;
      return is_on_line;
}

void Line::OnLed() {
      led_line_->write(0);
}

void Line::OffLed() {
      led_line_->write(1);
}