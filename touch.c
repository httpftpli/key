#include  "touch.h"
#include <stdbool.h>
#include "timer.h"
#include <string.h>



#define TOUCH_DEBUG    0


unsigned int touchAccurMargin = TOUCHMISSDEFUALT;

void SpiCsEnable(void);
void SpiCsDisable(void);
void SpiSend(uint8_t *buf, uint8_t Length);
void SpiReceive(uint8_t *buf, uint8_t length);
uint8_t TouchSample(void);
uint32_t sum(uint16_t *buf, uint16_t nNum);
void bubbleSortAscend(uint16_t *buf, uint16_t nNum);
uint16_t TouchXsample(void);
uint16_t TouchYsample(void);
bool touchPushed(void);


void SpiCsEnable(void) {
    GPIO_ResetBits(ADS7843_CS_GPIO, ADS7843_CS_GPIOBIT);
}

void SpiCsDisable(void) {
    GPIO_SetBits(ADS7843_CS_GPIO, ADS7843_CS_GPIOBIT);
}


void TouchInit(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    //INT
    GPIO_InitStruct.GPIO_Pin = ADS7843_INT_GPIOBIT;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(ADS7843_INT_GPIO, &GPIO_InitStruct);

    //BUYE
    GPIO_InitStruct.GPIO_Pin = ADS7843_BUYE_GPIOBIT;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(ADS7843_BUYE_GPIO, &GPIO_InitStruct);

    //CS
    GPIO_InitStruct.GPIO_Pin = ADS7843_CS_GPIOBIT;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_Init(ADS7843_CS_GPIO, &GPIO_InitStruct);
    //SPI
    GPIO_InitStruct.GPIO_Pin = ADS7843_SLK_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ADS7843_PORT, &GPIO_InitStruct); //时钟gpio 端口模式


    GPIO_InitStruct.GPIO_Pin = ADS7843_MISO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ADS7843_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = ADS7843_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ADS7843_PORT, &GPIO_InitStruct);

    //AF

    GPIO_PinAFConfig(ADS7843_PORT, ADS7843_SLK_PINSOC, GPIO_AF_0);
    GPIO_PinAFConfig(ADS7843_PORT, ADS7843_MISO_PINSOC, GPIO_AF_0);
    GPIO_PinAFConfig(ADS7843_PORT, ADS7843_MOSI_PINSOC, GPIO_AF_0);

    SpiCsDisable();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_InitTypeDef SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;

    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI1, ENABLE);

    //spiInit(0,0);
    //touchTimerInit();
}


bool touchPushed(void){
    return !(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT));
}



void bubbleSortAscend(uint16_t *buf, uint16_t nNum) {
    uint16_t j, k;
    uint16_t flag = nNum;
    unsigned int tmp;
    while (flag > 0) {
        k = flag;
        flag = 0;
        for (j = 1; j < k; j++) {
            if (buf[j - 1] > buf[j]) {
                tmp = buf[j - 1];
                buf[j - 1] = buf[j];
                buf[j] = tmp;
                flag = j;
            }
        }
    }
}

uint32_t sum(uint16_t *buf, uint16_t nNum) {
    int16_t sum = 0;
    for (int i = 0; i < nNum; i++) {
        sum += buf[i];
    }
    return sum;
}


void SpiSend(uint8_t *buf, uint8_t Length) {
    uint8_t i;
    //uint8_t Dummy;
    for (i = 0; i < Length; i++) {
        /* Move on only TX FIFO not full. */
        while (SPI_TransmissionFIFOStatus_Full == SPI_GetTransmissionFIFOStatus(SPI1)) {
        }
        SPI_SendData8(SPI1, *(buf + i));
    }

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
}


void SpiReceive(uint8_t *buf, uint8_t length) {
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == SET) {
        SPI_I2S_ReceiveData16(SPI1);
    }
    for (uint8_t i = 0; i < length; i++) {
        SPI_SendData8(SPI1, 0);
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET);
        *(buf + i) = SPI_ReceiveData8(SPI1);
    }
}


uint16_t TouchXsample(void) {
    uint8_t rcvbuf[2];
    uint8_t sendval = ADS7843_CTRL_X_SAMPLE;
    uint16_t val;

    SpiCsEnable();
    SpiSend(&sendval, 1);
    SpiReceive(&rcvbuf[0], 2);
    SpiCsDisable();

    val = (uint16_t)rcvbuf[0];
    val = (val & 0x7f) << 5;
    val = val | ((uint16_t)rcvbuf[1]) >> 3;
    return val;
}


uint16_t TouchYsample(void) {
    uint8_t rcvbuf[2];
    uint16_t val;
    uint8_t sendval = ADS7843_CTRL_Y_SAMPLE;

    SpiCsEnable();
    SpiSend(&sendval, 1);
    SpiReceive(&rcvbuf[0], 2);
    SpiCsDisable();

    val = (uint16_t)rcvbuf[0];
    val = (val & 0x7f) << 5;
    val = val | ((uint16_t)rcvbuf[1]) >> 3;
    return val;
}


#define SAMPLE_NUM_ERVERY    5




void  touchSample(uint16_t *x, uint16_t *y) {
    uint16_t xsampleval[SAMPLE_NUM_ERVERY], ysampleval[SAMPLE_NUM_ERVERY];
#if TOUCH_DEBUG
    //GPIOSetValue(3, 0, 1);
#endif
    for(int i=0;i<100;i++);
    for (int i = 0; i < SAMPLE_NUM_ERVERY; i++) {
        xsampleval[i] = TouchXsample();
    }
    for(int i=0;i<100;i++);
    bubbleSortAscend(xsampleval, lenthof(xsampleval));
    for (int i = 0; i < SAMPLE_NUM_ERVERY; i++) {
        ysampleval[i] = TouchYsample();
    }
#if TOUCH_DEBUG
    static unsigned short debugbufx[200][SAMPLE_NUM_ERVERY],debugbufy[200][SAMPLE_NUM_ERVERY];
    static unsigned int debugbufindex = 0;
    memcpy(debugbufx[debugbufindex],xsampleval, sizeof xsampleval);
    memcpy(debugbufy[debugbufindex],ysampleval, sizeof ysampleval);
    if(++debugbufindex == 200) debugbufindex = 0;
    // GPIOSetValue(3, 0, 0);
#endif
    bubbleSortAscend(ysampleval, lenthof(ysampleval));
    *y =  sum(ysampleval + 1, lenthof(ysampleval) - 2) / (lenthof(ysampleval) - 2);
    *x =  sum(xsampleval + 1, lenthof(xsampleval) - 2) / (lenthof(xsampleval) - 2);

}



bool touched = false;
unsigned short touchpointX ;
unsigned short touchpointY ;


void TouchScan(){
#define NO_TOUCH            0
#define TOUCHED     3
#define TOUCH_SAMPLE_BEGIN  5
#define TOUCH_DEBOUNCE      7
#define TOUCH_FROZE       4


  static int samplecounter = 0,stat = NO_TOUCH;
  static uint16_t xbuf[TOUCH_SAMPLE_NUM],ybuf[TOUCH_SAMPLE_NUM];
  static uint16_t xbuftemp[TOUCH_SAMPLE_NUM],ybuftemp[TOUCH_SAMPLE_NUM];
  static unsigned int timemark;
  touched = false;
  switch(stat){
   case NO_TOUCH:
      if(touchPushed()){
         stat = TOUCH_DEBOUNCE;
         timemark = timerTick05ms;
      }
      break;
  case TOUCH_DEBOUNCE:
    if(!touchPushed()){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      if(timerTick05ms-timemark> 30){
        stat = TOUCH_SAMPLE_BEGIN;
        memset(xbuf,0 ,sizeof xbuf);
        memset(ybuf,0 ,sizeof ybuf);
        timemark = timerTick05ms;
      }
    }
    break;
  case TOUCH_SAMPLE_BEGIN:
     if(!touchPushed()){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      touchSample(xbuf+samplecounter,ybuf+samplecounter);
      samplecounter ++;
      if(samplecounter == TOUCH_SAMPLE_NUM){
         samplecounter = 0;
         memcpy(xbuftemp, xbuf,sizeof xbuf);
         memcpy(ybuftemp, ybuf,sizeof ybuf);
         bubbleSortAscend(xbuftemp, TOUCH_SAMPLE_NUM);
         bubbleSortAscend(ybuftemp, TOUCH_SAMPLE_NUM);
         if(((xbuftemp[TOUCH_SAMPLE_NUM-1]-xbuftemp[0])<touchAccurMargin)&&
            ((ybuftemp[TOUCH_SAMPLE_NUM-1]-ybuftemp[0])<touchAccurMargin)){
           touchpointX = xbuftemp[TOUCH_SAMPLE_NUM/2];
           touchpointY = ybuftemp[TOUCH_SAMPLE_NUM/2];
           stat = TOUCHED;
           timemark = timerTick05ms;
           touched = 1;
          }else if((timerTick05ms-timemark>60) &&
                   ((xbuftemp[TOUCH_SAMPLE_NUM-1]-xbuftemp[0])<touchAccurMargin+15)&&
                   ((ybuftemp[TOUCH_SAMPLE_NUM-1]-ybuftemp[0])<touchAccurMargin+15)){
           touchpointX = xbuftemp[TOUCH_SAMPLE_NUM/2];
           touchpointY = ybuftemp[TOUCH_SAMPLE_NUM/2];
           stat = TOUCHED;
           timemark = timerTick05ms;
           touched = 1;
          }
      }
    }
    break;
  case TOUCHED:
     if(!touchPushed()){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      touchSample(xbuf+samplecounter,ybuf+samplecounter);
      samplecounter ++;
      if((samplecounter == TOUCH_SAMPLE_NUM)){
         samplecounter = 0;
         memcpy(xbuftemp, xbuf,sizeof xbuf);
         memcpy(ybuftemp, ybuf,sizeof ybuf);
         bubbleSortAscend(xbuftemp, TOUCH_SAMPLE_NUM);
         bubbleSortAscend(ybuftemp, TOUCH_SAMPLE_NUM);
         if(((xbuftemp[TOUCH_SAMPLE_NUM-1]-xbuftemp[0])<touchAccurMargin+60)&&
            ((ybuftemp[TOUCH_SAMPLE_NUM-1]-ybuftemp[0])<touchAccurMargin+60)){
                touchpointX = xbuftemp[TOUCH_SAMPLE_NUM/2];
                touchpointY = ybuftemp[TOUCH_SAMPLE_NUM/2];
                touched = 1;
                stat = TOUCH_FROZE;
          }
      }
    }
    break;
  case TOUCH_FROZE:
    if(!touchPushed()){
        samplecounter = 0;
        stat = NO_TOUCH;
    }else{
      if(timerTick05ms-timemark > 40){
        stat = TOUCHED;
        samplecounter = 0;
        timemark = timerTick05ms;
      }
    }
    break;
  default:
    break;
  }
}


void  setTouchAccur(unsigned char val){
    touchAccurMargin = val;
 }
