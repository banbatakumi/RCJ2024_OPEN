#include <Arduino.h>

#include "MPU6050_6Axis_MotionApps612.h"

#define CHANGE_TO_DEG 57.2957795131

const uint8_t LED_RED = 16;
const uint8_t LED_GREEN = 17;

MPU6050 mpu;

// MPU control/status vars
uint8_t devStatus;       // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;     // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64];  // FIFO storage buffer

// orientation/motion vars
Quaternion q;         // [w, x, y, z]         quaternion container
VectorFloat gravity;  // [x, y, z]            gravity vector
float ypr[3];         // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

void setup() {
      Serial.begin(115200);  // 通信速度: 9600, 14400, 19200, 28800, 38400, 57600, 115200

      pinMode(LED_GREEN, OUTPUT);
      pinMode(LED_RED, OUTPUT);
      digitalWrite(LED_GREEN, HIGH);

      // IMU
      Wire.begin();
      Wire.setClock(400000);  // 400kHz I2C clock. Comment this line if having compilation difficulties

      mpu.initialize();

      devStatus = mpu.dmpInitialize();

      // supply your own gyro offsets here, scaled for min sensitivity

      // robot1
      // mpu.setXAccelOffset(-4503);
      // mpu.setYAccelOffset(-4119);
      // mpu.setZAccelOffset(1411);
      // mpu.setXGyroOffset(98);
      // mpu.setYGyroOffset(8);
      // mpu.setZGyroOffset(-3);
      // robot2
      mpu.setXAccelOffset(-7817);
      mpu.setYAccelOffset(-1307);
      mpu.setZAccelOffset(1107);
      mpu.setXGyroOffset(1);
      mpu.setYGyroOffset(26);
      mpu.setZGyroOffset(11);

      digitalWrite(LED_GREEN, LOW);
      if (devStatus == 0) {
            // mpu.CalibrateAccel(5);
            // mpu.CalibrateGyro(5);
            // mpu.PrintActiveOffsets();

            mpu.setDMPEnabled(true);

            packetSize = mpu.dmpGetFIFOPacketSize();

      } else {
            digitalWrite(LED_RED, HIGH);
      }
      TIMSK0 = 0;
}

void loop() {
      while (1) {                                           // 呼び出しのオーバーヘッド節減
            if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {  // Get the Latest packet 200Hz
                  mpu.dmpGetQuaternion(&q, fifoBuffer);
                  mpu.dmpGetGravity(&gravity, &q);
                  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

                  int16_t yaw = ypr[0] * CHANGE_TO_DEG * 10 + 1800;
                  int16_t pitch = ypr[2] * CHANGE_TO_DEG * 10 + 1800;
                  int16_t roll = ypr[1] * CHANGE_TO_DEG * 10 + 1800;

                  // UART送信
                  Serial.write(0xFF);
                  Serial.write((uint8_t)((yaw & 0xFF00) >> 8));
                  Serial.write((uint8_t)(yaw & 0x00FF));
                  Serial.write((uint8_t)((pitch & 0xFF00) >> 8));
                  Serial.write((uint8_t)(pitch & 0x00FF));
                  Serial.write((uint8_t)((roll & 0xFF00) >> 8));
                  Serial.write((uint8_t)(roll & 0x00FF));
                  Serial.write(0xAA);
                  // Serial.println(yaw - 1800);
            }
      }
}