
#include "stm32f0xx.h"


volatile unsigned int timerTick1ms = 0;
volatile unsigned int timerTick05ms = 0;


//extern unsigned int   SystemCoreClock  ;
void timerTick05msInit(void) { //timer for 0.5 ms interrupt
    SysTick_Config(SystemCoreClock / 2000);
}


void timerInit(TIM_TypeDef* TIMx,unsigned short perscaler,unsigned short durationVal, unsigned short TIM_CounterMode,unsigned char isOnePulse, unsigned short intSource){
  unsigned int RCC_APBPeriph_TIMx;
  unsigned int temp = (unsigned int)TIMx;
  switch(temp){
  case (unsigned int)TIM2:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM2;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM3:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM3;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM6:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM6;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case (unsigned int)TIM14:
    RCC_APBPeriph_TIMx = RCC_APB1Periph_TIM14;
    RCC_APB1PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM1:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM1;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM15:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM15;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM16:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM16;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  case  (unsigned int)TIM17:
    RCC_APBPeriph_TIMx = RCC_APB2Periph_TIM17;
    RCC_APB2PeriphClockCmd(RCC_APBPeriph_TIMx, ENABLE);
    break;
  default:
    return;
  }

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Prescaler = perscaler-1;
  TIM_TimeBaseStructure.TIM_Period = durationVal;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode;
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
  TIM_SelectOnePulseMode(TIMx, (isOnePulse==1)?TIM_OPMode_Single :TIM_OPMode_Repetitive);
  if(intSource!=0){
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
  }
}


void timerRestart(TIM_TypeDef* TIMx){
    TIM_Cmd(TIMx, DISABLE);
    TIM_SetCounter(TIMx, 0);
    TIM_Cmd(TIMx, ENABLE);
}



void timerRst(TIM_TypeDef* TIMx){
    TIM_SetCounter(TIMx, 0);
}

void timerStop(TIM_TypeDef* TIMx){
    TIM_Cmd(TIMx, DISABLE);
}







