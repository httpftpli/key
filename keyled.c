#include "stm32f0xx.h"
#include "main.h"
#include "timer.h"
#include "keyled.h"


#define SPI_GPIO_LED_CS           GPIOB
#define SPI_GPIO_LED_CLK          GPIOB
#define SPI_GPIO_LED_DATA         GPIOB

#define SPI_GPIOPIN_LED_CS        GPIO_Pin_12
#define SPI_GPIOPIN_LED_CLK       GPIO_Pin_13
#define SPI_GPIOPIN_LED_DATA      GPIO_Pin_15


#define SPI_GPIOPIN_LED_CS_PINSOC        GPIO_PinSource12
#define SPI_GPIOPIN_LED_CLK_PINSOC       GPIO_PinSource13
#define SPI_GPIOPIN_LED_DATA_PINSOC      GPIO_PinSource15



struct LED led[16];
unsigned short ledstat;
static unsigned short ledbitmap = 0xffff;


void keyLedProcess() {
    everydiffdo(unsigned int, ledkeytimermark, timerTick1ms) {
        for (int i = 0; i < lenthof(led); i++) {
            switch(led[i].func) {
            case ONOFF:
                if (led[i].on && led[i].ontime != 0xffff
                    && led[i].ontimeval++ >= led[i].ontime) {
                    led[i].on =  false;
                    led[i].ontimeval = 0;
                    led[i].stat = 1;
                    led[i].func = 0;
                    ledstat = 1;
                }
                break;
            case BLINKON:
                if (led[i].ontimeval++ >= led[i].ontime) {
                    led[i].ontimeval = 0;
                    led[i].on =  false;
                    led[i].stat = 1;
                    led[i].func = BLINKOFF;
                    ledstat = 1;
                }
                break;
            case BLINKOFF:
                if (led[i].offtimeval++ >= led[i].offtime) {
                    led[i].offtimeval = 0;
                    led[i].on  =  true;
                    led[i].func = BLINKON;
                    led[i].stat = 1;
                    ledstat = 1;
                }
                break;
            default:
                break;
            }
        }
        if (ledstat == 1) {
            ledstat = 0;
            for (int i = 0; i < 16; i++) {
              if(led[i].stat == 1){
                led[i].stat = 0;
                if (led[i].on == true) {
                    ledbitmap &=  ~(unsigned short)(1 << i);
                } else {
                    ledbitmap |= (unsigned short)(1 << i);
                }
                keyLedSet(ledbitmap);
                }
            }
        }
    }
}



void keyLedSet(unsigned short val) {
    GPIO_ResetBits(SPI_GPIO_LED_CS, SPI_GPIOPIN_LED_CS);
    SPI_SendData8(SPI2, val >> 8);
    SPI_SendData8(SPI2, val);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
    GPIO_SetBits(SPI_GPIO_LED_CS, SPI_GPIOPIN_LED_CS);
}





void keyLedOn_bitmap(unsigned short bitmap) {
    for (int i = 0; i < 16; i++) {
        if ((bitmap & 1 << i) == 0) {
            led[i].stat = ONOFF;
            led[i].ontime = 0xffff;
            led[i].on = 1;
        } else {
            led[i].stat = ONOFF;
            led[i].ontime = 0xffff;
            led[i].on = 0;
        }
    }
    ledbitmap = bitmap;
    keyLedSet(bitmap);
}




void keyLedInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    //CS
    GPIO_InitStruct.GPIO_Pin = SPI_GPIOPIN_LED_CS;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_Init(SPI_GPIO_LED_CS, &GPIO_InitStruct);
    //SPI CLK
    GPIO_InitStruct.GPIO_Pin = SPI_GPIOPIN_LED_CLK;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SPI_GPIO_LED_CLK, &GPIO_InitStruct); //时钟gpio 端口模式

    //SPI MOSI
    GPIO_InitStruct.GPIO_Pin = SPI_GPIOPIN_LED_DATA;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(SPI_GPIO_LED_DATA, &GPIO_InitStruct);

    //AF
    //GPIO_PinAFConfig(SPI_GPIO_LED_CS, SPI_GPIOPIN_LED_CS_PINSOC, GPIO_AF_0);
    GPIO_PinAFConfig(SPI_GPIO_LED_CLK, SPI_GPIOPIN_LED_CLK_PINSOC, GPIO_AF_0);
    GPIO_PinAFConfig(SPI_GPIO_LED_DATA, SPI_GPIOPIN_LED_DATA_PINSOC, GPIO_AF_0);

    //SpiCsDisable();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;

    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;

    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_SSOutputCmd(SPI2, ENABLE);
    GPIO_SetBits(SPI_GPIO_LED_CS, SPI_GPIOPIN_LED_CS);
    SPI_Cmd(SPI2, ENABLE);
    keyLedSet(0xffff);
}


