#include "stm32f0xx.h"


#define LCDBL_GPIO                      GPIOB
#define LCDBL_GPIOPIN                   GPIO_Pin_8       
#define LCDBL_GPIOBIT_PinSource         GPIO_PinSource8  

   

extern void LcdBlInit(void);
extern void LcdBlCtr(uint8_t  val);

