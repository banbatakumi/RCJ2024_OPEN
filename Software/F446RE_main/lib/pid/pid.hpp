#ifndef __PID__
#define __PID__

#include "MyMath.hpp"
#include "Timer.hpp"

class PID {
     public:
      PID();

#define PID_TYPE 0
#define PI_D_TYPE 1

      void SetGain(float kp, float ki, float kd);
      void SetSamplingFreq(uint16_t sampling_freq = 100);
      void SetLimit(double min_limit = -100, double max_limit = 100);

      void Compute(float input, float target);
      float Get();
      void ResetI();

      void SetType(uint8_t type = 0);

     private:
      double p_, i_, d_;
      double pre_p_;
      double kp_, ki_, kd_;
      double pid_;
      double pre_input_;

      double sampling_period_;
      double max_limit_;
      double min_limit_;

      uint8_t type_;

      Timer sampling_timer;
};

#endif