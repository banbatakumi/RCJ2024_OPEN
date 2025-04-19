#include "kicker.hpp"

Kicker::Kicker(DigitalOut *charge, DigitalOut *kick) : charge_(charge), kick_(kick) {
}

void Kicker::Init() {
      charge_ = 0;
      kick_ = 0;
}

void Kicker::Kick(float power) {
      if (do_charge_ == 0) do_kick_ = 1;
      kick_power_ = power;
      if (kick_power_ > 1) kick_power_ = 1;
      if (kick_power_ < 0) kick_power_ = 0;
}

void Kicker::Charge() {
      if (do_kick_ == 0) do_charge_ = 1;
}

void Kicker::Discharge() {
}

void Kicker::Control() {
      if (do_kick_ == 1) {
            cnt++;
            if (cnt < DEAD_TIME * FREQ) {
                  kick_->write(0);
                  charge_->write(0);
            } else if (cnt < ((DEAD_TIME + KICK_TIME) * FREQ)) {
                  if ((cnt % int(1 / kick_power_)) == 0) {
                        kick_->write(1);
                  } else {
                        kick_->write(0);
                  }
            } else {
                  do_kick_ = 0;
                  do_charge_ = 1;
                  cnt = 0;
            }
      } else if (do_charge_ == 1) {
            cnt++;
            if (cnt < DEAD_TIME * FREQ) {
                  kick_->write(0);
                  charge_->write(0);
            } else if (cnt < ((DEAD_TIME + CHARGE_TIME) * FREQ)) {
                  if ((cnt % int(1 / CHARGE_DUTY)) == 0) {
                        charge_->write(1);
                  } else {
                        charge_->write(0);
                  }
            } else {
                  charge_->write(1);
                  do_charge_ = 0;
                  cnt = 0;
            }
      } else {
            kick_->write(0);
      }
}