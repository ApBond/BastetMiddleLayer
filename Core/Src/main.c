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
		if(animationTImer>10)
		{
			animationTImer=0;
			animationLoop();
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
				if(changedChannels->mixChanged)
				{
					motorSetCommunicationMode(data.MIX,BROADCAST_ID);
					changedChannels->mixChanged=0;
				}
				if(changedChannels->elevChanged)
				{
					motorSetMotionMode(data.ELEV,BROADCAST_ID);
					changedChannels->elevChanged=0;
				}
			}
			else
			{
				//motorStop(BROADCAST_ID);
			}
		}
     }
}
