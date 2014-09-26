#include  "touch.h"

TOUCHPRA TouchPra;

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
    GPIO_Init(ADS7843_PORT, &GPIO_InitStruct); //鏃堕挓gpio 绔彛妯″紡


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
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;

    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    SPI_Cmd(SPI1, ENABLE);

    TouchPra.TouchIntState = 1;
    TouchPra.TouchDataState = 0;
    TouchPra.TouchState = 0;
    TouchPra.TouchContinuousCount = 0;
    TouchPra.TouchIntCount = TOUCHINTMAX;
    TouchPra.TouchScanCount = 0;
    TouchPra.TouchMissCount = TOUCHMISSDEFUALT;
    //spiInit(0,0);
    //touchTimerInit();
}

uint8_t   GetTouchIntVaule(void) {
    return(GPIO_ReadInputDataBit(ADS7843_INT_GPIO, ADS7843_INT_GPIOBIT));
}
uint8_t   GetTouchBuyeVaule(void) {
    return(GPIO_ReadInputDataBit(ADS7843_BUYE_GPIO, ADS7843_BUYE_GPIOBIT));
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

uint16_t sum(uint16_t *buf, uint16_t nNum) {
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


uint8_t GetTouchSample(void) {

    uint16_t xsampleval[8], ysampleval[8];
    uint16_t i;

    for (i = 0; i < 8; i++) {
        xsampleval[i] = TouchXsample();
        ysampleval[i] = TouchYsample();
    }

    /*
    for(i=0;i<5;i++)
    {
       Xbuf[i]=xsampleval[i+3];
       Ybuf[i]=ysampleval[i+3];
    }
    */
    bubbleSortAscend(xsampleval, lenthof(xsampleval));
    bubbleSortAscend(ysampleval, lenthof(xsampleval));
    if ((xsampleval[7] - xsampleval[0] > TouchPra.TouchMissCount) || (ysampleval[7] - ysampleval[0] > TouchPra.TouchMissCount)) {
        return  1;
    }
    TouchPra.TouchYsample =  sum(ysampleval + 2, lenthof(ysampleval) - 4) / (lenthof(ysampleval) - 4);
    TouchPra.TouchXsample =  sum(xsampleval + 2, lenthof(xsampleval) - 4) / (lenthof(xsampleval) - 4);
    return 0;
}



void TouchScan(void) {
    switch (TouchPra.TouchState) {
    case  0x00:
        {
            /////////////等待触摸按下
            if (TouchPra.TouchIntState == 0 && TouchPra.TouchScanCount == 0) {
                /////////触摸第一次有效,读数据并发送，进入连击状态
                if (GetTouchSample() == 0) {
                    TouchPra.TouchDataState = 1;
                    TouchPra.TouchState = 0x01;
                    TouchPra.TouchContinuousCount = TOUCHCONTINUOUSMAX;
                } else {
                    TouchPra.TouchDataState = 0;
                    TouchPra.TouchScanCount == TOUCHSCANTIME;
                }
            }
            break;
        }
    case  0x01:
        {
            /////////////等待连击，如不符合连击条件，
            if ((TouchPra.TouchState == 1) && (TouchPra.TouchContinuousCount == 0)) {
                /////////触摸第连击有效,读数据并发送，进入连击状态
                TouchPra.TouchState = 0x01;
                if (GetTouchSample() == 0) {
                    TouchPra.TouchDataState = 1;
                    TouchPra.TouchContinuousCount = TOUCHCONTINUOUSMAX;
                } else {
                    TouchPra.TouchDataState = 0;
                    TouchPra.TouchContinuousCount = TOUCHSCANTIME;
                }
            }

            break;
        }

    default:
        {
            TouchPra.TouchState = 0;
            break;
        }
    }
}




