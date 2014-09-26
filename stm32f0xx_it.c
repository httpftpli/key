/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.3.1
  * @date    17-January-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "beep.h"
#include "keyscan.h"
#include "touch.h"
#include "uart.h"





/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */




void NMI_Handler(void) {
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void) {
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void) {
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void) {
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void) {
}




/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

void TIM14_IRQHandler(void) {
    if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
        if (TouchPra.TouchScanCount > 0) {
            TouchPra.TouchScanCount--;
        }
        //触摸INT信号10MS滤波
        if (TouchPra.TouchIntState) {
            if (GetTouchIntVaule() == 0) {
                if (TouchPra.TouchIntCount > 0) {
                    TouchPra.TouchIntCount--;
                } else {
                    TouchPra.TouchIntCount = TOUCHINTMAX;
                    TouchPra.TouchIntState = 0;
                }
            } else {
                TouchPra.TouchIntCount = TOUCHINTMAX;
                TouchPra.TouchIntState = 1;
                TouchPra.TouchState = 0;
            }


        } else {
            if (GetTouchIntVaule()) {
                //if(TouchPra.TouchIntCount>0)
                //	{
                //	  TouchPra.TouchIntCount--;
                //	}
                //else
                //{
                TouchPra.TouchIntCount = TOUCHINTMAX;
                TouchPra.TouchIntState = 1;
                TouchPra.TouchState = 0;
                //}
            } else {
                TouchPra.TouchIntCount = TOUCHINTMAX;
                TouchPra.TouchIntState = 0;
            }
        }
        /////////连击计时
        if (TouchPra.TouchContinuousCount > 0) {
            TouchPra.TouchContinuousCount--;
        }

        //不完整帧超时保护

        if (Usart2Pra.Usart2Rxing) {
            Usart2Pra.Usart2RxTimeOutCount++;
            if (Usart2Pra.Usart2RxTimeOutCount >= USART2RXTIMEOUTMAX) {
                Usart2_Rst();
            }
        }
        //按键扫描计时
        if (KeyScanCount > 0) {
            KeyScanCount--;
        }
        ////按键连击等待计时
        if (KeyContinuousCount > 0) {
            KeyContinuousCount--;
        }
        ////按键连击计时
        if (KeyRunContinuousCount > 0) {
            KeyRunContinuousCount--;
        }

        //蜂鸣器控制
        if (BeepCtrPra.BeepKeyCount > 0) {
            BeepCtrPra.BeepKeyCount--;
            BeepCtrPra.Beep_KeyOffEn = 1;
            BeepON();
        } else {
            if (BeepCtrPra.Beep_KeyOffEn) {
                BeepOFF();
                BeepCtrPra.Beep_KeyOffEn = 0;
            }
        }

        switch (BeepCtrPra.BeepMode) {
        case 0x00:
            {
                break;
            }
        case 0x01:
            {
                Alarm1BeepCtr();
                break;
            }
        case 0x02:
            {
                Alarm2BeepCtr();
                break;
            }
        }
    }
}


void USART2_IRQHandler(void) {
    //发送控制
    if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET) {
        /* Write one byte to the transmit data register */
        USART_SendData(USART2, Usart2Pra.Usart2SendBuff[Usart2Pra.Usart2SendNum]);
        Usart2Pra.Usart2SendNum++;
        if (Usart2Pra.Usart2SendBuff[1] < 0x80) {
            if (Usart2Pra.Usart2SendNum >= USART2_COM_SHORT_NUM) {
                /* Disable the EVAL_COM1 Transmit interrupt */
                USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
                Usart2Pra.Usart2SendNum = 0;
                Usart2Pra.Usart2SendBuye = 0;
            }
        } else {
            if (Usart2Pra.Usart2SendNum >= USART2_COM_LONG_NUM) {
                /* Disable the EVAL_COM1 Transmit interrupt */
                USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
                Usart2Pra.Usart2SendNum = 0;
                Usart2Pra.Usart2SendBuye = 0;
            }
        }
    }
    // USART in mode Receiver --------------------------------------------------//
    //接收控制
    else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        Usart2Pra.Usart2RxTimeOutCount = 0;
        //允许接收
        if (Usart2Pra.Usart2RxOK == 0) {
            if (Usart2Pra.Usart2Rxing == 0) {
                //判断帧头，不是的丢掉
                Usart2Pra.Usart2RxLen = 0;
                Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] = USART_ReceiveData(USART2);
                if (Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] == COM_ZHENSTART) {
                    Usart2Pra.Usart2RxLen++;
                    Usart2Pra.Usart2Rxing = 1;

                } else {
                    Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] = 0;

                }
            } else {
                Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] = USART_ReceiveData(USART2);
                //接收命令码，用回来判断帧长
                if (Usart2Pra.Usart2RxLen == 1) {
                    Usart2Pra.Usart2RxLen++;
                    return;
                } else if (Usart2Pra.Usart2RxLen > 1) {
                    //判断长短帧
                    if (Usart2Pra.Usart2RxBuff[1] < 0x80) {
                        if (Usart2Pra.Usart2RxLen >= USART2_COM_SHORT_NUM - 1) {
                            //判断短帧结构是否完整
                            if (Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] == COM_ZHENEND) {

                                Usart2Pra.Usart2RxOK = 1;
                            } else {
                                Usart2Pra.Usart2RxOK = 0;
                            }
                            Usart2Pra.Usart2RxLen = 0;
                            Usart2Pra.Usart2Rxing = 0;
                        } else {
                            Usart2Pra.Usart2RxLen++;
                        }

                    } else {
                        //判断长帧结构是否完整
                        if (Usart2Pra.Usart2RxLen >= USART2_COM_LONG_NUM - 1) {
                            if (Usart2Pra.Usart2RxBuff[Usart2Pra.Usart2RxLen] == COM_ZHENEND) {

                                Usart2Pra.Usart2RxOK = 1;
                            } else {
                                Usart2Pra.Usart2RxOK = 0;
                            }
                            Usart2Pra.Usart2RxLen = 0;
                            Usart2Pra.Usart2Rxing = 0;
                        } else {
                            Usart2Pra.Usart2RxLen++;
                        }
                    }
                }

            }
        }
        USART_ReceiveData(USART2);

    } else {
        //串口异常恢复
        Usart2_Rst();

    }






    /* Disable the EVAL_COM1 Receive interrupt */
    //USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/




