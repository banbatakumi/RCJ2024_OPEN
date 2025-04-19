#include "app.h"

#include "mode.hpp"
#include "robot.hpp"

Robot robot;
Mode mode(&robot);

void TimInterrupt1khz() {
      robot.heartBeat();
      robot.kicker.Control();
}

void setup() {
      robot.HardwareInit();
}

void main_app() {
      while (1) {
            mode.MainMode();
      }
}