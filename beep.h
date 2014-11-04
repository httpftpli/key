#include "stm32f0xx.h"

/////////////////////////////
#define BEEPOUT_GPIO     GPIOA
#define BEEPOUT_GPIOBIT  GPIO_Pin_1
#define BEEP_GPIO_CLK               RCC_AHBPeriph_GPIOA
   
#define  KEY_BEEP_TIME     100           //按键响时间100MS
#define  ALARM1_BEEP_TIME   50           //报警模式1响50MS
#define  ALARM1_BEEPOFF_SHORT_TIME  50   //报警模式1短停50MS
#define  ALARM1_BEEPOFF_LONG_TIME  1000  //报警模式1长停1S  

#define  ALARM2_BEEP_TIME   500          //报警模式2 响500MS
#define  ALARM2_BEEPOFF_TIME  500        //报警模式2停500MS



extern void beepInit(void);
extern void beep(unsigned int dur);



 

