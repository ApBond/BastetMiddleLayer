#ifndef __MOTOR_COMMUNICATION_H
#define __MOTOR_COMMUNICATION_H

#include "main.h"

#define BROADCAST_ID 1
#define SET_VECTOR_ID 2

#define ERROR_CHANNEL_ID 3

#define ALL_RESET 0x00
#define START_STOP_DRIVE 0x01
#define CHANGE_MODE 0x02
#define CHANGE_SPEED 0x03
#define CHANGE_TORQUE 0x04
#define CHANGE_ANGLE 0x05
#define CHANGE_DEVICE_COMMUNICATION_MODE 0x06
#define CHANGE_MOTION_CONTROL_MODE 0x07


#define MOTOR_START 0x01
#define MOTOR_STOP 0x00

#define TORQUE_MODE 0x00
#define SPEED_MODE 0x01

#define NO_SEND_DATA 0x00
#define SEND_DEBUG_DATA 0x01
#define SEND_ODOM_DATA 0x02

typedef enum
{
    NO_DATA,
    DEBUG,
    ODOM
}Communication_mode_t;

typedef enum
{
    PDU_CONTROL,
    ROS_CONTROL
}Motion_control_mode_t;

void motorStart(uint16_t motorId);
void motorStop(uint16_t motorId);
void motorSetSpeedRpm(int16_t speed,uint16_t motorId);
void motorSetAngleDeg(int16_t angle,uint16_t motorId);
void motorSetCommunicationMode(Communication_mode_t mode,uint16_t motorId);
void motorSetMotionMode(Motion_control_mode_t mode,uint16_t motorId);

#endif