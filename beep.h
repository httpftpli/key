#include "stm32f0xx.h"

/////////////////////////////
#define BEEPOUT_GPIO     GPIOA
#define BEEPOUT_GPIOBIT  GPIO_Pin_1
#define BEEP_ON  Bit_SET
#define BEEP_OFF  Bit_RESET  
#define BEEP_GPIO_CLK               RCC_AHBPeriph_GPIOA
   
#define  KEY_BEEP_TIME     100           //按键响时间100MS
#define  ALARM1_BEEP_TIME   50           //报警模式1响50MS
#define  ALARM1_BEEPOFF_SHORT_TIME  50   //报警模式1短停50MS
#define  ALARM1_BEEPOFF_LONG_TIME  1000  //报警模式1长停1S  

#define  ALARM2_BEEP_TIME   500          //报警模式2 响500MS
#define  ALARM2_BEEPOFF_TIME  500        //报警模式2停500MS




typedef struct __beepctrpra{
  uint8_t  BeepMode;          // 0表示按键模式，1表示蜂鸣器警报模式 1状态 2:表示2状态.....
  uint8_t  Beep_KeyOffEn;     //
  uint16_t BeepKeyCount;     //
 
}BEEPCTRPRA;

typedef struct __alarm1beepctrcra{
  uint8_t  AlarmBeepState;
  uint16_t AlarmBeepOnCount;
  uint16_t AlarmBeepOffShortCount;   
  uint16_t AlarmBeepOffLongCount;
}AlARM1BEECPTRPRA;

typedef struct __alarm2beepctrcra{
  uint8_t  AlarmBeepState;
  uint16_t AlarmBeepOnCount;
  uint16_t AlarmBeepOffCount;
}AlARM2BEECPTRPRA;


void beepInit(void);
void BeepON(void);
void BeepOFF(void); 
extern void Alarm1BeepCtr(void);
extern void Alarm2BeepCtr(void);


extern BEEPCTRPRA	BeepCtrPra;
extern AlARM1BEECPTRPRA Alarm1BeepCtrPra;
extern AlARM2BEECPTRPRA Alarm2BeepCtrPra;


 

