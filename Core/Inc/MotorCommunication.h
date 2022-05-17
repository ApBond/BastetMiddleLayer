#ifndef __MOTOR_COMMUNICATION_H
#define __MOTOR_COMMUNICATION_H

#include "main.h"


void motorStart(uint16_t motorId);
void motorStop(uint16_t motorId);
void motorSetSpeedRpm(int16_t speed,uint16_t motorId);
void motorSetAngleDeg(int16_t angle,uint16_t motorId);


#endif