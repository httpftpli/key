#include "stm32f0xx.h"

#define SPI_GPIO_LED_CS           GPIOB
#define SPI_GPIO_LED_CLK          GPIOB
#define SPI_GPIO_LED_DATA         GPIOB

#define SPI_GPIOPIN_LED_CS        GPIO_Pin_12
#define SPI_GPIOPIN_LED_CLK       GPIO_Pin_13
#define SPI_GPIOPIN_LED_DATA      GPIO_Pin_15


#define SPI_GPIOPIN_LED_CS_PINSOC        GPIO_PinSource12
#define SPI_GPIOPIN_LED_CLK_PINSOC       GPIO_PinSource13
#define SPI_GPIOPIN_LED_DATA_PINSOC      GPIO_PinSource15


void keyLedSet(unsigned short val){
  GPIO_ResetBits(SPI_GPIO_LED_CS,SPI_GPIOPIN_LED_CS);
  SPI_SendData8(SPI2, val>>8);
  SPI_SendData8(SPI2, val);
  while ( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY)==SET);
  GPIO_SetBits(SPI_GPIO_LED_CS,SPI_GPIOPIN_LED_CS);
}



void keyLedInit(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
 //CS
  GPIO_InitStruct.GPIO_Pin =SPI_GPIOPIN_LED_CS;
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
  GPIO_Init(SPI_GPIO_LED_CLK, &GPIO_InitStruct);//时钟gpio 端口模式

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
  GPIO_SetBits(SPI_GPIO_LED_CS,SPI_GPIOPIN_LED_CS);
  SPI_Cmd(SPI2, ENABLE);
  keyLedSet(0xffff);
}


