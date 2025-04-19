#include <Arduino.h>
#include "speedFunctionsESP32.h"

void high(unsigned char pin) {
    if(pin < 32) {
        *((volatile unsigned long *)GPIO_OUT_W1TS_REG) |= 1<<pin;
    } else {
        *((volatile unsigned long *)GPIO_OUT1_W1TS_REG) |= 1<<pin;
    }
}

void low(unsigned char pin) {
    if(pin < 32) {
        *((volatile unsigned long *)GPIO_OUT_W1TC_REG) |= 1<<pin;
    } else {
        *((volatile unsigned long *)GPIO_OUT1_W1TC_REG) |= 1<<pin;
    }
}