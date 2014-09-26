#include "keyscan.h"

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;

uint8_t KeyScanBuf[3];         //去抖数据缓冲区
uint8_t KeyValueBuf[2];        //键值数据缓冲区
uint8_t KeyValue;             //键值
uint8_t KeyScanCount;          //扫描时间计数器 1个1MS
uint16_t KeyContinuousCount;    //连击时间计数器 1个1MS
uint16_t KeyRunContinuousCount;    //连击时间计数器 1个1MS


// 允许连击的按键
const uint8_t  KeyValueContinuous[NUM_KeyContinuous ]=
{
  KeyContinuous_UP,
  KeyContinuous_DOWN,
  KeyContinuous_LEFT,
  KeyContinuous_RIGHT,
  KeyContinuous_PnUP,
  KeyContinuous_PnDOWN 
                                      
};  

GPIOGROUP keyScanOutput[NUM_KEY_OUT] ={
  { KEY_OUT0_PORT, KEY_OUT0},{ KEY_OUT1_PORT, KEY_OUT1},{ KEY_OUT2_PORT, KEY_OUT2},{ KEY_OUT3_PORT, KEY_OUT3},
  { KEY_OUT4_PORT, KEY_OUT4},{ KEY_OUT5_PORT, KEY_OUT5},{ KEY_OUT6_PORT, KEY_OUT6},{ KEY_OUT7_PORT, KEY_OUT7}
};
 



GPIOGROUP keyScanInput[NUM_KEY_IN] ={
  {KEY_D0_IN_PORT,KEY_D0},{KEY_D1_IN_PORT,KEY_D1},{KEY_D2_IN_PORT,KEY_D2},{KEY_D3_IN_PORT,KEY_D3},
  {KEY_D4_IN_PORT,KEY_D4},{KEY_D5_IN_PORT,KEY_D5},{KEY_D6_IN_PORT,KEY_D6},{KEY_D7_IN_PORT,KEY_D7}
};
void Keyscandelay(uint16_t n)
{
   while(n)
   {
        n--;
   }   
}




void leyScanOutputReset(void){
  uint8_t i=0;
  for(i=0;i<NUM_KEY_OUT;i++)
  {
    GPIO_SetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
  }
}
void keyScanInit(void)
{
   uint8_t i=0;
   GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC, ENABLE);
   //output
   for(i=0;i<NUM_KEY_OUT;i++){    
      GPIO_InitStructure.GPIO_Pin = keyScanOutput[i].bit;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
      GPIO_Init(keyScanOutput[i].port, &GPIO_InitStructure);           
   }
   //input
   
   for(i=0;i<NUM_KEY_IN;i++){
      GPIO_InitStructure.GPIO_Pin = keyScanInput[i].bit;	
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
      GPIO_Init(keyScanInput[i].port, &GPIO_InitStructure);
   }
   //output 1
   leyScanOutputReset();
   KeyScanBuf[2]=0xff;
   KeyScanBuf[1]=0xff;
   KeyScanBuf[0]=0xff;
   KeyValueBuf[1]=0xff;
   KeyValueBuf[0]=0xff;
   KeyValue=0xff;
   KeyScanCount=KEYSCANTIME;
   KeyContinuousCount=KEYCONTINUOUSTIME;
   KeyRunContinuousCount=KEYRUNCONTINUOUSTIME;
   
}

uint8_t keyScan(void)
{
    uint8_t   i,j,bitval;
    uint8_t  keycodenow=0xff;
    leyScanOutputReset();
    for(i=0;i<NUM_KEY_OUT;i++)
    {
       
       GPIO_ResetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
       Keyscandelay(100);
       for(j=0;j<NUM_KEY_IN;j++)
       {
          bitval =  GPIO_ReadInputDataBit(keyScanInput[j].port, keyScanInput[j].bit);
          if(bitval==0)
          {
            keycodenow = i*8+j;
            GPIO_SetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
            return  keycodenow;
          }
       }
       GPIO_SetBits(keyScanOutput[i].port, keyScanOutput[i].bit);
    }
    return  keycodenow;
}
//返回3  表示连击有效，发送键值，如有触摸数据一并发送
//返回2 表示按键有效，发送键值，如有触摸数据一并发送
//返回1 表示按键无效，但触摸有数据可以发送
//返回0 表示扫描时间未到，触摸即使有数据, 也不发送键值
uint8_t  GetKeystate(void)
{
   uint8_t i;
   if(KeyScanCount==0)
   {
      KeyScanBuf[2]=KeyScanBuf[1];           
      KeyScanBuf[1]=KeyScanBuf[0];
      KeyScanBuf[0]=keyScan();
      KeyScanCount=KEYSCANTIME;
   
      if((KeyScanBuf[2]==KeyScanBuf[1])&&(KeyScanBuf[2]==KeyScanBuf[0]))  //去抖
      {
          KeyValueBuf[1]=KeyValueBuf[0];                         //保存上次的键值
          KeyValueBuf[0]=KeyScanBuf[0];                          //取得当前键值
          if(KeyValueBuf[1]!=KeyValueBuf[0])                    //按键发生变化
          {
            if(KeyValueBuf[0]==0xff)
            { 
                KeyValue=0xff;
                return   1;                                     //按键放开
            }
            else
            {
                  KeyValue=KeyValueBuf[0];                    //按键有效
                  KeyContinuousCount=KEYCONTINUOUSTIME;       //启动连击计时
                  return  2;
            }
          }
          else                                               //按键无变化
          {
            if(KeyContinuousCount==0)                    //判断连击
            {
               for(i=0;i<NUM_KeyContinuous;i++)
               {
                   if(KeyValueBuf[0]==KeyValueContinuous[i]&&KeyRunContinuousCount==0) 
                   {
                      KeyValue=KeyValueBuf[0];
                      
                      return  3;
                   }
               }
               KeyValue=0xff;                           //不是有效连击
               return  1; 
            }
            KeyValue=0xff;                              //未到连击时间
            return  1;    
         }
      }
       KeyValue=0xff;                              //按键去抖中
       return  1;
   }
   else                                            //扫描时间未到，不允许发送任何数据
   {
     KeyValue=0xff;
     return  0;
   } 
}
