#ifndef ANIMATION_H
#define ANIMATION_H

#include "stm32f303x8.h"
#include "LedControl.h"

void animationInit(void);
void animationLoop(void);
static void rangeIndication(uint16_t ledStart,uint16_t ledNum, uint16_t range,uint16_t rangeMax,uint16_t rangeMin);


#endif

