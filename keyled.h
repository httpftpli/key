#ifndef __KEYLED_H___
#define __KEYLED_H___
#include <stdbool.h>


enum LEDFUN {ONOFF=1, BLINKON, BLINKOFF};

struct LED {
    unsigned char func;
    unsigned char stat;
    bool          on;
    unsigned int ontime;
    unsigned int ontimeval;
    unsigned int offtime;
    unsigned int offtimeval;
};


extern unsigned short ledstat;
extern struct LED led[16];
extern void keyLedInit(void) ;
extern void keyLedOn_bitmap(unsigned short val);
extern void keyLedProcess();
extern void keyLedSet(unsigned short val);


#endif