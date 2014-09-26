#include  "lcd.h"


void LcdInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = LCDBL_GPIO;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(LCDBL_GPIO, &GPIO_InitStructure);
    LcdBlCtr(LCDBL_ON);
}
void LcdBlCtr(uint8_t  newstate) {
    if (newstate == LCDBL_OFF) {
        GPIO_ResetBits(LCDBL_GPIO, LCDBL_GPIO);
    } else {
        GPIO_SetBits(LCDBL_GPIO, LCDBL_GPIO);
    }
}

