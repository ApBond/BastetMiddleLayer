#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "stm32f303x8.h"
#include "rcc.h"
#include "string.h"

#define LEDS_COUNT 298
#define WAIT_TIME_US 1000
#define TIMER_ARR 90-1
#define PWM_LOW 26-1
#define PWM_HIGH 65-1

void ledInit(void);
uint8_t ledUpdate(void);
static void ledWait(void);
uint8_t getIdleFlag(void);
uint8_t ledSetRGB(uint16_t led,uint8_t r,uint8_t g, uint8_t b);
void buffClear(void);
uint8_t ledSetHSV(uint16_t led,uint16_t hue, uint16_t sat, uint16_t val);
uint8_t ledSetBlockRGB(uint16_t ledStart,uint16_t ledCount,uint8_t r,uint8_t g, uint8_t b);
uint8_t ledSetBlockHSV(uint16_t ledStart,uint16_t ledCount,uint16_t hue, uint16_t sat, uint16_t val);

#endif

