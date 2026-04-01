#ifndef _CORE0B_MAIN_H_
#define _CORE0B_MAIN_H_

#include "setup.h"

#define CORE0B_CONTROL_FREQ 500  // Hz
#define MAX_POWER 100

void Core0b_setup() {
      Wire.begin();
      Wire.setClock(400000);

      mpu.initialize();

      devStatus = mpu.dmpInitialize();

// supply your own gyro offsets here, scaled for min sensitivity
#ifdef ROBOT_1
      mpu.setXAccelOffset(1750);
      mpu.setYAccelOffset(1588);
      mpu.setZAccelOffset(1486);
      mpu.setXGyroOffset(410);
      mpu.setYGyroOffset(87);
      mpu.setZGyroOffset(28);
#endif
#ifdef ROBOT_2
      mpu.setXAccelOffset(-692);
      mpu.setYAccelOffset(270);
      mpu.setZAccelOffset(946);
      mpu.setXGyroOffset(-41);
      mpu.setYGyroOffset(128);
      mpu.setZGyroOffset(38);
#endif

      if (devStatus == 0) {
            // mpu.CalibrateAccel(5);
            // mpu.CalibrateGyro(5);
            // mpu.PrintActiveOffsets();

            mpu.setDMPEnabled(true);

            packetSize = mpu.dmpGetFIFOPacketSize();
      } else {
            Serial.print(F("DMP Initialization failed (code "));
            Serial.print(devStatus);
            Serial.println(F(")"));
      }
      motorPID.SelectType(PI_D_TYPE);
      motorPID.SetGain(8, 8, 1);
      motorPID.SetILimit(100);
}

void Core0b_loop() {
      if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {  //  read a packet from FIFO
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
      }
      yaw = SimplifyDeg(ypr[0] * 180 / M_PI - yaw_correction);
      if (do_rotate) {
            int16_t power;
            motorPID.Compute(yaw, 0);
            power = motorPID.Get();
            if (abs(power) > MAX_POWER) power = MAX_POWER * (abs(power) / power);
            motor.Run(power);
      } else {
            motor.Run(0);
            motorPID.ResetPID();
            yaw_correction += yaw;
      }
}

#endif