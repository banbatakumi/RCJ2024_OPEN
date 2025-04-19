#include "oled.h"

Oled::Oled() : oled(U8G2_R0, /* reset=*/U8X8_PIN_NONE) {}

void Oled::Init() {
      oled.begin();
      oled.setPowerSave(0);
      oled.setFont(u8g2_font_courR10_tr);
}