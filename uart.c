#include "stm32f0xx.h"
#include "main.h"
#include "uart.h"
#include "beep.h"
#include "touch.h"
#include "keyscan.h"
#include "lcdbl.h"
#include "keyled.h"
#include <string.h>
#include "timer.h"


#define CMD_TIMEOUT_US      500

static bool uarttxfinish = true;
static unsigned char uarttxbuf[20];

#define INIT_CMD(buf,cmd) do{buf[0] = 0XBB;\
                  buf[1] = cmd;\
                  buf[7] = 0X0D;}while(0)



typedef enum {
    CMD_STAT_RCV_IDLE,
    CMD_STAT_RCV_START,
    CMD_STAT_RCV_SHORT,
    CMD_STAT_RCV_LONG,
    CMD_STAT_RCV_END,
    CMD_STAT_RCV_FINISH
} CMD_STAT;

typedef struct {
    unsigned int p;
    CMD_STAT stat;
    unsigned char buf[32];
}CMD;

CMD g_cmd = { .stat = CMD_STAT_RCV_IDLE, .p = 0 };




void DMA1_Channel4_5_IRQHandler(void) {
    if (DMA_GetFlagStatus(DMA1_FLAG_TC4) == SET) {
        uarttxfinish = 1;
    }
    DMA_ClearFlag(DMA1_FLAG_TC4);
    DMA_Cmd(DMA1_Channel4, DISABLE);
}



void TIM14_IRQHandler(void){
    if(TIM_GetITStatus(TIM14,TIM_IT_Update)==SET){
        TIM_ClearFlag(TIM14, TIM_FLAG_Update);
         __disable_irq();
        g_cmd.stat = CMD_STAT_RCV_IDLE;
        g_cmd.p = 0;
        __enable_irq();
    }
}



void USART2_IRQHandler(void) {
    uint16_t data = 0;
    uint32_t it = USART2->ISR;
    USART2->ICR   =  it;
    if(it & (1<<5)) {  //rxne
        data = USART_ReceiveData(USART2) & 0xff;
    }
    if(it & (1<<11)) {//Receiver timeout
    }
    switch (g_cmd.stat) {
    case CMD_STAT_RCV_IDLE:
        if (data == COM_ZHENSTART) {
            g_cmd.buf[g_cmd.p] = data;
            timerRestart(TIM14);
            g_cmd.stat = CMD_STAT_RCV_START;
            g_cmd.p++;
        }
        break;
    case CMD_STAT_RCV_START:
        timerRestart(TIM14);
        if (data >= 0x80) {
            g_cmd.buf[g_cmd.p] = data;
            __disable_irq();
            g_cmd.stat = CMD_STAT_RCV_LONG;
            g_cmd.p++;
            __enable_irq();
        } else {
            g_cmd.buf[g_cmd.p] = data;
            __disable_irq();
            g_cmd.stat = CMD_STAT_RCV_SHORT;
            g_cmd.p++;
            __enable_irq();
        }
        break;
    case CMD_STAT_RCV_SHORT:
        g_cmd.buf[g_cmd.p++] = data;
        if (g_cmd.p == 7) {
            __disable_irq();
            g_cmd.stat = CMD_STAT_RCV_END;
            __enable_irq();
        }
        timerRestart(TIM14);
        break;
    case CMD_STAT_RCV_LONG:
        g_cmd.buf[g_cmd.p++] = data;
        if (g_cmd.p == 31) {
            __disable_irq();
            g_cmd.stat = CMD_STAT_RCV_END;
            __enable_irq();
        }
        timerRestart(TIM14);
        break;
    case CMD_STAT_RCV_END:
        if (data == COM_ZHENEND) {
            timerStop(TIM14);
            g_cmd.buf[g_cmd.p] = data;
            __disable_irq();
            g_cmd.stat = CMD_STAT_RCV_FINISH;
            g_cmd.p = 0;
            __enable_irq();
        } else {
            timerStop(TIM14);
            __disable_irq();
            g_cmd.p = 0;
            g_cmd.stat = CMD_STAT_RCV_IDLE;
            __enable_irq();
        }
        break;
    case CMD_STAT_RCV_FINISH:
        break;
    default:
        __disable_irq();
        g_cmd.p = 0;
        g_cmd.stat = CMD_STAT_RCV_IDLE;
        __enable_irq();
        timerStop(TIM14);
        break;
    }
}



void Usart2_Rst(void) {
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}




static DMA_InitTypeDef   dmaChUartTxdef;

void Usart2_ini(void) {
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(USART2_TX_GPIO_CLK | USART2_RX_GPIO_CLK, ENABLE);

    /* Enable USART clock */
    USART2_APBPERIPHCLOCK(USART2_CLK, ENABLE);

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    /* Configure USART Tx and Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
    GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
    GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);
    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(USART2_TX_GPIO_PORT, USART2_TX_SOURCE, USART2_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(USART2_RX_GPIO_PORT, USART2_RX_SOURCE, USART2_RX_AF);

    /* USARTx configuration ----------------------------------------------------*/
    /* USARTx configured as follow:
    - BaudRate = 38400 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    USART_InitStructure.USART_BaudRate = BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    //USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

    /* NVIC configuration */
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DMA_StructInit(&dmaChUartTxdef);
    dmaChUartTxdef.DMA_MemoryBaseAddr = (unsigned int)uarttxbuf;
    dmaChUartTxdef.DMA_PeripheralBaseAddr = (unsigned int)(&USART2->TDR);
    dmaChUartTxdef.DMA_DIR = DMA_DIR_PeripheralDST;
    dmaChUartTxdef.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dmaChUartTxdef.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dmaChUartTxdef.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dmaChUartTxdef.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dmaChUartTxdef.DMA_Mode = DMA_Mode_Normal;
    dmaChUartTxdef.DMA_Priority = DMA_Priority_Medium;
    dmaChUartTxdef.DMA_M2M = DMA_M2M_Disable;

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
    timerInit(TIM14, SystemCoreClock / 1000000, CMD_TIMEOUT_US, TIM_CounterMode_Up, 1, TIM_IT_Update);
    NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}




void uartSend(USART_TypeDef *USARTx, void *buf, unsigned int len) {
    if (len == 0) {
        return;
    }
    while (!uarttxfinish);
    uarttxfinish = 0;
    dmaChUartTxdef.DMA_BufferSize = len;
    memcpy(uarttxbuf, buf, len);
    DMA_Init(DMA1_Channel4, &dmaChUartTxdef);
    DMA_Cmd(DMA1_Channel4, ENABLE);
}




void CMD_SendKeyTouch(unsigned char keycode, unsigned short touchx, unsigned short touchy) {
    unsigned char buf[8];
    INIT_CMD(buf,CMD_KEYTOUCH);
    buf[2] = keycode;
    buf[3] = touchx & 0xff;
    buf[4] = touchx>>8;
    buf[5] = touchy & 0xff;
    buf[6] = touchy>>8;
    uartSend(USART2, buf, 8);
}



void CMD_SetAlarmMode(void) {

}

void CMD_BeepOn(unsigned short val) {
     beep(val);
}


void CMD_ReadVer(void) {

}


void CMD_SetTouchMiss(unsigned char val) {
    setTouchAccur(val)  ;
}


void CMD_GoBoot(void) {

}

void CMD_LcdBlCtr(uint8_t  val) {
    LcdBlCtr(val);
}


void CMD_SetKeyLed_bitmap(unsigned short val){
    for (int i=0;i<16;i++) {
        led[i].on = !(val & i<<i);
        led[i].func = ONOFF;
    }
    keyLedOn_bitmap(val); 
}


void CMD_SetKeyLed(unsigned char iled,unsigned char funcode,unsigned short ontime_ms,unsigned short offtime_ms){
    if (iled>16) {
        return;
    }
    led[iled].func = (enum LEDFUN)funcode;
    led[iled].offtimeval = 0;
    led[iled].ontimeval = 0;
    led[iled].ontime = ontime_ms;
    led[iled].offtime = offtime_ms;
    led[iled].stat = 1;
    ledstat = 1;
}



void cmdProcess() {
    if (g_cmd.stat != CMD_STAT_RCV_FINISH) return;
    g_cmd.stat = CMD_STAT_RCV_IDLE;
    switch (g_cmd.buf[1]) {
    case CMD_SETALRMMODE:
        break;
    case CMD_BEEPON:
        CMD_BeepOn((g_cmd.buf[3] << 8) | g_cmd.buf[2]);
        break;
    case CMD_READVER:
        CMD_ReadVer();
        break;
    case CMD_SETTOUCHMISS:
        CMD_SetTouchMiss(g_cmd.buf[2]);
        break;
    case CMD_GOBOOT:
        CMD_GoBoot();
        break;
    case CMD_LCDBLCTR:
        CMD_LcdBlCtr(g_cmd.buf[2]);
        break;
    case CMD_SETKEYLED_BITMAP:
        CMD_SetKeyLed_bitmap((g_cmd.buf[3] << 8) | g_cmd.buf[2]);
        break;
    case CMD_SETKEYLED:
        CMD_SetKeyLed(g_cmd.buf[3]&0x0f,g_cmd.buf[3]>>4,
                      g_cmd.buf[4]&g_cmd.buf[5]<<8,
                      g_cmd.buf[6]&g_cmd.buf[7]<<8);
        break;
    default:
        break;
    }
}



///////////////////////////////////////////////////

