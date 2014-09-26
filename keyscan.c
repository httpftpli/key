#include "keyscan.h"

typedef struct __gpiogroup{
  GPIO_TypeDef* port;
  unsigned int bit;
}GPIOGROUP;

uint8_t KeyScanBuf[3];         //ȥ�����ݻ�����
uint8_t KeyValueBuf[2];        //��ֵ���ݻ�����
uint8_t KeyValue;             //��ֵ
uint8_t KeyScanCount;          //ɨ��ʱ������� 1��1MS
uint16_t KeyContinuousCount;    //����ʱ������� 1��1MS
uint16_t KeyRunContinuousCount;    //����ʱ������� 1��1MS


// ���������İ���
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
//����3  ��ʾ������Ч�����ͼ�ֵ�����д�������һ������
//����2 ��ʾ������Ч�����ͼ�ֵ�����д�������һ������
//����1 ��ʾ������Ч�������������ݿ��Է���
//����0 ��ʾɨ��ʱ��δ����������ʹ������, Ҳ�����ͼ�ֵ
uint8_t  GetKeystate(void)
{
   uint8_t i;
   if(KeyScanCount==0)
   {
      KeyScanBuf[2]=KeyScanBuf[1];           
      KeyScanBuf[1]=KeyScanBuf[0];
      KeyScanBuf[0]=keyScan();
      KeyScanCount=KEYSCANTIME;
   
      if((KeyScanBuf[2]==KeyScanBuf[1])&&(KeyScanBuf[2]==KeyScanBuf[0]))  //ȥ��
      {
          KeyValueBuf[1]=KeyValueBuf[0];                         //�����ϴεļ�ֵ
          KeyValueBuf[0]=KeyScanBuf[0];                          //ȡ�õ�ǰ��ֵ
          if(KeyValueBuf[1]!=KeyValueBuf[0])                    //���������仯
          {
            if(KeyValueBuf[0]==0xff)
            { 
                KeyValue=0xff;
                return   1;                                     //�����ſ�
            }
            else
            {
                  KeyValue=KeyValueBuf[0];                    //������Ч
                  KeyContinuousCount=KEYCONTINUOUSTIME;       //����������ʱ
                  return  2;
            }
          }
          else                                               //�����ޱ仯
          {
            if(KeyContinuousCount==0)                    //�ж�����
            {
               for(i=0;i<NUM_KeyContinuous;i++)
               {
                   if(KeyValueBuf[0]==KeyValueContinuous[i]&&KeyRunContinuousCount==0) 
                   {
                      KeyValue=KeyValueBuf[0];
                      
                      return  3;
                   }
               }
               KeyValue=0xff;                           //������Ч����
               return  1; 
            }
            KeyValue=0xff;                              //δ������ʱ��
            return  1;    
         }
      }
       KeyValue=0xff;                              //����ȥ����
       return  1;
   }
   else                                            //ɨ��ʱ��δ�������������κ�����
   {
     KeyValue=0xff;
     return  0;
   } 
}
