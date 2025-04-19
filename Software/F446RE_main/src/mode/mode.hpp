#ifndef MODE_HPP  // インクルードガードの開始
#define MODE_HPP
#include "robot.hpp"

#define BALL_NEAR_FRONT abs(robot->info.Cam.ball_dir) < 30 && robot->info.Cam.ball_dis < 20
#define BALL_NEAR_BACK abs(robot->info.Cam.ball_dir) > 150 && robot->info.Cam.ball_dis < 20
class Mode {
     public:
      Mode(Robot* robotPtr) : robot(robotPtr) {}
      void MainMode();
      void Offence();
      void Defence();

     private:
      Timer process_timer;
      Robot* robot;

      uint16_t process_time_;  // us
};

#endif  // インクルードガードの終了