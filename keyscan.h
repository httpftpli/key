#include "stm32f0xx.h"
#include <stdbool.h>


#define  NUM_KEY_IN     8
#define  KEY_D0_IN_PORT  GPIOA
#define  KEY_D1_IN_PORT  GPIOA
#define  KEY_D2_IN_PORT  GPIOB
#define  KEY_D3_IN_PORT  GPIOB
#define  KEY_D4_IN_PORT  GPIOC
#define  KEY_D5_IN_PORT  GPIOC
#define  KEY_D6_IN_PORT  GPIOB
#define  KEY_D7_IN_PORT  GPIOC


#define KEY_D0  GPIO_Pin_12    //GPIOA
#define KEY_D1  GPIO_Pin_8     //GPIOA
#define KEY_D2  GPIO_Pin_11    //GPIOB
#define KEY_D3  GPIO_Pin_10    //GPIOB
#define KEY_D4  GPIO_Pin_15    //GPIOB
#define KEY_D5  GPIO_Pin_14    //GPIOB
#define KEY_D6  GPIO_Pin_9     //GPIOB
#define KEY_D7  GPIO_Pin_13     //GPIOC

#define  NUM_KEY_OUT     8
#define  KEY_OUT0_PORT  GPIOA
#define  KEY_OUT1_PORT  GPIOB
#define  KEY_OUT2_PORT  GPIOB
#define  KEY_OUT3_PORT  GPIOB
#define  KEY_OUT4_PORT  GPIOB
#define  KEY_OUT5_PORT  GPIOB
#define  KEY_OUT6_PORT  GPIOB
#define  KEY_OUT7_PORT  GPIOB

#define KEY_OUT0  GPIO_Pin_15    //GPIOA
#define KEY_OUT1  GPIO_Pin_3    //GPIOB
#define KEY_OUT2  GPIO_Pin_4    //GPIOB
#define KEY_OUT3  GPIO_Pin_5    //GPIOB
#define KEY_OUT4  GPIO_Pin_6    //GPIOB
#define KEY_OUT5  GPIO_Pin_7    //GPIOB
#define KEY_OUT6  GPIO_Pin_2    //GPIOB
#define KEY_OUT7  GPIO_Pin_1    //GPIOB


#define  KEYSCANTIME   10         //键扫描间隔 10MS
#define  KEYCONTINUOUSTIME      2000  //连续按键2秒 认为开始连击
#define  KEYRUNCONTINUOUSTIME   150  //连击有效间隔


//允许连击的 键个数
#define  NUM_KeyContinuous        6
//允许连击的 键值
#define  KeyContinuous_UP        0x19
#define  KeyContinuous_DOWN      0x15
#define  KeyContinuous_LEFT      0x1b
#define  KeyContinuous_RIGHT     0x1d
#define  KeyContinuous_PnUP      0x23
#define  KeyContinuous_PnDOWN    0x24








extern unsigned char keycode;
extern bool keytouched;

void Keyscandelay(uint16_t n);
void keyScanOutputReset(void);
void keyScanInit(void);
unsigned char keyScan(void);

extern uint8_t KeyValue;
extern uint8_t KeyScanCount;          //扫描时间计数器 1个1MS
extern uint16_t KeyContinuousCount;    //连击时间计数器 1个1MS
extern uint16_t KeyRunContinuousCount;


