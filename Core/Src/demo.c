#include "demo.h"

#define TARGET_MOTOR 104
#define BLINK_PERIOD_MS 300

uint8_t demoState=0;
uint8_t intFlag=0;
const uint16_t stateTimeMs[3]= {4000,8000,4000};

void EXTI4_IRQHandler(void)
{
    if(EXTI->PR & EXTI_PR_PR4)
    {
        EXTI->PR|=EXTI_PR_PR4;
        if(intFlag==0)
        {
            intFlag=1;
            //TIM17->CR1 |= TIM_CR1_CEN;
            /*TIM7->ARR = (stateTimeMs[demoState]*10)-1;
            TIM17->CR1 |= TIM_CR1_CEN;
            TIM7->CR1 |= TIM_CR1_CEN;*/
        }
    }
    
}


void TIM7_IRQHandler(void)
{
    if(TIM7->SR & TIM_SR_UIF)
    {
        TIM7->SR &=~TIM_SR_UIF;
        //TIM7->CR1 &= ~TIM_CR1_CEN;
        // switch(demoState)
        // {
        //     case 1:
        //         motorSetAngleDeg(-90,TARGET_MOTOR);
        //         //motorSetSpeedRpm(-70,TARGET_MOTOR);
        //     break;
        //     case 2:
        //         motorSetAngleDeg(0,TARGET_MOTOR);
        //     break;
        //     case 3:
        //         motorStop(TARGET_MOTOR);
        //         //motorSetSpeedRpm(0,TARGET_MOTOR);
        //     break;
        // }
        if(demoState==3)
        {
            demoState=0;
            TIM17->CR1 &= ~TIM_CR1_CEN;
            GPIOB->BSRR = GPIO_BSRR_BR_0;
        }
        else
        {
            intFlag=1;
            TIM7->ARR = (stateTimeMs[demoState]*10)-1;
            demoState++;
            TIM7->CR1 |= TIM_CR1_CEN;
        }

    }
}

void TIM1_TRG_COM_TIM17_IRQHandler(void)
{
    static uint8_t blinkState=0;
    if(TIM17->SR & TIM_SR_UIF)
    {
        TIM17->SR &=~TIM_SR_UIF;
        if(blinkState)
            GPIOB->BSRR = GPIO_BSRR_BS_0;
        else
            GPIOB->BSRR = GPIO_BSRR_BR_0;
        blinkState=!blinkState;
    }
}

void timer7Init(void)
{ 
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 7200-1;
    TIM7->EGR|=TIM_EGR_UG;//!!Генерация update event для запись предделителя!!
    TIM7->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM7_IRQn);
}

void timer17Init(void)
{ 
    RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
    TIM17->PSC = 7200-1;
    TIM17->ARR = (BLINK_PERIOD_MS*10/2)-1;
    TIM17->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);
    NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn,0);
}

void demoInit(void)
{
    //Button PB4
    //Indicator PB0
    GPIOB->MODER|=GPIO_MODER_MODER0_0;
    GPIOB->BSRR = GPIO_BSRR_BR_0;
	NVIC_EnableIRQ(EXTI4_IRQn);//Включить прерывание
	SYSCFG->EXTICR[1]|=SYSCFG_EXTICR2_EXTI4_PB;
	EXTI->IMR|=EXTI_IMR_IM4;
	EXTI->FTSR|=EXTI_FTSR_TR4;
    
    //timer7Init();
    timer17Init();
	__enable_irq();
}