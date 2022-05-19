#include "main.h"

SemaphoreHandle_t xPduCompleteSemaphore;

extern uint8_t sbusData[BUFF_SIZE];
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

uint8_t state=0;

static uint16_t testTimer=0;

extern uint8_t flag;

void TIM6_DAC1_IRQHandler(void)
{
    if(TIM6->SR & TIM_SR_UIF)
    {
        TIM6->SR &=~TIM_SR_UIF;
        testTimer++;
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

void vPDUControlTask(void * pvParameters)
{
    PDU_Data_t data;
    PDU_Changed_Channels_t changedChannels;
    rcCommand_t command;
    uint8_t sendData[8];
    uint16_t temp;
    uint32_t tmpR;
    while(1)
    {
        sbubRead();
        
        if(xSemaphoreTake( xPduCompleteSemaphore, ( TickType_t ) 10 ) == pdTRUE)
		{
			flag=0;
			uartTransmitt(0x1);
		}
        vTaskDelay(100);
    }

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

    TaskHandle_t xHandle = NULL;
    RccClockInit();
	timer6Init();

	delayInit();
	canInit(0x011c0008);
	// demoInit();
    sbusInit(36000000,100000);
	
    //GPIOB->MODER |= GPIO_MODER_MODER0_0;
    
    //xTaskCreate(vPDUControlTask,"PDU",configMINIMAL_STACK_SIZE,NULL,4,&xHandle);

	//xPduCompleteSemaphore=xSemaphoreCreateBinary();

    //vTaskStartScheduler();
    while(1)
    {
		// if(intFlag==1)
		// {
		// 	TIM17->CR1 |= TIM_CR1_CEN;
		// 	motorStart(1);
		// 	delay_ms(10);
		// 	motorSetSpeedRpm(70,1);
		// 	delay_ms(10);
		// 	motorSetAngleDeg(90,1);
		// 	delay_ms(4000);
		// 	motorSetAngleDeg(-90,1);
		// 	delay_ms(8000);
		// 	motorSetAngleDeg(0,1);
		// 	delay_ms(4000);
		// 	motorStop(1);
		// 	TIM17->CR1 &= ~TIM_CR1_CEN;
		// 	GPIOB->BSRR = GPIO_BSRR_BR_0;
		// 	delay_ms(10);
		// 	intFlag=0;
		// }

		if(testTimer>=100)
        {
            sbubRead();
            testTimer=0;
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
						sendData[1]=0x1;
					}
					else
					{
						sendData[1]=0x0;
					}
					canWrite(sendData,2,0x01);
					changedChannels->gearChanged=0;
				}
				if(changedChannels->motionControlChanged)
				{
					temp=(int16_t)((float)(command.Vx*2048));
					sendData[0]=temp&0x00FF;
					sendData[1]=(temp>>8);
					temp = (int16_t)((float)command.Vy*2048);
					sendData[2]=temp&0x00FF;
					sendData[3]=(temp>>8);
					tmpR = (int32_t)((float)command.wz*2048);
					sendData[4]=tmpR & 0x00FF;
					sendData[5]=tmpR >> 8;
					canWrite(sendData,8,0x02);
				}
			}
			else
			{
				// sendData[0]=0x01;
				// sendData[1]=0x0;
				// canWrite(sendData,2,0x01);
			}
		}
     }
}

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}