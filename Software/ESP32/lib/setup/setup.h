#ifndef _SETUP_H_
#define _SETUP_H_

#include "BluetoothSerial.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "WiFi.h"
#include "Wire.h"
#include "ir.h"
#include "motor.h"
#include "pid.h"
#include "simplify_deg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/soc.h"

#define bluetooth
#define ROBOT_2
#define USE_REMOTE_CONTROLLER  // リモコン受信機能を有効にする

// ピン定義
const uint8_t led_pin = 23;
const uint8_t ir_led_pin[4] = {4, 5, 18, 19};

// リモコンデータ構造体
struct RemoteData {
  int8_t joy_lx;      // 左ジョイスティックX (-100～100)
  int8_t joy_ly;      // 左ジョイスティックY (-100～100)
  int8_t joy_rx;      // 右ジョイスティックX (未実装)
  int8_t joy_ry;      // 右ジョイスティックY (未実装)
  bool button_a;      // ボタンA
  bool button_b;      // ボタンB
  bool button_x;      // ボタンX
  bool button_y;      // ボタンY
  bool button_lb;     // LB
  bool button_rb;     // RB
  bool button_lt;     // LT
  bool button_rt;     // RT
  bool button_back;   // BACK
  bool button_start;  // START
  bool button_ls;     // 左ジョイスティック押し込み (JSW)
  bool button_rs;     // 右ジョイスティック押し込み (SC)
};

RemoteData remote_data = {};  // グローバル変数

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