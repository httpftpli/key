#include "keyscan.h"
#include "timer.h"

typedef struct __gpiogroup {
    GPIO_TypeDef *port;
    unsigned int bit;
}GPIOGROUP;

uint8_t KeyScanBuf[3];         //去抖数据缓冲区
uint8_t KeyValueBuf[2];        //键值数据缓冲区
uint8_t KeyValue;             //键值
uint8_t KeyScanCount;          //扫描时间计数器 1个1MS
uint16_t KeyContinuousCount;    //连击时间计数器 1个1MS
uint16_t KeyRunContinuousCount;    //连击时间计数器 1个1MS


// 允许连击的按键
const uint8_t  KeyValueContinuous[NUM_KeyContinuous] =
{
    KeyContinuous_UP,
    KeyContinuous_DOWN,
    KeyContinuous_LEFT,
    KeyContinuous_RIGHT,
    KeyContinuous_PnUP,
    KeyContinuous_PnDOWN

};

GPIOGROUP keyScanOutput[NUM_KEY_OUT] = {
    { KEY_OUT0_PORT, KEY_OUT0 }, { KEY_OUT1_PORT, KEY_OUT1}, { KEY_OUT2_PORT, KEY_OUT2}, { KEY_OUT3_PORT, KEY_OUT3},
    { KEY_OUT4_PORT, KEY_OUT4}, { KEY_OUT5_PORT, KEY_OUT5}, { KEY_OUT6_PORT, KEY_OUT6}, { KEY_OUT7_PORT, KEY_OUT7}
};




GPIOGROUP keyScanInput[NUM_KEY_IN] = {
    { KEY_D0_IN_PORT, KEY_D0 }, { KEY_D1_IN_PORT, KEY_D1}, { KEY_D2_IN_PORT, KEY_D2}, { KEY_D3_IN_PORT, KEY_D3},
    { KEY_D4_IN_PORT, KEY_D4}, { KEY_D5_IN_PORT, KEY_D5}, { KEY_D6_IN_PORT, KEY_D6}, { KEY_D7_IN_PORT, KEY_D7}
};


void Keyscandelay(uint16_t n) {
    while (n) {
        n--;
    }
}




void keyScanOutputReset(void) {
    uint8_t i = 0;
    for (i = 0; i < NUM_KEY_OUT; i++) {
        GPIO_SetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
    }
}


void keyScanInit(void) {
    uint8_t i = 0;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
    //output
    for (i = 0; i < NUM_KEY_OUT; i++) {
        GPIO_InitStructure.GPIO_Pin = keyScanOutput[i].bit;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(keyScanOutput[i].port, &GPIO_InitStructure);
    }
    //input
    for (i = 0; i < NUM_KEY_IN; i++) {
        GPIO_InitStructure.GPIO_Pin = keyScanInput[i].bit;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz ;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_Init(keyScanInput[i].port, &GPIO_InitStructure);
    }
    //output 1
    keyScanOutputReset();
}


#define TIMEINTERVAL_2KEY        500 //500ms
#define TIMEINTERVAL_DEBOUNCE    10  //10ms
#define TIMEINTERVAL_KEYHOLD     100  //10ms

#define NOKEY                   0
#define KEYTODEBOUNCE           1
#define KEYPUSHED               2
#define KEYHOLD                 3


unsigned char keycode;
bool keytouched = false;

unsigned char keyScan(void) {
    static unsigned int keystate = NOKEY;
    static unsigned int timermark;
    unsigned char keycodenow;
    keytouched = false;
    for (int i = 0; i < (sizeof(keyScanOutput) / sizeof(GPIOGROUP)); i++) {
        keyScanOutputReset();
        GPIO_ResetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
        for(int i=0;i<100;i++); //delay
        for (int j = 0; j < (sizeof(keyScanInput) / sizeof(GPIOGROUP)); j++) {
            unsigned int bitval = GPIO_ReadInputDataBit(keyScanInput[j].port, keyScanInput[j].bit);
            if (0 == bitval) {
                keycodenow = i * 8 + j;
                goto KEYPROCESS;
            }
        }
    }
    keystate = NOKEY;
    return -1;

    KEYPROCESS:
    switch (keystate) {
    case NOKEY:
        keystate = KEYTODEBOUNCE;
        keycode = keycodenow;
        timermark = timerTick1ms;
        break;
    case KEYTODEBOUNCE:
        if (keycodenow == keycode) {
            if ((timerTick1ms - timermark) >= TIMEINTERVAL_DEBOUNCE) {
                keystate = KEYPUSHED;
                keycode = keycodenow;
                timermark = timerTick1ms;
                keytouched = true;
                return keycode;
            }
        } else {
            keystate = NOKEY;
        }
        break;
    case KEYPUSHED:
        if (keycodenow == keycode) {
            if ((timerTick1ms - timermark) >= TIMEINTERVAL_2KEY) {
                keystate = KEYHOLD;
                keycode = keycodenow;
                timermark = timerTick1ms;
                keytouched = true;
                return keycode;
            }
        } else {
            keystate = NOKEY;
        }
        break;
    case KEYHOLD:
        if (keycodenow == keycode) {
            if ((timerTick1ms - timermark) >= TIMEINTERVAL_KEYHOLD) {
                keystate = KEYHOLD;
                keycode = keycodenow;
                timermark = timerTick1ms;
                keytouched = true;
                return keycode;
            }
        } else {
            keystate = NOKEY;
        }
        break;
    default:
        break;
    }
    return -1;
}
