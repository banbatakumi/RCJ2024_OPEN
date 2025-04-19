#ifndef __Parametors__
#define __Parametors__

#define MAIN_FREQ 2000  // Hz
#define PERIOD_US (int)((1.0f / MAIN_FREQ) * 1000000)

#define LINE_SEND_FREQ 100  // Hz
#define LINE_SEND_PERIOD_US (int)((1.0f / LINE_SEND_FREQ) * 1000000)
#define UI_SEND_FREQ 10  // Hz
#define UI_SEND_PERIOD_US (int)((1.0f / UI_SEND_FREQ) * 1000000)
#define ESP32_SEND_FREQ 500  // Hz
#define ESP32_SEND_PERIOD_US (int)((1.0f / ESP32_SEND_FREQ) * 1000000)

#define CATCH_FRONT_TH 500
#define CATCH_BACK_TH 500

#define HOLD_MAX_POWER 100
#define HOLD_WAIT_POWER 50

#define VOLTAGE_RC 0.999

#define wifi

#endif