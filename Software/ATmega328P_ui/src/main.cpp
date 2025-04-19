#include <Arduino.h>

#include "config.h"

void setup() {
      Serial.begin(38400);  // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200

      // OLEDセットアップ
      oled.Init();
      oled.FirstPage();
      do {
            oled.PrintCenter("Crescent(Re)", 64, 32);
      } while (oled.NextPage());

      led.SetBrightness(50);
      led.Clear();

      // 起動音
      tone(buzzer_pin, 2000, 100);
      delay(100);
      tone(buzzer_pin, 1000, 100);
      delay(100);
}

void loop() {  // 呼び出しのオーバーヘッド節減
      while (1) {
            ModeRun();
      }
}