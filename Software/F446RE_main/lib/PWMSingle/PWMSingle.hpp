#ifndef __PWMSINGLE__
#define __PWMSINGLE__

#include "MyMath.hpp"
#include "tim.h"

#ifdef __cplusplus
extern "C" {

class PwmSingleOut {
     public:
      PwmSingleOut(TIM_HandleTypeDef *htim, uint32_t channel)
          : _htim(htim), _channel(channel), _maxValue(_htim->Init.Period) {
      }

      void init() {
            HAL_TIM_PWM_Start(_htim, _channel);
            _maxValue = _htim->Init.Period;
      }

      void write(float duty) {
            duty = (int)(Constrain(duty, 0.0, 1.0) * _maxValue);
            __HAL_TIM_SET_COMPARE(_htim, _channel, duty);
      }

      void operator=(float duty) {
            write(duty);
      }

      void sound(uint16_t period, uint16_t time) {
            // 周波数変更
            int default_prescaler = _htim->Init.Prescaler;
            int default_period = _htim->Init.Period;
            _htim->Init.Prescaler = 9;
            _htim->Init.Period = (SystemCoreClock / (period * (_htim->Init.Prescaler + 1))) - 1;
            if (HAL_TIM_Base_Init(_htim) != HAL_OK) {
                  Error_Handler();
            }

            float duty = (int)(0.1 * _htim->Init.Period);  // duty比を0.5にする

            // 出力
            __HAL_TIM_SET_COMPARE(_htim, _channel, duty);
            HAL_Delay(time);
            __HAL_TIM_SET_COMPARE(_htim, _channel, 0);
            _htim->Init.Prescaler = default_prescaler;
            _htim->Init.Period = default_period;
            if (HAL_TIM_Base_Init(_htim) != HAL_OK) {
                  Error_Handler();
            }
      }

     private:
      TIM_HandleTypeDef *_htim;
      uint32_t _channel;
      uint32_t _maxValue;

      bool _usePwmPin_t;
};
};

#endif
#endif