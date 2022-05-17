#ifndef __PDU_H
#define __PDU_H

#include "main.h"

#define BUFF_SIZE 50

#define pi 3.14159265
#define deg 57.29577951
#define CONSTRAIN 5
#define L 0.945
#define C 0.63
#define r_wheel 0.2

typedef enum 
{
    SBUS_PROCESS,
    SBUS_COMPLETE,
    SBUS_ERROR
}Sbus_state_t;

typedef struct 
{
	int course;
	int speed;
	int corstrain;
	int rotate;
	uint8_t RUDD;
	uint8_t ELEV;
	uint8_t AILE;
	uint8_t GEAR;
	uint8_t MIX;
	uint8_t FMOD;
	int AUX4;
	int AUX5;
}PDU_Data_t;

typedef struct 
{
	uint8_t motionControlChanged;
	uint8_t ruddChanged;
	uint8_t elevChanged;
	uint8_t aileChanged;
	uint8_t gearChanged;
	uint8_t mixChanged;
	uint8_t fmodChanged;
}PDU_Changed_Channels_t;

typedef struct
{
    float V;
    float R;
    float gam;
} rcCommand_t;

void sbusInit(uint32_t coreFreq, uint32_t baudRate);
void uartTransmitt(uint8_t data);
Sbus_state_t sbusParce(uint8_t data);
void decodeSbus(void);
void readPdu(void);
PDU_Data_t getPduData(void);
PDU_Changed_Channels_t* getPduChangedChannels(void);
rcCommand_t normaliz(int course, int speed, int rotate, int corstrain, uint8_t elev);
uint8_t pduGetError(void);
void sbubRead(void);

#endif