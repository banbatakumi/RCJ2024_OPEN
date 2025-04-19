#include "hardware.hpp"

uint16_t adc_get_val[12];

Hardware::Hardware() {
}

void Hardware::Init() {
      led3 = 1;
      HAL_ADC_Start(&hadc1);
      HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_get_val, 12);
      for (uint8_t i = 0; i < 12; i++) {
            while (!(adc_get_val[i] > 0));
      }

      // 諸々の初期化
      serial6.init();
      HAL_Delay(100);
      led3 = 0;
}

void Hardware::GetSensors() {
      info.Encoder.photo_val[0] = adc_get_val[0];
      info.Encoder.photo_val[1] = adc_get_val[1];
      info.Encoder.photo_val[2] = adc_get_val[2];
      info.Encoder.photo_val[3] = adc_get_val[3];

      info.Line.leftside_val = adc_get_val[4];
      info.Line.rightside_val = adc_get_val[5];
      info.Line.mux1x_val = adc_get_val[6];
      info.Line.mux1y_val = adc_get_val[7];
      info.Line.mux2x_val = adc_get_val[8];
      info.Line.mux2y_val = adc_get_val[9];
      info.Line.mux3x_val = adc_get_val[10];
      info.Line.mux3y_val = adc_get_val[11];

      info.Line.dir = line.dir;
      info.Line.is_half_out = line.is_half_out;
      info.Line.is_leftside_white = line.is_leftside_white;
      info.Line.is_rightside_white = line.is_rightside_white;
      info.Line.is_on_line = line.IsOnLine();
      info.Line.max_interval = line.max_interval;

      info.Encoder.rps[0] = encoder.GetMotorRPS(0);
      info.Encoder.rps[1] = encoder.GetMotorRPS(1);
      info.Encoder.rps[2] = encoder.GetMotorRPS(2);
      info.Encoder.rps[3] = encoder.GetMotorRPS(3);
}

void Hardware::MainUart() {
      if (send_interval_timer.read_us() >= SEND_PERIOD_US) {
            static const uint8_t HEADER = 0xFF;  // ヘッダ
            static const uint8_t FOOTER = 0xAA;  // ヘッダ
            static const uint8_t data_size = 9;
            uint8_t send_data[data_size];
            send_data[0] = HEADER;
            send_data[1] = info.Encoder.rps[0];
            send_data[2] = info.Encoder.rps[1];
            send_data[3] = info.Encoder.rps[2];
            send_data[4] = info.Encoder.rps[3];
            send_data[5] = info.Line.max_interval << 4 | info.Line.is_on_line << 3 | info.Line.is_half_out << 2 | info.Line.is_leftside_white << 1 | info.Line.is_rightside_white;
            send_data[6] = info.Line.success_reset_line;
            send_data[7] = info.Line.dir * 0.5 + 90;
            send_data[8] = FOOTER;
            serial6.write(send_data, data_size);
            send_interval_timer.reset();
      }

      // 受信

      while (serial6.available()) {
            static const uint8_t HEADER = 0xFF;   // ヘッダ
            static const uint8_t FOOTER = 0xAA;   // フッタ
            static const uint8_t data_size = 2;   // データのサイズ
            static uint8_t index = 0;             // 受信したデータのインデックスカウンター
            static uint8_t recv_data[data_size];  // 受信したデータ
            static uint8_t recv_byte;

            recv_byte = serial6.read();
            if (index == 0) {
                  if (recv_byte == HEADER) {
                        index++;
                  } else {
                        index = 0;
                  }
            } else if (index == (data_size + 1)) {
                  if (recv_byte == FOOTER) {
                        info.Line.do_read = recv_data[0];
                        info.Line.reset_line = recv_data[1];
                  }
                  index = 0;
            } else {
                  recv_data[index - 1] = recv_byte;
                  index++;
            }
      }
}