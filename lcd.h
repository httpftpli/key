#include "stm32f0xx.h"


#define LCDBL_GPIO     GPIOB
#define LCDBL_GPIOBIT  GPIO_Pin_8


extern void LcdInit(void);
extern void LcdBlCtr(uint8_t  val);

