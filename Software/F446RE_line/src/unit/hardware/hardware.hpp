#ifndef __HARDWARE__
#define __HARDWARE__

#include "BufferedSerial.hpp"
#include "DigitalInOut.hpp"
#include "MyMath.hpp"
#include "adc.h"
#include "encoder.hpp"
#include "line.hpp"

typedef struct {
      struct {
            uint8_t photo_val[4];
            uint8_t rps[4];
      } Encoder;

      struct {
            uint8_t leftside_val, rightside_val;
            uint8_t mux1x_val, mux1y_val;
            uint8_t mux2x_val, mux2y_val;
            uint8_t mux3x_val, mux3y_val;

            bool reset_line;
            bool success_reset_line;

            volatile bool do_read;

            int16_t dir;
            uint8_t max_interval;
            bool is_half_out;
            bool is_on_line;
            bool is_leftside_white;
            bool is_rightside_white;
      } Line;
} Info;

class Hardware {
     public:
      Hardware();

      // values
      Info info;

      DigitalOut led1 = DigitalOut(LED1_GPIO_Port, LED1_Pin);
      DigitalOut led2 = DigitalOut(LED2_GPIO_Port, LED2_Pin);
      DigitalOut led3 = DigitalOut(LED3_GPIO_Port, LED3_Pin);

      DigitalOut led_line = DigitalOut(LED_LINE_GPIO_Port, LED_LINE_Pin);

      DigitalOut mux1a = DigitalOut(MUX1A_GPIO_Port, MUX1A_Pin);
      DigitalOut mux1b = DigitalOut(MUX1B_GPIO_Port, MUX1B_Pin);
      DigitalOut mux2a = DigitalOut(MUX2A_GPIO_Port, MUX2A_Pin);
      DigitalOut mux2b = DigitalOut(MUX2B_GPIO_Port, MUX2B_Pin);
      DigitalOut mux3a = DigitalOut(MUX3A_GPIO_Port, MUX3A_Pin);
      DigitalOut mux3b = DigitalOut(MUX3B_GPIO_Port, MUX3B_Pin);

      BufferedSerial serial6 = BufferedSerial(&huart6, 256);

      Encoder encoder = Encoder(&led_line, info.Encoder.photo_val);

      Line line = Line(&led_line, &mux1a, &mux1b, &mux2a, &mux2b, &mux3a, &mux3b,
                       &info.Line.leftside_val, &info.Line.rightside_val,
                       &info.Line.mux1x_val, &info.Line.mux1y_val,
                       &info.Line.mux2x_val, &info.Line.mux2y_val,
                       &info.Line.mux3x_val, &info.Line.mux3y_val);

      void Init();
      void GetSensors();
      void MainUart();

      Timer send_interval_timer;

     private:
};

#endif