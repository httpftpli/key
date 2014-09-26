#include "stm32f0xx.h"

#define LCDBL_GPIO           GPIOB
#define LCDBL_GPIO_GPIOBIT         GPIO_Pin_8

extern void LcdBlInit(void);
extern void LcdBlOn(void);
extern void LcdBlOff(void);
