#include "lcdled.h"

void LcdBlInit(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    //output

    GPIO_InitStructure.GPIO_Pin = LCDBL_GPIO_GPIOBIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LCDBL_GPIO, &GPIO_InitStructure);
    LcdBlOff();
}

void LcdBlOn(void) {
    GPIO_ResetBits(LCDBL_GPIO, LCDBL_GPIO_GPIOBIT);
}


void LcdBlOff(void) {
    GPIO_SetBits(LCDBL_GPIO, LCDBL_GPIO_GPIOBIT);
}
