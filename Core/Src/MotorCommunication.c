#include "MotorCommunication.h"


void motorStart(uint16_t motorId)
{
    uint8_t data[2];
    data[0]=0x1;
    data[1]=0x1;
    canWrite(data,2,motorId);
}

void motorStop(uint16_t motorId)
{
    uint8_t data[2];
    data[0]=0x1;
    data[1]=0x0;
    canWrite(data,2,motorId);
}

void motorSetSpeedRpm(int16_t speed,uint16_t motorId)
{
    char data[3];
    data[0] = 0x03;
    data[1] = speed & 0xFF;
    data[2] = speed >> 8;
    canWrite(data,3,motorId);
}

void motorSetAngleDeg(int16_t angle,uint16_t motorId)
{
    uint8_t  data[3];
    data[0] = 0x05;
    data[1] = angle & 0xFF;
    data[2] = angle >> 8;
    canWrite(data,3,motorId);
}
