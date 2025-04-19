#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "moving_ave.h"

#define MOVING_AVE_NUM 10
#define PWM_FREQ 60000
#define PWM_RESOLUTION 8

class Motor {
     public:
      Motor(uint8_t motor_a_, uint8_t motor_b_);
      void Run(int16_t power_);

     private:
      MovingAve motorAve;
};

#endif