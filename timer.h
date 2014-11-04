#ifndef __TIMER__H__
#define __TIMER__H__

extern  void timerTick05msInit(void);

extern volatile unsigned int timerTick1ms;
extern volatile unsigned int timerTick05ms;


extern void timerInit(TIM_TypeDef* TIMx,unsigned short perscaler,unsigned short durationVal, unsigned short TIM_CounterMode,unsigned char isOnePulse, unsigned short intSource);
extern void timerRestart(TIM_TypeDef* TIMx);
extern void timerRst(TIM_TypeDef* TIMx);
extern void timerStop(TIM_TypeDef* TIMx);

#endif
