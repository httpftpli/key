#include "stm32f0xx.h"
#include <stdbool.h>
#include "main.h"


#define ADS7843_INT_GPIO            GPIOB
#define ADS7843_INT_GPIOBIT         GPIO_Pin_0
#define ADS7843_BUYE_GPIO           GPIOA
#define ADS7843_BUYE_GPIOBIT        GPIO_Pin_0


#define ADS7843_CS_GPIO             GPIOA
#define ADS7843_CS_GPIOBIT          GPIO_Pin_4
#define ADS7843_PORT                GPIOA
#define ADS7843_SLK_PIN             GPIO_Pin_5
#define ADS7843_MISO_PIN            GPIO_Pin_6
#define ADS7843_MOSI_PIN            GPIO_Pin_7

#define ADS7843_SLK_PINSOC            GPIO_PinSource5
#define ADS7843_MISO_PINSOC           GPIO_PinSource6
#define ADS7843_MOSI_PINSOC           GPIO_PinSource7


#define ADS7843_CTRL_Y_SAMPLE       0xd0   //0b11010000
#define ADS7843_CTRL_X_SAMPLE       0x90   //0b10010000

#define  TOUCHMISSDEFUALT    12
#define TOUCH_SAMPLE_NUM   9




extern unsigned int touchAccurMargin;
extern unsigned short touchpointX ;
extern unsigned short touchpointY ;
extern bool touched;
extern void TouchInit(void);
extern void TouchScan(void);
extern void  setTouchAccur(unsigned char val);

