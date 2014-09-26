#include "main.h"
#include "uart.h"
#include "beep.h"
#include "touch.h"
#include "keyscan.h"
#include "lcdled.h"

USART2PRA Usart2Pra;

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
    Usart2Pra.Usart2SendNum = 0;
    Usart2Pra.Usart2SendBuye = 0;
    Usart2Pra.Usart2Rxing = 0;
    Usart2Pra.Usart2RxTimeOutCount = 0;
    Usart2Pra.Usart2RxLen = 0;
    Usart2Pra.Usart2RxOK = 0;
}

void Usart2_ini(void) {
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO clock */
    RCC_AHBPeriphClockCmd(USART2_TX_GPIO_CLK | USART2_RX_GPIO_CLK, ENABLE);

    /* Enable USART clock */
    USART2_APBPERIPHCLOCK(USART2_CLK, ENABLE);

    /* Connect PXx to USARTx_Tx */
    GPIO_PinAFConfig(USART2_TX_GPIO_PORT, USART2_TX_SOURCE, USART2_TX_AF);

    /* Connect PXx to USARTx_Rx */
    GPIO_PinAFConfig(USART2_RX_GPIO_PORT, USART2_RX_SOURCE, USART2_RX_AF);

    /* Configure USART Tx and Rx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = USART2_TX_PIN;
    GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = USART2_RX_PIN;
    GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStructure);

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


    /* NVIC configuration */
    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable USART */
    USART_Cmd(USART2, ENABLE);
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    Usart2Pra.Usart2SendNum = 0;
    Usart2Pra.Usart2SendBuye = 0;
    Usart2Pra.Usart2Rxing = 0;
    Usart2Pra.Usart2RxTimeOutCount = 0;
    Usart2Pra.Usart2RxLen = 0;
    Usart2Pra.Usart2RxOK = 0;
    Usart2Pra.Usart2CmdRun = 0;

}

void CMD_SendKeyTouch(uint8_t  cmdstate) {
    if (cmdstate == SENDKEYNONLY) {
        Usart2Pra.Usart2SendBuye = 1;
        Usart2Pra.Usart2SendBuff[0] = COM_ZHENSTART;
        Usart2Pra.Usart2SendBuff[1] = CMD_KEYTOUCH;
        Usart2Pra.Usart2SendBuff[2] = KeyValue;
        Usart2Pra.Usart2SendBuff[3] = 0xff;
        Usart2Pra.Usart2SendBuff[4] = 0xff;
        Usart2Pra.Usart2SendBuff[5] = 0xff;
        Usart2Pra.Usart2SendBuff[6] = 0xff;
        Usart2Pra.Usart2SendBuff[7] = COM_ZHENEND;
    } else if (cmdstate == SENDTOUCHNONLY) {
        Usart2Pra.Usart2SendBuye = 1;
        Usart2Pra.Usart2SendBuff[0] = COM_ZHENSTART;
        Usart2Pra.Usart2SendBuff[1] = CMD_KEYTOUCH;
        Usart2Pra.Usart2SendBuff[2] = 0xff;

        Usart2Pra.Usart2SendBuff[3] = (uint8_t)(TouchPra.TouchXsample % 256);
        Usart2Pra.Usart2SendBuff[4] = (uint8_t)(TouchPra.TouchXsample / 256);
        Usart2Pra.Usart2SendBuff[5] = (uint8_t)(TouchPra.TouchYsample % 256);
        Usart2Pra.Usart2SendBuff[6] = (uint8_t)(TouchPra.TouchYsample / 256);

        /*   
           Usart2Pra.Usart2SendBuff[3]=0xAA;
           Usart2Pra.Usart2SendBuff[4]=0xAA;
           Usart2Pra.Usart2SendBuff[5]=0x55;
           Usart2Pra.Usart2SendBuff[6]=0x55;
           */
        Usart2Pra.Usart2SendBuff[7] = COM_ZHENEND;
    } else if (cmdstate == SENDTOUCHNANDKEY) {
        Usart2Pra.Usart2SendBuye = 1;
        Usart2Pra.Usart2SendBuff[0] = COM_ZHENSTART;
        Usart2Pra.Usart2SendBuff[1] = CMD_KEYTOUCH;
        Usart2Pra.Usart2SendBuff[2] = KeyValue;

        Usart2Pra.Usart2SendBuff[3] = (uint8_t)(TouchPra.TouchXsample % 256);
        Usart2Pra.Usart2SendBuff[4] = (uint8_t)(TouchPra.TouchXsample / 256);
        Usart2Pra.Usart2SendBuff[5] = (uint8_t)(TouchPra.TouchYsample % 256);
        Usart2Pra.Usart2SendBuff[6] = (uint8_t)(TouchPra.TouchYsample / 256);
        /*   
           Usart2Pra.Usart2SendBuff[3]=0xAA;
           Usart2Pra.Usart2SendBuff[4]=0xAA;
           Usart2Pra.Usart2SendBuff[5]=0x55;
           Usart2Pra.Usart2SendBuff[6]=0x55;
           */
        Usart2Pra.Usart2SendBuff[7] = COM_ZHENEND;
    }



    Usart2Pra.Usart2SendNum = 0;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
void CMD_SetAlarmMode(void) {
    BeepCtrPra.BeepMode = Usart2Pra.Usart2CmdBuff[2];
    if (BeepCtrPra.BeepMode == 0) {
        BeepOFF();
    }

}

void CMD_BeepOn(void) {
    uint16_t  i;
    i = (uint16_t)(Usart2Pra.Usart2CmdBuff[3]);
    BeepCtrPra.BeepKeyCount = i * 256 + Usart2Pra.Usart2CmdBuff[2];
}
void CMD_ReadVer(void) {
    Usart2Pra.Usart2SendBuye = 1;
    Usart2Pra.Usart2SendBuff[0] = COM_ZHENSTART;
    Usart2Pra.Usart2SendBuff[1] = CMD_READVER;
    Usart2Pra.Usart2SendBuff[2] = Ver[0];
    Usart2Pra.Usart2SendBuff[3] = Ver[1];
    Usart2Pra.Usart2SendBuff[4] = Ver[2];
    Usart2Pra.Usart2SendBuff[5] = Ver[3];
    Usart2Pra.Usart2SendBuff[6] = Ver[4];
    Usart2Pra.Usart2SendBuff[7] = COM_ZHENEND;
    Usart2Pra.Usart2SendNum = 0;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
void CMD_SetTouchMiss(void) {
    uint16_t  i;
    i = (uint16_t)(Usart2Pra.Usart2CmdBuff[3]) << 8;
    TouchPra.TouchMissCount = Usart2Pra.Usart2CmdBuff[2] + i;
}


void CMD_GoBoot(void) {
    while (1) {
    }
}

void CMD_LcdBlCtr(void) {
    if (Usart2Pra.Usart2CmdBuff[2]) {
        LcdBlOn();
    } else {
        LcdBlOff();
    }
}


///////////////////////////////////////////////////

