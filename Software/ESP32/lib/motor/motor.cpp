
#include "motor.h"

Motor::Motor(uint8_t motor_a_, uint8_t motor_b_) {
      motorAve.SetLength(MOVING_AVE_NUM);

      pinMode(motor_a_, OUTPUT);               // PWM出力を行う端子を出力端子として設定
      ledcSetup(0, PWM_FREQ, PWM_RESOLUTION);  // PWM出力波形の初期設定
      ledcAttachPin(motor_a_, 0);              // チャンネルに対する出力端子を設定
      pinMode(motor_b_, OUTPUT);               // PWM出力を行う端子を出力端子として設定
      ledcSetup(1, PWM_FREQ, PWM_RESOLUTION);  // PWM出力波形の初期設定
      ledcAttachPin(motor_b_, 1);              // チャンネルに対する出力端子を設定
}

void Motor::Run(int16_t power_) {
      int16_t power = power_ * 2.56;
      motorAve.Compute(&power);

      if (power > 0) {
            ledcWrite(0, power);
            ledcWrite(1, 0);
      } else {
            ledcWrite(0, 0);
            ledcWrite(1, power * -1);
      }
}