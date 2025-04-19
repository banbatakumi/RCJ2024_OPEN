#ifndef _CORE0A_MAIN_H_
#define _CORE0A_MAIN_H_

#include "setup.h"

#define CORE0A_CONTROL_FREQ 50  // Hz

#ifdef ROBOT_1
String MACadd = "3C:61:05:67:EB:AA";  // ROBOT_2のアドレス
uint8_t address[6] = {0x3C, 0x61, 0x05, 0x67, 0xEB, 0xAA};
#endif
#ifdef ROBOT_2
String MACadd = "EC:94:CB:7E:05:42";  // ROBOT_1のアドレス
uint8_t address[6] = {0xEC, 0x94, 0xCB, 0x7E, 0x05, 0x42};
#endif

uint16_t disconnect_count;

void Core0a_setup() {
      pinMode(led_pin, OUTPUT);
      #ifdef ROBOT_1
            SerialBT.begin("ROBOT_1_ESP32", true);
            bool connected = SerialBT.connect(address);
            if (connected) {
                  Serial.println("Connect OK");
            } else {
                  while (!SerialBT.connected(10000)) Serial.println("No connect");
            }
            if (SerialBT.disconnect()) Serial.println("Disconnected Succesfully!");

            SerialBT.connect();
      #endif
      #ifdef ROBOT_2
            SerialBT.begin("ROBOT_2_ESP32");
      #endif
      #ifdef GET_MAC
            uint8_t macBT[6];
            esp_read_mac(macBT, ESP_MAC_BT);
            Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", macBT[0], macBT[1], macBT[2], macBT[3], macBT[4], macBT[5]);
      #endif
      //SerialBT.begin("ROBOT_ESP32");
}

void Core0a_loop() {
      #ifdef ROBOT_1
            if (SerialBT.connected(10000)) {
                  is_connect = 1;
                  // 送信
                  uint8_t send_data = can_get_pass << 3 | is_catch_ball << 2 | is_defense << 1 | is_moving;
                  SerialBT.write(send_data);

                  // 受信
                  if (SerialBT.available()) {
                        uint8_t recv_data = SerialBT.read();
                        is_ally_moving = (recv_data) & 1;
                        is_ally_defense = (recv_data >> 1) & 1;
                        is_ally_catch_ball = (recv_data >> 2) & 1;
                        can_ally_get_pass = (recv_data >> 3) & 1;

                        digitalWrite(led_pin, HIGH);
                  } else {
                        digitalWrite(led_pin, LOW);
                  }
            } else {
                  is_connect = 0;
                  SerialBT.begin("ROBOT_1_ESP32", true);
                  bool connected = SerialBT.connect(address);
                  if (connected) {
                        Serial.println("Connect OK");
                  } else {
                        while (!SerialBT.connected(10000)) Serial.println("No connect");
                  }
            }
            Serial.println(is_connect);
      #endif
      #ifdef ROBOT_2
            // 送信
            uint8_t send_data = can_get_pass << 3 | is_catch_ball << 2 | is_defense << 1 | is_moving;
            SerialBT.write(send_data);

            // 受信
            if (SerialBT.available()) {
                  is_connect = 1;
                  uint8_t recv_data = SerialBT.read();
                  is_ally_moving = (recv_data) & 1;
                  is_ally_defense = (recv_data >> 1) & 1;
                  is_ally_catch_ball = (recv_data >> 2) & 1;
                  can_ally_get_pass = (recv_data >> 3) & 1;
                  digitalWrite(led_pin, HIGH);
                  disconnect_count = 0;
            } else {
                  digitalWrite(led_pin, LOW);
                  disconnect_count++;
                  if (disconnect_count > 100) {
                        SerialBT.begin("ROBOT_2_ESP32");
                        disconnect_count = 0;
                        is_connect = 0;
                  }
            }
      #endif
      //if (SerialBT.available()) pc_command = SerialBT.read();
}

#endif