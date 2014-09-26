#include "beep.h"


//·äÃùÆ÷¿ØÖÆ²ÎÊý


BEEPCTRPRA  BeepCtrPra;
AlARM1BEECPTRPRA Alarm1BeepCtrPra;
AlARM2BEECPTRPRA Alarm2BeepCtrPra;



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
    BeepCtrPra.BeepMode = 0;
    BeepCtrPra.BeepKeyCount = 0;

    BeepCtrPra.Beep_KeyOffEn = 0;

    Alarm1BeepCtrPra.AlarmBeepState = 0;
    Alarm1BeepCtrPra.AlarmBeepOnCount = ALARM1_BEEP_TIME;
    Alarm1BeepCtrPra.AlarmBeepOffShortCount = ALARM1_BEEPOFF_SHORT_TIME;
    Alarm1BeepCtrPra.AlarmBeepOffLongCount = ALARM1_BEEPOFF_LONG_TIME;
}

void Alarm1BeepCtr(void) {
    switch (Alarm1BeepCtrPra.AlarmBeepState) {
    case 0x00:
        {
            if (Alarm1BeepCtrPra.AlarmBeepOnCount > 0) {
                BeepON();
                Alarm1BeepCtrPra.AlarmBeepOnCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOnCount = ALARM1_BEEP_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x01;
            }
            break;
        }
    case 0x01:
        {
            if ((Alarm1BeepCtrPra.AlarmBeepOffShortCount > 0) && (BeepCtrPra.Beep_KeyOffEn == 0)) {
                BeepOFF();
                Alarm1BeepCtrPra.AlarmBeepOffShortCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOffShortCount = ALARM1_BEEPOFF_SHORT_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x02;
            }
            break;
        }
    case 0x02:
        {
            if (Alarm1BeepCtrPra.AlarmBeepOnCount > 0) {
                BeepON();
                Alarm1BeepCtrPra.AlarmBeepOnCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOnCount = ALARM1_BEEP_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x03;
            }
            break;
        }
    case 0x03:
        {
            if ((Alarm1BeepCtrPra.AlarmBeepOffShortCount > 0) && (BeepCtrPra.Beep_KeyOffEn == 0)) {
                BeepOFF();
                Alarm1BeepCtrPra.AlarmBeepOffShortCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOffShortCount = ALARM1_BEEPOFF_SHORT_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x04;
            }
            break;

        }
    case 0x04:
        {
            if (Alarm1BeepCtrPra.AlarmBeepOnCount > 0) {
                BeepON();
                Alarm1BeepCtrPra.AlarmBeepOnCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOnCount = ALARM1_BEEP_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x05;
            }
            break;
        }
    case 0x05:
        {
            if ((Alarm1BeepCtrPra.AlarmBeepOffLongCount > 0) && (BeepCtrPra.Beep_KeyOffEn == 0)) {
                BeepOFF();
                Alarm1BeepCtrPra.AlarmBeepOffLongCount--;
            } else {
                Alarm1BeepCtrPra.AlarmBeepOffLongCount = ALARM1_BEEPOFF_LONG_TIME;
                Alarm1BeepCtrPra.AlarmBeepState = 0x00;
            }
            break;

        }
    default:
        {
            Alarm1BeepCtrPra.AlarmBeepState = 0x00;
            break;
        }
    }
}


void Alarm2BeepCtr(void) {
    switch (Alarm2BeepCtrPra.AlarmBeepState) {
    case 0x00:
        {
            if (Alarm2BeepCtrPra.AlarmBeepOnCount > 0) {
                BeepON();
                Alarm2BeepCtrPra.AlarmBeepOnCount--;
            } else {
                Alarm2BeepCtrPra.AlarmBeepOnCount = ALARM2_BEEP_TIME;
                Alarm2BeepCtrPra.AlarmBeepState = 0x01;
            }
            break;
        }
    case 0x01:
        {
            if ((Alarm2BeepCtrPra.AlarmBeepOffCount > 0) && (BeepCtrPra.Beep_KeyOffEn == 0)) {
                BeepOFF();
                Alarm2BeepCtrPra.AlarmBeepOffCount--;
            } else {
                Alarm2BeepCtrPra.AlarmBeepOffCount = ALARM2_BEEPOFF_TIME;
                Alarm2BeepCtrPra.AlarmBeepState = 0x00;
            }
            break;
        }
    default:
        {
            Alarm2BeepCtrPra.AlarmBeepState = 0x00;
            break;
        }
    }
}



