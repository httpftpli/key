#include "stm32f0xx.h"


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

#define  TOUCHINTMAX  30                  //30MS滤波 用手  轻触效果用这个调节
#define  TOUCHCONTINUOUSMAX   70          //70M连击

#define  TOUCHMISSDEFUALT    30  
#define  TOUCHSCANTIME      3           //出现无效数据后重读间隔

#define lenthof(array) (sizeof((array))/sizeof((array[0])))


typedef struct __touchpra {

    uint8_t  TouchIntCount;   //INT脚滤波计数器
    uint8_t  TouchIntState;
    uint8_t  TouchState;
    uint8_t  TouchScanCount;
    uint16_t  TouchContinuousCount;
    uint8_t  TouchDataState;
    uint16_t  TouchXsample;
    uint16_t  TouchYsample;
    uint16_t  TouchMissCount;

}TOUCHPRA;



void SpiCsEnable(void);
void SpiCsDisable(void);
void SpiSend(uint8_t *buf, uint8_t Length);
void SpiReceive(uint8_t *buf, uint8_t length);
uint8_t GetTouchSample(void);
uint16_t TouchXsample(void);
uint16_t TouchXsample(void);
uint16_t sum(uint16_t *buf, uint16_t nNum);
void bubbleSortAscend(uint16_t *buf, uint16_t nNum);
uint8_t   GetTouchBuyeVaule(void);



extern void TouchInit(void);
extern uint8_t   GetTouchIntVaule(void);
extern void TouchScan(void);
extern TOUCHPRA TouchPra;

