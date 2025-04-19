#ifndef __HARDWARE__
#define __HARDWARE__

#include "BufferedSerial.hpp"
#include "DigitalInOut.hpp"
#include "PWMSingle.hpp"
#include "Timer.hpp"
#include "config.h"
#include "main.h"

typedef struct {
      int16_t yaw;

      struct {
            uint8_t ball_dir;
            uint8_t ball_dis;
            uint8_t yellow_goal_dir;
            uint8_t yellow_goal_dis;
            uint8_t blue_goal_dir;
            uint8_t blue_goal_dis;
            uint8_t wall_dis;

            uint8_t proximity;
            bool is_goal_front;
            bool is_camera_initialized = 0;
      } Cam[4];

      int16_t ball_dir;
      int16_t ball_dis;
      int16_t yellow_goal_dir;
      int16_t yellow_goal_dis;
      int16_t blue_goal_dir;
      int16_t blue_goal_dis;
      int16_t own_x;
      int16_t own_y;

      bool is_goal_front;
} Info;

class Hardware {
     public:
      Hardware();

      // values
      Info info;

      PwmSingleOut led1 = PwmSingleOut(&htim2, TIM_CHANNEL_3);
      PwmSingleOut led2 = PwmSingleOut(&htim2, TIM_CHANNEL_4);
      PwmSingleOut led3 = PwmSingleOut(&htim3, TIM_CHANNEL_4);
      PwmSingleOut led4 = PwmSingleOut(&htim3, TIM_CHANNEL_3);

      BufferedSerial serial3 = BufferedSerial(&huart3, 256);  // main
      // BufferedSerial serial1 = BufferedSerial(&huart1, 256);  // cam3
      // BufferedSerial serial2 = BufferedSerial(&huart2, 256);  // cam1
      // BufferedSerial serial5 = BufferedSerial(&huart5, 256);  // cam2
      // BufferedSerial serial6 = BufferedSerial(&huart6, 256);  // cam4

      BufferedSerial m1n_serial[4] = {BufferedSerial(&huart2, 256),
                                      BufferedSerial(&huart5, 256),
                                      BufferedSerial(&huart1, 256),
                                      BufferedSerial(&huart6, 256)};

      void Init();

      void MainUart();
      void M1nUart();

      Timer main_send_interval_timer;

     private:
};

#endif