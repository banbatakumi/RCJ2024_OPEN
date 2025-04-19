#include "ir.h"

Ir::Ir(uint8_t ir_0_, uint8_t ir_1_, uint8_t ir_2_, uint8_t ir_3_, uint8_t ir_4_, uint8_t ir_5_, uint8_t ir_6_, uint8_t ir_7_) {
      ir[0] = ir_0_;
      ir[1] = ir_1_;
      ir[2] = ir_2_;
      ir[3] = ir_3_;
      ir[4] = ir_4_;
      ir[5] = ir_5_;
      ir[6] = ir_6_;
      ir[7] = ir_7_;

      Ave0.SetLength(MOVING_AVE_NUM);
      Ave1.SetLength(MOVING_AVE_NUM);
      Ave2.SetLength(MOVING_AVE_NUM);
      Ave3.SetLength(MOVING_AVE_NUM);
      Ave4.SetLength(MOVING_AVE_NUM);
      Ave5.SetLength(MOVING_AVE_NUM);
      Ave6.SetLength(MOVING_AVE_NUM);
      Ave7.SetLength(MOVING_AVE_NUM);

      for (uint8_t i = 0; i < 8; i++) pinMode(ir[i], INPUT);
      for (uint8_t i = 0; i < IR_QTY; i++) {  // 各センサに座標を与える
            unit_vector_x[i] = MyCos(i * 360.0f / IR_QTY);
            unit_vector_y[i] = MySin(i * 360.0f / IR_QTY);
      }
}

void Ir::Read() {
      // センサそれぞれの値を読む
      for (uint8_t i = 0; i < IR_QTY; i++) val[i] = 0;
      for (uint16_t i = 0; i < READ_NUM_OF_TIME; i++) {
            for (uint8_t j = 0; j < IR_QTY; j++) val[j] += digitalRead(ir[j]);
      }

      // 移動平均を取る
      Ave0.Compute(&val[0]);
      Ave1.Compute(&val[1]);
      Ave2.Compute(&val[2]);
      Ave3.Compute(&val[3]);
      Ave4.Compute(&val[4]);
      Ave5.Compute(&val[5]);
      Ave6.Compute(&val[6]);
      Ave7.Compute(&val[7]);

      result_vector_x = 0;
      result_vector_y = 0;
      for (uint8_t i = 0; i < IR_QTY; i++) {
            val[i] = (READ_NUM_OF_TIME - val[i]) * (100.0f / READ_NUM_OF_TIME);  // 100までに補正
            rc_val[i] = val[i] * (1 - RC) + rc_val[i] * RC;
            result_vector_x += rc_val[i] * unit_vector_x[i];  // X成分を合成
            result_vector_y += rc_val[i] * unit_vector_y[i];  // Y成分を合成
      }

      dir = MyAtan2(result_vector_y, result_vector_x);                                    // 角度を出す
      dis = sqrt(result_vector_x * result_vector_x + result_vector_y * result_vector_y);  // 距離を出す
      if (dis > 100) dis = 100;
}

int16_t Ir::GetDir() {
      return dir;
}

uint8_t Ir::GetDis() {
      return dis;
}