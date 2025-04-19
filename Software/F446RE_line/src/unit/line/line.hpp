#ifndef __LINE_HPP__
#define __LINE_HPP__

#include "DigitalInOut.hpp"
#include "MovingAve.hpp"
#include "MyMath.hpp"
#include "Timer.hpp"
#include "config.h"

#define LINE_QTY 24
#define SET_TH_NUM 3000
#define TH_VALUE 5
#define READ_INTERVAL_TIME_US 10

class Line {
     public:
      Line(DigitalOut *led_line,
           DigitalOut *mux1a, DigitalOut *mux1b,
           DigitalOut *mux2a, DigitalOut *mux2b,
           DigitalOut *mux3a, DigitalOut *mux3b,
           uint8_t *leftside_val, uint8_t *rightside_val,
           uint8_t *mux1x_val, uint8_t *mux1y_val,
           uint8_t *mux2x_val, uint8_t *mux2y_val,
           uint8_t *mux3x_val, uint8_t *mux3y_val);

      void Read();
      void Compute();
      bool SetTh();
      bool IsOnLine();
      void OnLed();
      void OffLed();

      int16_t dir;
      uint8_t max_interval;
      bool is_half_out;
      bool is_leftside_white;
      bool is_rightside_white;

     private:
      DigitalOut *led_line_;
      DigitalOut *mux1a_;
      DigitalOut *mux1b_;
      DigitalOut *mux2a_;
      DigitalOut *mux2b_;
      DigitalOut *mux3a_;
      DigitalOut *mux3b_;

      uint8_t *leftside_val_;
      uint8_t *rightside_val_;
      uint8_t *mux1x_val_;
      uint8_t *mux1y_val_;
      uint8_t *mux2x_val_;
      uint8_t *mux2y_val_;
      uint8_t *mux3x_val_;
      uint8_t *mux3y_val_;

      uint8_t read_sensors_group_ = 0;

      uint32_t val_[LINE_QTY];
      uint32_t pre_val_[LINE_QTY];
      uint32_t th_val_[LINE_QTY];

      uint32_t leftside_th_;
      uint32_t rightside_th_;
      uint16_t set_th_cnt_ = 0;

      int16_t pre_dir_;
      bool pre_white_qty_;
      bool is_white_[LINE_QTY];
      uint8_t white_qty_;

      Timer read_interval_timer;
};

#endif