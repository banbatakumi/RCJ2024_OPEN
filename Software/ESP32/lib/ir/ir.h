#ifndef IR_H
#define IR_H

#include "Arduino.h"
#include "moving_ave.h"
#include "my_math.h"

#define IR_QTY 8
#define READ_NUM_OF_TIME 1500
#define MOVING_AVE_NUM 5
#define RC 0.5

class Ir {
     public:
      Ir(uint8_t ir_0_, uint8_t ir_1_, uint8_t ir_2_, uint8_t ir_3_, uint8_t ir_4_, uint8_t ir_5_, uint8_t ir_6_, uint8_t ir_7_);
      void Read();
      int16_t GetDir();
      uint8_t GetDis();

     private:
      MovingAve Ave0;
      MovingAve Ave1;
      MovingAve Ave2;
      MovingAve Ave3;
      MovingAve Ave4;
      MovingAve Ave5;
      MovingAve Ave6;
      MovingAve Ave7;

      float unit_vector_x[IR_QTY];
      float unit_vector_y[IR_QTY];
      uint8_t ir[IR_QTY];
      int16_t val[IR_QTY];
      int16_t rc_val[IR_QTY];
      int16_t result_vector_x, result_vector_y;
      int16_t dir, dis;
};

#endif