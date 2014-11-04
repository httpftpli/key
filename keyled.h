#ifndef __KEYLED_H___
#define __KEYLED_H___
#include <stdbool.h>


enum LEDFUN {ONOFF=1, BLINKON, BLINKOFF,LEDFUNMAX};

struct LED {
    unsigned char func;
    unsigned char stat;
    bool          on;
    unsigned short ontime;
    unsigned short ontimeval;
    unsigned short offtime;
    unsigned short offtimeval;
};


extern unsigned short ledstat;
extern struct LED led[16];
extern void keyLedInit(void) ;
extern void keyLedOn_bitmap(unsigned short val);
extern void keyLedProcess();
extern void keyLedSet(unsigned short val);


#endif