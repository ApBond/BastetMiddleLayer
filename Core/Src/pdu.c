#include "pdu.h"

const int courseZeroPoint = 1026;
const int speedZeroPoint = 1006;
const int corstrainZeroPoint = 342;
const int rotateZeroPoint = 1024;
const int twoPositionSwitchZeroLevel = 342;
const int twoPositionSwitchOneLevel = 1706;
const int threePositionSwitchOneLevel = 342;
const int threePositionSwitchTwoLevel = 684;
const int threePositionSwitchThreeLevel = 1706;

const int rotateMaxPoint = 1706;
const int speedMaxPoint = 1706;
const int courseMaxPoint = 1706;
const int corstrainMaxPoint = 1706;

const float baseLenght = 0.94;
const int maxSpeed = 3;
const float maxRotate = 0.78539816;
const float maxCourse = 0.78539816*2;

uint8_t sbusData[BUFF_SIZE];
uint8_t payload[24];
static uint16_t pduChannels[16];
static PDU_Data_t pduData;
static PDU_Changed_Channels_t changedChannels;
uint8_t pduErrorState=1;
uint8_t flag=0;

double sign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

int16_t intSign(double a){
	if (a>0) return 1;
	else if (a<0) return -1;
	else return 0;
}

void USART2_IRQHandler(void)
{
	uint8_t data;
	static uint8_t i=0;
	Sbus_state_t state;
	//BaseType_t xHigherPriorityTaskWoken=pdFALSE;
	if(USART2->ISR & USART_ISR_RXNE)
	{
		data=USART2->RDR;
		state=sbusParce(data);
		if(state==SBUS_COMPLETE || state==SBUS_ERROR)
		{
			USART2->CR1&=~USART_CR1_UE;
			flag=1;
		}	
	}
}

void sbubRead(void)
{
	USART2->CR1|=USART_CR1_UE;
	
}

void sbusInit(uint32_t coreFreq, uint32_t baudRate)
{
	RCC->APB1ENR|=RCC_APB1ENR_USART2EN;//Включить тактирование  
    //RCC->CFGR3 |= RCC_CFGR3_USART1SW_0;
	GPIOA->MODER|= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1;//PA10 в режим альтернативной функции
	GPIOA->AFR[0]|= 7<<8 | 7<<12;//Включить AF7
	USART2->CR1 = 0;//Сбрасываем конфигурацию
	USART2->CR1|=USART_CR1_RE | USART_CR1_TE | USART_CR1_M0;//Включить приём и передачу
    USART2->CR1|=USART_CR1_PCE;//Enable even parity
    USART2->CR2 |= USART_CR2_STOP_1;//2 stop bits
    USART2->CR2 |= USART_CR2_RXINV | USART_CR2_TXINV;//Инвертировать данные
	USART2->BRR|=coreFreq/(baudRate);//Скорость работы uart
	USART2->CR1|=USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn,6);
	//USART2->CR1|=USART_CR1_UE;//Включить uart
}



void uartTransmitt(uint8_t data)
{
	USART2->TDR = data;//Отправляем данные
}

Sbus_state_t sbusParce(uint8_t data)
{
	static uint8_t prevByte=0;
	static int parserState=-1;
	static uint16_t errorCounter=0;
	if(data==0x0F && prevByte==0x00)
	{
		parserState++;
	}
	else if(parserState!=-1)
	{
		payload[parserState]=data;
		parserState++;
		if(parserState==24)
		{
			parserState=-1;
			if(data==0x00 && prevByte==0x00)
			{
				pduErrorState=0;
				return SBUS_COMPLETE;
			}
			else
			{
				pduErrorState=1;
				return SBUS_ERROR;	
			}
		}
	}
	prevByte=data;
	return SBUS_PROCESS;
}

void decodeSbus(void)
{
	pduChannels[0]  = (uint16_t) ((payload[0]    |payload[1] <<8)                     & 0x07FF);
	pduChannels[1]  = (uint16_t) ((payload[1]>>3 |payload[2] <<5)                     & 0x07FF);
	pduChannels[2]  = (uint16_t) ((payload[2]>>6 |payload[3] <<2 |payload[4]<<10)    & 0x07FF);
	pduChannels[3]  = (uint16_t) ((payload[4]>>1 |payload[5] <<7)                     & 0x07FF);
	pduChannels[4]  = (uint16_t) ((payload[5]>>4 |payload[6] <<4)                     & 0x07FF);
	pduChannels[5]  = (uint16_t) ((payload[6]>>7 |payload[7] <<1 |payload[8]<<9)     & 0x07FF);
	pduChannels[6]  = (uint16_t) ((payload[8]>>2 |payload[9] <<6)                     & 0x07FF);
	pduChannels[7]  = (uint16_t) ((payload[9]>>5 |payload[10]<<3)                     & 0x07FF);
	pduChannels[8]  = (uint16_t) ((payload[11]   |payload[12]<<8)                     & 0x07FF);
	pduChannels[9]  = (uint16_t) ((payload[12]>>3|payload[13]<<5)                     & 0x07FF);
	pduChannels[10] = (uint16_t) ((payload[13]>>6|payload[14]<<2 |payload[15]<<10)   & 0x07FF);
	pduChannels[11] = (uint16_t) ((payload[15]>>1|payload[16]<<7)                     & 0x07FF);
	pduChannels[12] = (uint16_t) ((payload[16]>>4|payload[17]<<4)                     & 0x07FF);
	pduChannels[13] = (uint16_t) ((payload[17]>>7|payload[18]<<1 |payload[19]<<9)    & 0x07FF);
	pduChannels[14] = (uint16_t) ((payload[19]>>2|payload[20]<<6)                     & 0x07FF);
	pduChannels[15] = (uint16_t) ((payload[20]>>5|payload[21]<<3)                     & 0x07FF);


}

void readPdu(void)
{
	PDU_Data_t temp=pduData;
	pduData.course = pduChannels[0]-courseZeroPoint;
	pduData.speed = pduChannels[1]-speedZeroPoint;
	pduData.corstrain = pduChannels[2]-corstrainZeroPoint;
	pduData.rotate = pduChannels[3]-rotateZeroPoint;
	if(pduData.course!=temp.course || pduData.speed!=temp.speed || pduData.corstrain!=temp.corstrain || pduData.rotate!=temp.rotate)
		changedChannels.motionControlChanged=1;

	pduData.RUDD = (pduChannels[4]==twoPositionSwitchOneLevel) ? 1 : 0;
	if(pduData.RUDD!=temp.RUDD)
		changedChannels.ruddChanged=1;
	pduData.ELEV = (pduChannels[5]==twoPositionSwitchOneLevel) ? 1 : 0;
	if(pduData.ELEV!=temp.ELEV)
		changedChannels.elevChanged=1;
	pduData.AILE = (pduChannels[6]==twoPositionSwitchOneLevel) ? 1 : 0;
	if(pduData.AILE!=temp.AILE)
		changedChannels.aileChanged=1;
	pduData.GEAR = (pduChannels[7]==twoPositionSwitchOneLevel) ? 1 : 0;
	if(pduData.GEAR!=temp.GEAR)
		changedChannels.gearChanged=1;

	pduData.MIX = (pduChannels[8]==threePositionSwitchThreeLevel) ? 2 : ((pduChannels[8]==threePositionSwitchTwoLevel) ? 1 : 0);
	if(pduData.MIX!=temp.MIX)
		changedChannels.mixChanged=1;
	pduData.FMOD = (pduChannels[9]==threePositionSwitchThreeLevel) ? 2 : ((pduChannels[9]==threePositionSwitchTwoLevel) ? 1 : 0);
	if(pduData.GEAR!=temp.GEAR)
		changedChannels.gearChanged=1;
}

rcCommand_t normaliz(int course, int speed, int rotate, int corstrain, uint8_t elev)
{
	float generalGam;
	float vel_coef;
	rcCommand_t command;
	if (fabs(course)<20) course=0;
	if (fabs(speed)<20) speed=0;
	if (fabs(rotate)<20) rotate=0;
	if (fabs(corstrain)<20) corstrain=0;

	generalGam=rotate*maxRotate/(rotateMaxPoint-rotateZeroPoint);
	command.R=baseLenght/2/tan(generalGam);
	if (fabs(command.R)<0.1) command.R=0.1*sign(command.R);//минимальный радиус поворота
	if (fabs(command.R)>50) command.R=16000;

	command.V=(speed)/((float)(speedMaxPoint-speedZeroPoint));
	vel_coef=(corstrain/((float)(corstrainMaxPoint-corstrainZeroPoint)));
	command.V=command.V*vel_coef*maxSpeed;

	command.gam=(course*maxCourse)/((float)(courseMaxPoint-courseZeroPoint));
	if (fabs(command.gam)<0.01) command.gam=0;
	if (command.gam!=0) command.R=16000;
	return command;
}

PDU_Data_t getPduData(void)
{
	return pduData;
}
PDU_Changed_Channels_t* getPduChangedChannels(void)
{
	return &changedChannels;
}

uint8_t pduGetError(void)
{
	return pduErrorState;
}

