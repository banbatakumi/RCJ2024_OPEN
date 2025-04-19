#ifndef PID_H
#define PID_H

#include "Arduino.h"
#include "simplify_deg.h"

class PID {
     public:
      PID();

#define PID_TYPE 0
#define PI_D_TYPE 1
#define I_PD_TYPE 2

      void SetGain(float kp_, float ki_, float kd_);
      void ResetPID();
      void SelectType(uint8_t type_ = PID_TYPE);

      void Compute(float input_, float target_);
      float Get();
      void SetILimit(float i_limit_ = 100);

     private:
      float p, i, d;
      float pre_p;
      float kp, ki, kd;
      float pid;
      float pre_input;
      float i_limit;

      uint64_t pre_time;
      float dt;

      uint8_t type;
};

#endif