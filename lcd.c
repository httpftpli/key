#include  "lcd.h"




static void TIM_PWM_Config(void)
{
	/* TIM1 的配置 ---------------------------------------------------
   TIM1 输入时钟(TIM1CLK) 设置为 APB2 时钟 (PCLK2)    
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock 为48 MHz */
   
  unsigned int TimerPeriod = SystemCoreClock/50000-1;
  timerPeriod = TimerPeriod;
  /*计算CCR1 跳转值 在占空比为50%时*/
  unsigned int Channel1Pulse = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10); //50%
  /* TIM1 时钟使能 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16 , ENABLE);
  
  /* Time 定时基础设置*/
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  /* Time 定时设置为上升沿计算模式*/
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;//使能频道1配置
  TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	
  /* TIM1 计算器使能*/
  TIM_Cmd(TIM1, ENABLE);
  /* TIM1 主输出使能 */
  TIM_CtrlPWMOutputs(TIM16, ENABLE);	
}

void LcdInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LCDBL_GPIOBIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LCDBL_GPIO, &GPIO_InitStructure);
    GPIO_PinAFConfig(LCDBL_GPIO, LCDBL_GPIOBIT, GPIO_AF_2); //TIM16_CH1
    TIM_PWM_Config();
    LcdBlCtr(0);
}



void LcdBlCtr(uint8_t  val) {
    TIM_SetCompare1(TIM16, TimerPeriod*100/val);
}



static unsigned short timerPeriod ;




