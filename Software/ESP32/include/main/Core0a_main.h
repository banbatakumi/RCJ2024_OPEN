#ifndef _CORE0A_MAIN_H_
#define _CORE0A_MAIN_H_

#include "setup.h"

#define CORE0A_CONTROL_FREQ 100  // Hz

#ifdef bluetooth
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
      // SerialBT.begin("ROBOT_ESP32");
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
      // if (SerialBT.available()) pc_command = SerialBT.read();
}

#endif

#ifdef wifi
const char* ssid = "ESP32-Crescent";  // SSID
const char* password = "20060210";    // 8文字以上

WiFiServer server(1234);
WiFiClient client;
void Core0a_setup() {  // アクセスポイントモード起動
      WiFi.softAP(ssid, password);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);

      server.begin();
}

void Core0a_loop() {
      // 新しいクライアントが来たら保存
      if (!client || !client.connected()) {
            client = server.available();
            if (client) {
                  Serial.println("クライアント接続完了");
            }
      }

      // 接続中かつ送信タイミングになったらデータ送信
      if (client && client.connected()) {
            String payload = String(voltage) + "," +
                             String(moving_dir) + "," +
                             String(moving_speed, 2) + "," +
                             String(own_x) + "," +
                             String(own_y) + "," +
                             String(under_yaw) + "," +
                             String(ball_dir) + "," +
                             String(ball_dis) + "," +
                             String(yellow_goal_dir) + "," +
                             String(yellow_goal_size) + "," +
                             String(blue_goal_dir) + "," +
                             String(blue_goal_size) + "," +
                             String(is_hold_ball_front) + "," +
                             String(is_hold_ball_back) + "," +
                             String(is_on_line) + "," +
                             String(line_inside_dir) + "," +
                             String(line_depth);
            client.println(payload);

            // データ受信
            if (client.available()) {
                  String receivedData = client.readStringUntil('\n');  // 改行までのデータを受信
                  Serial.println("受信データ: " + receivedData);

                  do_kick = false;  // キック動作を実行するフラグを立てる
                  // データ解析
                  if (receivedData.startsWith("MOVE:")) {
                        stop = false;
                        String str = receivedData.substring(5);  // "MOVE:"以降を取得
                        int16_t angle = str.toInt();             // 角度を整数に変換
                        move_dir = angle;                        // ロボットの進行方向を更新

                        Serial.println("進行方向を更新: " + String(move_dir));
                  } else if (receivedData.startsWith("DRIBBLER:")) {
                        String str = receivedData.substring(9);  // "DRIBBLER:"以降を取得
                        if (str == "ON") {
                              do_dribble = true;  // ドリブル動作を実行するフラグを立てる
                              Serial.println("ドリブル動作を実行");
                        } else if (str == "OFF") {
                              do_dribble = false;  // ドリブル動作を停止するフラグを立てる
                              Serial.println("ドリブル動作を停止");
                        }
                  } else if (receivedData.startsWith("SPEED:")) {
                        String str = receivedData.substring(6);  // "SPEED:"以降を取得
                        move_speed = str.toFloat();              // スピードを浮動小数点数に変換
                        Serial.println("スピードを更新: " + String(move_speed));
                  } else if (receivedData.startsWith("FACE:")) {
                        String str = receivedData.substring(5);  // "FACE:"以降を取得
                        face_angle = str.toInt();                // 角度を整数に変換
                        Serial.println("顔の角度を更新: " + String(face_angle));
                  } else if (receivedData == "KICK") {
                        do_kick = true;  // キック動作を実行するフラグを立てる
                        Serial.println("キック動作を実行");
                  } else if (receivedData == "STOP") {
                        stop = true;  // ロボットを停止
                        Serial.println("ロボットを停止");
                  } else {
                        Serial.println("無効なコマンド: " + receivedData);
                  }
            }
      }
}
#endif

#endif