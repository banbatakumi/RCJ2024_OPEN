#ifndef _SETUP_H_
#define _SETUP_H_

// #include "BluetoothSerial.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "WiFi.h"
#include "Wire.h"
#include "ir.h"
#include "motor.h"
#include "pid.h"
#include "simplify_deg.h"

#define wifi
#define ROBOT_2

// ピン定義
const uint8_t led_pin = 23;
const uint8_t ir_led_pin[4] = {4, 5, 18, 19};

Ir ir(12, 14, 27, 26, 25, 2, 15, 13);
Motor motor(32, 33);
MPU6050 mpu;
PID motorPID;
#ifdef bluetooth
BluetoothSerial SerialBT;
#endif

// MPU control/status vars
uint8_t devStatus;       // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
Quaternion q;         // [w, x, y, z]         quaternion container
VectorFloat gravity;  // [x, y, z]            gravity vector
float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

float yaw_correction;  // yaw軸の補正値
int16_t yaw;

bool is_connect;

// メインマイコンから受け取るデータ
bool do_rotate;
bool on_ir_led;
#ifdef bluetooth
bool is_moving;
bool is_defense;
bool is_catch_ball;
bool can_get_pass;
#endif
// blutoothで受け取るデータ
bool is_ally_moving;
bool is_ally_defense;
bool is_ally_catch_ball;
bool can_ally_get_pass;

#ifdef wifi
float voltage;
int16_t own_x;
int16_t own_y;
int16_t moving_dir;
float moving_speed;
int16_t ball_dir;
uint8_t ball_dis;
int16_t yellow_goal_dir;
uint8_t yellow_goal_size;
int16_t blue_goal_dir;
uint8_t blue_goal_size;
bool is_hold_ball_front;
bool is_hold_ball_back;
bool is_on_line;
int16_t line_inside_dir;
uint8_t line_depth;
int16_t under_yaw;

int16_t move_dir;
float move_speed;
int16_t face_angle;
bool do_kick;
bool stop;
bool do_dribble;
#endif
#endif