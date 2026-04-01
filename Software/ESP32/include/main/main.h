#ifndef _MAIN_H_
#define _MAIN_H_

#include "setup.h"

#define MAIN_CONTROL_FREQ 1000

void main_setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  pinMode(ir_led_pin[0], OUTPUT);
  pinMode(ir_led_pin[1], OUTPUT);
  pinMode(ir_led_pin[2], OUTPUT);
  pinMode(ir_led_pin[3], OUTPUT);
}

void main_loop() {
  ir.Read();
  if (on_ir_led) {
    // for (uint8_t i = 0; i < 8; i++) {
    //       digitalWrite(ir_led_pin[0], HIGH);
    //       delayMicroseconds(12);
    //       digitalWrite(ir_led_pin[0], LOW);
    //       delayMicroseconds(12);
    // }
    // for (uint8_t i = 0; i < 4; i++) {
    //       digitalWrite(ir_led_pin[1], HIGH);
    //       delayMicroseconds(12);
    //       digitalWrite(ir_led_pin[1], LOW);
    //       delayMicroseconds(12);
    // }
    // for (uint8_t i = 0; i < 4; i++) {
    //       digitalWrite(ir_led_pin[2], HIGH);
    //       delayMicroseconds(12);
    //       digitalWrite(ir_led_pin[2], LOW);
    //       delayMicroseconds(12);
    // }
    // for (uint8_t i = 0; i < 4; i++) {
    //       digitalWrite(ir_led_pin[3], HIGH);
    //       delayMicroseconds(12);
    //       digitalWrite(ir_led_pin[3], LOW);
    //       delayMicroseconds(12);
    // }
  }
}

#endif