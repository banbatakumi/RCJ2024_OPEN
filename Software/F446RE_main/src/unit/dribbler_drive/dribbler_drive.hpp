#ifndef __DRIBBLERDRIVE__
#define __DRIBBLERDRIVE__

#include "PWMSingle.hpp"
#include "Timer.hpp"

#define MAX_POWER 100  // MAX:100
#define MIN_POWER 5

#define CHECK_SPEED 100  // ms

class DribblerDrive {
     public:
      DribblerDrive(PwmSingleOut *motor_a, PwmSingleOut *motor_b);

      void Hold(uint8_t power);
      void Kick();
      void Brake(uint16_t time = 0);
      void Init();
      void CheckConnection();

     private:
      PwmSingleOut *motor_a_;
      PwmSingleOut *motor_b_;

      double power_;

      Timer dt_timer;
};

#endif