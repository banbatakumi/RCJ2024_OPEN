#ifndef __ENCODER__
#define __ENCODER__

#include "DigitalInOut.hpp"
#include "MovingAve.hpp"
#include "MyMath.hpp"
#include "Timer.hpp"

#define SENSOR_QTY 4
#define MAX_RPS 200  // 12000[rpm] / 60[s]
#define MIN_RPS 2.5  // 1 / (0.1[s] * 4)
#define MIN_INTERVAL (1.0f / (4 * MAX_RPS))
#define MAX_INTERVAL (1.0f / (4 * MIN_RPS))
class Encoder {
     public:
      Encoder(DigitalOut *led_line, uint8_t *photo_val);

      void Read();
      uint8_t GetMotorRPS(uint8_t encoder_num);

     private:
      uint8_t *photo_val_;
      DigitalOut *led_line_;

      uint8_t pre_photo_val[SENSOR_QTY];
      uint8_t rps_[SENSOR_QTY];
      double interval_time_s_[SENSOR_QTY];
      double pre_interval_time_s_[SENSOR_QTY];
      uint8_t threshold_[SENSOR_QTY];
      uint8_t min_val_[SENSOR_QTY] = {255, 255, 255, 255};
      uint8_t max_val_[SENSOR_QTY];
      bool is_white_[SENSOR_QTY];
      bool pre_is_white_[SENSOR_QTY];

      Timer interval_timer[SENSOR_QTY];
      MovingAve ave[SENSOR_QTY];
};

#endif