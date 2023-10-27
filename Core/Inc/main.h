#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f303x8.h"
#include "rcc.h"
#include "can.h"
#include "pdu.h"
#include "demo.h"
#include "MotorCommunication.h"
#include "LedControl.h"
#include "animation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//Фары 15 диодов
//Бока 65

#define BACK_LIGHT_LEFT_BEGIN 0
#define BACK_LIGHT_LEFT_COUNT 15

#define BLUE_LIGHT_LEGT_BEGIN 15
#define BLUE_LIGHT_LEFT_COUNT 65

#define FORW_LIGHT_LEFT_BEGIN 80
#define FORW_LIGHT_LEFT_COUNT 15

#define FORW_LIGHT_RIGHT_BEGIN 95
#define FORW_LIGHT_RIGHT_COUNT 15

#define BLUE_LIGHT_RIGHT_BEGIN 110
#define BLUE_LIGHT_RIGHT_COUNT 65

#define BACK_LIGHT_RIGHT_BEGIN 175
#define BACK_LIGHT_RIGHT_COUNT 15

#endif