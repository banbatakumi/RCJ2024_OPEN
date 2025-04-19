#ifndef __KICKER__
#define __KICKER__

#include "DigitalInOut.hpp"

#define KICK_TIME 0.2    // s
#define DEAD_TIME 0.002  // s
#define CHARGE_TIME 1    // s
#define FREQ 1000        // Hz
#define CHARGE_DUTY 0.1

class Kicker {
     public:
      Kicker(DigitalOut *charge, DigitalOut *kick);

      void Kick(float power = 1);
      void Charge();
      void Init();
      void Discharge();
      void Control();

     private:
      DigitalOut *charge_;
      DigitalOut *kick_;

      bool do_kick_;
      bool do_charge_;
      float kick_power_;

      uint16_t cnt;
};

#endif