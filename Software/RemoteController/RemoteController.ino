#include <Arduino.h>
#include <BluetoothSerial.h>

// Bluetooth オブジェクト
BluetoothSerial SerialBT;

// ピン定義
const int JOYSTICK_X = 36;   // アナログ入力
const int JOYSTICK_Y = 39;   // アナログ入力
const int JOYSTICK_SW = 32;  // デジタル入力
const int SWITCH_A = 33;     // デジタル入力
const int SWITCH_B = 26;     // デジタル入力
const int SWITCH_C = 25;     // デジタル入力

// キャリブレーション値（中央値）
int centerX = 2048;
int centerY = 2048;

// 前回の値
int lastJoyX = 0;
int lastJoyY = 0;
bool lastSwitchSW = false;
bool lastSwitchA = false;
bool lastSwitchB = false;
bool lastSwitchC = false;

// センサー値構造体
struct SensorData {
  int8_t joyX;  // -100 ～ 100
  int8_t joyY;  // -100 ～ 100
  bool joySW;
  bool switchA;
  bool switchB;
  bool switchC;
};

// センサー値を読み取る
SensorData readSensors() {
  SensorData data;
  // ジョイスティックの値を-100～100に調整（キャリブレーション済み中央値を使用）
  int rawX = analogRead(JOYSTICK_X);
  int rawY = analogRead(JOYSTICK_Y);

  // 左右で異なる分母を使用して常に-100～100の範囲に正規化
  if (rawX >= centerX) {
    data.joyX = (rawX - centerX) * 100 / (4095 - centerX);
  } else {
    data.joyX = (rawX - centerX) * 100 / centerX;
  }

  if (rawY >= centerY) {
    data.joyY = (rawY - centerY) * 100 / (4095 - centerY);
  } else {
    data.joyY = (rawY - centerY) * 100 / centerY;
  }

  data.joySW = !digitalRead(JOYSTICK_SW);  // プルアップなので反転
  data.switchA = !digitalRead(SWITCH_A);
  data.switchB = !digitalRead(SWITCH_B);
  data.switchC = !digitalRead(SWITCH_C);
  return data;
}

// Bluetoothでデータを送信
void sendData(const SensorData& data) {
  // フォーマット: "JX:xxxx,JY:xxxx,JSW:x,SA:x,SB:x,SC:x"
  String message = String("JX:") + data.joyX +
                   ",JY:" + data.joyY +
                   ",JSW:" + (data.joySW ? 1 : 0) +
                   ",SA:" + (data.switchA ? 1 : 0) +
                   ",SB:" + (data.switchB ? 1 : 0) +
                   ",SC:" + (data.switchC ? 1 : 0);
  SerialBT.println(message);
}

// ジョイスティックをキャリブレーション
void calibrateJoystick() {
  Serial.println("\n⚙️  Calibrating joystick...");
  Serial.println("Keep joystick at center position for 2 seconds");
  delay(2000);

  long sumX = 0, sumY = 0;
  const int SAMPLES = 100;

  for (int i = 0; i < SAMPLES; i++) {
    sumX += analogRead(JOYSTICK_X);
    sumY += analogRead(JOYSTICK_Y);
    delay(10);
  }

  centerX = sumX / SAMPLES;
  centerY = sumY / SAMPLES;

  Serial.printf("✓ Calibration complete! CenterX: %d, CenterY: %d\n", centerX, centerY);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=== ESP32 Remote Controller ===");
  Serial.println("Initializing...");

  // ピン設定
  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);  // 内部プルアップ有効
  pinMode(SWITCH_A, INPUT_PULLUP);     // 内部プルアップ有効
  pinMode(SWITCH_B, INPUT_PULLUP);     // 内部プルアップ有効
  pinMode(SWITCH_C, INPUT_PULLUP);     // 内部プルアップ有効

  // Bluetooth初期化
  SerialBT.begin("ESP32_RemoteController");  // Bluetooth名を設定
  Serial.println("✓ Bluetooth started");
  Serial.println("✓ Device name: ESP32_RemoteController");
  Serial.println("✓ Pin configuration complete");

  // ジョイスティックをキャリブレーション
  calibrateJoystick();

  Serial.println("\nWaiting for Bluetooth connection...");
  Serial.println("=============================\n");
}

void loop() {
  // センサー値を読み取る
  SensorData data = readSensors();

  // ジョイスティック: デッドゾーン(±2)を設定して変化をチェック
  bool joyChanged = (abs(data.joyX - lastJoyX) > 2 ||
                     abs(data.joyY - lastJoyY) > 2);

  // スイッチ: 状態変化をチェック
  bool switchChanged = (data.joySW != lastSwitchSW ||
                        data.switchA != lastSwitchA ||
                        data.switchB != lastSwitchB ||
                        data.switchC != lastSwitchC);

  // データ送信（変化があった場合）
  if (joyChanged || switchChanged) {
    sendData(data);

    // シリアルデバッグ出力
    Serial.print("📡 Sent - JX:");
    Serial.print(data.joyX);
    Serial.print(" JY:");
    Serial.print(data.joyY);
    Serial.print(" JSW:");
    Serial.print(data.joySW);
    Serial.print(" SA:");
    Serial.print(data.switchA);
    Serial.print(" SB:");
    Serial.print(data.switchB);
    Serial.print(" SC:");
    Serial.println(data.switchC);

    lastJoyX = data.joyX;
    lastJoyY = data.joyY;
    lastSwitchSW = data.joySW;
    lastSwitchA = data.switchA;
    lastSwitchB = data.switchB;
    lastSwitchC = data.switchC;
  }

  // タイムスタンプ定義
  static unsigned long lastSend = 0;
  static unsigned long lastDebug = 0;

  // 定期送信（ジョイスティックの微小な動きを補捉）
  if (millis() - lastSend > 50) {
    sendData(data);
    lastSend = millis();
  }

  // 250msごとに詳細なセンサー値を出力
  if (millis() - lastDebug > 250) {
    Serial.printf("📊 Raw - JX:%4d JY:%4d | SW:%d A:%d B:%d C:%d\n",
                  data.joyX, data.joyY,
                  data.joySW, data.switchA, data.switchB, data.switchC);
    lastDebug = millis();
  }

  delay(10);
}
