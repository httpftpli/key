#include "stm32f0xx.h"





#define   BAUDRATE  38400
#define   USART2_TX_GPIO_PORT  GPIOA
#define   USART2_RX_GPIO_PORT  GPIOA
#define   USART2_TX_SOURCE     GPIO_PinSource2
#define   USART2_RX_SOURCE     GPIO_PinSource3
#define   USART2_TX_AF         GPIO_AF_1
#define   USART2_RX_AF         GPIO_AF_1
#define   USART2_TX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define   USART2_CLK                       RCC_APB1Periph_USART2
#define   USART2_APBPERIPHCLOCK            RCC_APB1PeriphClockCmd
#define   USART2_RX_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define   USART2_TX_PIN                    GPIO_Pin_2
#define   USART2_RX_PIN                    GPIO_Pin_3
#define   USART2_TX_IE    USART_CR1_TCIE
#define   USART2_RX_IE    USART_CR1_RXNEIE



#define   USART2_COM_SHORT_NUM     8
#define   USART2_COM_LONG_NUM      32
#define   COM_ZHENSTART            0xbb
#define   COM_ZHENEND              0x0D

#define   COM_CMD_ANJIAN           0x01
#define   COM_CMD_SETALRMMODE      0x02


#define   COM_CMD_READVER          0x30
#define   USART2RXTIMEOUTMAX       100


/////////////////////////////////////////
#define   CMD_KEYTOUCH          0X01
#define   CMD_SETALRMMODE       0X02
#define   CMD_BEEPON            0X03
#define   CMD_READVER           0X04
#define   CMD_SETTOUCHMISS      0X05
#define   CMD_GOBOOT            0X06
#define   CMD_LCDBLCTR          0X07
#define   CMD_SETKEYLED         0X08




///////////////////////////////////////
#define     SENDKEYNONLY   0
#define     SENDTOUCHNONLY   1
#define     SENDTOUCHNANDKEY   2




typedef struct __usart2pra {
    uint8_t  Usart2SendBuye;
    uint8_t  Usart2SendNum;
    uint8_t  Usart2SendBuff[USART2_COM_LONG_NUM];

    //////////////////////////////
    uint8_t  Usart2Rxing;
    uint8_t  Usart2RxLen;
    uint8_t  Usart2RxOK;

    uint8_t  Usart2RxTimeOutCount;
    uint8_t  Usart2RxBuff[USART2_COM_LONG_NUM];
    uint8_t  Usart2CmdBuff[USART2_COM_LONG_NUM];
    uint8_t  Usart2CmdRun;

}USART2PRA;












extern void Usart2_Rst(void);
extern void Usart2_ini(void);

extern void CMD_SendKeyTouch(uint8_t  cmdstate);
extern void CMD_SetAlarmMode(void);
extern void CMD_BeepOn(void);
extern void CMD_ReadVer(void);
extern void CMD_SetTouchMiss(void);
extern void CMD_GoBoot(void);
extern void CMD_LcdBlCtr(void);





//extern uint8_t KeyValue;
extern USART2PRA Usart2Pra;

