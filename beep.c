#include "beep.h"
#include "stdbool.h"
#include "timer.h"
#include "main.h"



//AlarmBeep £º


void BeepON(void) {
    GPIO_SetBits(BEEPOUT_GPIO, BEEPOUT_GPIOBIT);
}

void BeepOFF(void) {
    GPIO_ResetBits(BEEPOUT_GPIO, BEEPOUT_GPIOBIT);
}


void beepInit(void) {
    //beep out
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(BEEP_GPIO_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin = BEEPOUT_GPIOBIT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BEEPOUT_GPIO, &GPIO_InitStructure);
    BeepOFF();
}




void beep(unsigned int dur){
    #define BEEP_OFF  0;
    #define BEEP_ON   1;
    static unsigned int beeponlasttime = 0;
    static bool beepstat = BEEP_OFF
    if (dur>beeponlasttime) {
        beeponlasttime = dur;
    }
    everydiffdo(unsigned int,dur,timerTick1ms){
        if(beeponlasttime!=0) beeponlasttime--;
    }
    if ((beeponlasttime!=0) && !beepstat ) {
        BeepON();
        beepstat = BEEP_ON;
    }
    if ((beeponlasttime==0) && beepstat) {
        BeepOFF();
        beepstat = BEEP_OFF;
    }
}


