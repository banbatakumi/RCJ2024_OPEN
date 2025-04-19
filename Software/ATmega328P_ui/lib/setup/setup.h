#ifndef _SETUP_H_
#define _SETUP_H_

#include "Adafruit_NeoPixel.h"
#include "neopixel.h"
#include "oled.h"
#include "simplify_deg.h"

// #define CenterX(x_, charnum_) (x_ - ((charnum_ * 9) / 2))
// #define CenterY(y_) (y_ + 6)

const uint8_t led_pin = 2;
const uint8_t buzzer_pin = 8;
const uint8_t robocup = 9;
const uint8_t button_pin[3] = {6, 7, 5};  // center, left, right

Oled oled;

NeoPixel led;

void ButtonRead();
void SendData();

void Home();
void Speed();
void Dribbler();
void Camera();
void Line();

int8_t item = 0, sub_item = 0, pre_sub_item;
int8_t set_val = 0;
bool is_button[3], pre_is_button[3];

// send data
bool reset_yaw;
bool success_reset_yaw;
bool reset_line;
bool success_reset_line;
uint8_t mode = 0;
uint8_t moving_speed = 100;
uint8_t line_moving_speed = 100;
uint8_t dribbler_sig = 0;

// receive data
bool is_moving;

#endif