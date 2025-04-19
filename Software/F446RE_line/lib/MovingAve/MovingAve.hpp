#ifndef __MOVING_AVE__
#define __MOVING_AVE__

#include "main.h"

class MovingAve {
     public:
      void Compute(float input);
      void SetLength(uint8_t length_ = 10);
      double Get();
      void Reset();

     private:
      double data_[100];
      double result_;
      uint8_t cnt_;
      uint8_t length_;
      double length_product_;
};
#endif