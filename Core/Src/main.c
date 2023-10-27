#include "main.h"


extern uint8_t sbusData[BUFF_SIZE];

uint8_t state=0;

static uint16_t testTimer=0;
static uint16_t animationTImer=0;

extern uint8_t flag;

void TIM6_DAC1_IRQHandler(void)
{
    if(TIM6->SR & TIM_SR_UIF)
    {
        TIM6->SR &=~TIM_SR_UIF;
        testTimer++;
		animationTImer++;
    }
}

void timer6Init(void)
{ 
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 7200-1;
    TIM6->ARR = 10-1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM6_DAC1_IRQn);
}

extern uint8_t demoState;
extern uint8_t intFlag;

int main(void)
{
    PDU_Data_t data;
    PDU_Changed_Channels_t* changedChannels;
    rcCommand_t command;
    uint8_t sendData[8];
    uint16_t temp;
    uint32_t tmpR;
	uint8_t lightAnimation=0;
	uint8_t lightDir=0;
    RccClockInit();
	timer6Init();

	delayInit();
	canInit(0x011c0008);
    sbusInit(36000000,100000);
	ledInit();
    while(1)
    {
		
		if(testTimer>=100)
        {
            sbubRead();
            testTimer=0;
        }
		if(animationTImer>5)
		{
			animationTImer=0;
			if(data.MIX==1)
			{
				ledSetBlockRGB(BACK_LIGHT_LEFT_BEGIN,BACK_LIGHT_LEFT_COUNT,255,0,0);
				ledSetBlockRGB(BLUE_LIGHT_LEGT_BEGIN,BLUE_LIGHT_LEFT_COUNT,0,0,255);
				ledSetBlockRGB(FORW_LIGHT_LEFT_BEGIN,FORW_LIGHT_LEFT_COUNT,255,255,255);
				ledSetBlockRGB(FORW_LIGHT_RIGHT_BEGIN,FORW_LIGHT_RIGHT_COUNT,255,255,255);
				ledSetBlockRGB(BLUE_LIGHT_RIGHT_BEGIN,BLUE_LIGHT_RIGHT_COUNT,0,0,255);
				ledSetBlockRGB(BACK_LIGHT_RIGHT_BEGIN,BACK_LIGHT_RIGHT_COUNT,255,0,0);
			}
			else if(data.MIX==0)
			{
				ledSetBlockRGB(0,LEDS_COUNT,0,0,0);	
			}
			else if(data.MIX==2)
			{
				
				if(data.AILE==1)
				{
					animationLoop();
				}
				else
				{
					
					if(lightAnimation==255)
					{
						lightDir=1;
					}
					else if(lightAnimation==0)
					{
						lightDir=0;
					}
					if(lightDir)
						lightAnimation--;
					else
						lightAnimation++;

					ledSetBlockRGB(BACK_LIGHT_LEFT_BEGIN,BACK_LIGHT_LEFT_COUNT,255,0,0);
					ledSetBlockRGB(FORW_LIGHT_LEFT_BEGIN,FORW_LIGHT_LEFT_COUNT,255,255,255);
					ledSetBlockRGB(FORW_LIGHT_RIGHT_BEGIN,FORW_LIGHT_RIGHT_COUNT,255,255,255);
					ledSetBlockRGB(BACK_LIGHT_RIGHT_BEGIN,BACK_LIGHT_RIGHT_COUNT,255,0,0);
					ledSetBlockRGB(BLUE_LIGHT_LEGT_BEGIN,BLUE_LIGHT_LEFT_COUNT,0,0,lightAnimation);
					ledSetBlockRGB(BLUE_LIGHT_RIGHT_BEGIN,BLUE_LIGHT_RIGHT_COUNT,0,0,lightAnimation);
				}
			}
			ledUpdate();
		 	
		} 
		if(flag==1)
		{
			flag=0;
			if(!pduGetError())
			{
				decodeSbus();
				readPdu();
			    data=getPduData();
			    changedChannels=getPduChangedChannels();
			    command=normaliz(data.course,data.speed,data.rotate,data.corstrain,data.ELEV);
			    if(changedChannels->gearChanged)
				{
					sendData[0]=0x01;
					if(data.GEAR)
					{
						motorStart(BROADCAST_ID);
					}
					else
					{
						motorStop(BROADCAST_ID);
					}
					changedChannels->gearChanged=0;
				}
				if(changedChannels->motionControlChanged)
				{
					temp=(int16_t)((float)(command.V*2048));
					sendData[0]=temp&0x00FF;
					sendData[1]=(temp>>8);
					temp = (int16_t)((float)command.gam*2048);
					sendData[2]=temp&0x00FF;
					sendData[3]=(temp>>8);
					tmpR = (int32_t)((float)command.R*65535);
					sendData[4]=tmpR & 0x000000FF;
					sendData[5]=(tmpR & 0x0000FF00)>>8;
					sendData[6]=(tmpR & 0x00FF0000)>>16;
					sendData[7]=(tmpR & 0xFF000000)>>24;
					canWrite(sendData,8,0x02);
					changedChannels->motionControlChanged=0;
				}
				// if(changedChannels->mixChanged)
				// {
				// 	motorSetCommunicationMode(data.MIX,BROADCAST_ID);
				// 	changedChannels->mixChanged=0;
				// }
				if(changedChannels->elevChanged)
				{
					motorSetMotionMode(data.ELEV,BROADCAST_ID);
					changedChannels->elevChanged=0;
				}
				if(changedChannels->mixChanged)
				{
					lightAnimation=255;
					changedChannels->mixChanged=0;
				}
			}
			else
			{
				//motorStop(BROADCAST_ID);
			}
		}
     }
}