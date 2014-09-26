/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/main.c
  * @author  MCD Application Team
  * @version V1.3.1
  * @date    17-January-2014
  * @brief   Main program body
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
#include "main.h"
#include "beep.h"
#include "keyscan.h"
#include "uart.h"
#include "touch.h"
#include "lcdled.h"
#include "keyled.h"


const uint8_t Ver[5] = { "01100" };
/** @addtogroup STM32F0xx_StdPeriph_Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


int main(void) {
    uint8_t  state, i;

    //系统外部时钟8M,PLL6倍，48M
    SystemInit();
    LcdBlInit();
    keyScanInit();
    beepInit();
    TouchInit();
    Timer14ini();
    Usart2_ini();
    keyLedInit();




    KeyValue = 0xff;
    //IWDG_Config();
    BeepCtrPra.BeepKeyCount = 200;
    BeepON();

    /* Infinite loop */

    while (1) {

        //扫描按键状态

        IWDG_ReloadCounter();
        TouchScan();
        state = GetKeystate();
        switch (state) {
        case  0x00:
            {
                break;
            }

        case  0x01:
            {
                if (TouchPra.TouchDataState) {
                    while (Usart2Pra.Usart2SendBuye) {
                    }
                    CMD_SendKeyTouch(SENDTOUCHNONLY);
                    TouchPra.TouchDataState = 0;
                }
                break;
            }
        case  0x02:
            {
                if (TouchPra.TouchDataState) {
                    while (Usart2Pra.Usart2SendBuye) {
                    }
                    CMD_SendKeyTouch(SENDTOUCHNANDKEY);
                    TouchPra.TouchDataState = 0;
                } else {
                    while (Usart2Pra.Usart2SendBuye) {
                    }
                    CMD_SendKeyTouch(SENDKEYNONLY);
                }
                BeepCtrPra.BeepKeyCount = KEY_BEEP_TIME;
                break;
            }
        case  0x03:
            {
                if (TouchPra.TouchDataState) {
                    while (Usart2Pra.Usart2SendBuye) {
                    }
                    CMD_SendKeyTouch(SENDTOUCHNANDKEY);
                    TouchPra.TouchDataState = 0;
                } else {
                    while (Usart2Pra.Usart2SendBuye) {
                    }
                    CMD_SendKeyTouch(SENDKEYNONLY);
                }
                BeepCtrPra.BeepKeyCount = KEYRUNCONTINUOUSTIME / 2;
                KeyRunContinuousCount = KEYRUNCONTINUOUSTIME;
                break;
            }
        default:
            {

                break;
            }

        }
        //接收完成倒出命令内容
        if (Usart2Pra.Usart2RxOK) {
            for (i = 0; i < USART2_COM_LONG_NUM; i++) {
                Usart2Pra.Usart2CmdBuff[i] = Usart2Pra.Usart2RxBuff[i];
                Usart2Pra.Usart2RxOK = 0;
                Usart2Pra.Usart2CmdRun = 1;
            }

        }
        //执行命令缓冲区中的命令
        if (Usart2Pra.Usart2CmdRun) {
            switch (Usart2Pra.Usart2CmdBuff[1]) {
            case CMD_SETALRMMODE:
                {
                    CMD_SetAlarmMode();
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            case CMD_BEEPON:
                {
                    CMD_BeepOn();
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            case CMD_READVER:
                {
                    CMD_ReadVer();
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            case CMD_SETTOUCHMISS:
                {
                    CMD_SetTouchMiss();
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            case CMD_GOBOOT:
                {
                    CMD_GoBoot();
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            case CMD_LCDBLCTR:
                {
                    CMD_LcdBlCtr();
                    Usart2Pra.Usart2CmdRun = 0;
                }
            case CMD_SETKEYLED :
                keyLedSet(Usart2Pra.Usart2CmdBuff[3] << 8 | Usart2Pra.Usart2CmdBuff[2]);
                Usart2Pra.Usart2CmdRun = 0;
                break;


            default :
                {
                    Usart2Pra.Usart2CmdRun = 0;
                    break;
                }
            }
        }
    }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif

/**
  * @}
  */
//////////////////////////////////



void Timer14ini(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = T10msCNT;
    TIM_TimeBaseStructure.TIM_Prescaler = 47; //48MHz / (47+1) = 1MHz about 1us
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);

    TIM_ITConfig(TIM14, TIM_EventSource_Update, ENABLE);
    TIM_Cmd(TIM14, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    /* Enable the TIM2 gloabal Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure);
}

void IWDG_Config(void) {
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
      dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                            = 250ms / (LSI/32)
                            = 0.25s / (LsiFreq/32)
                            = LsiFreq/(32 * 4)
                            = LsiFreq/128
                            LsiFreq=40k;
     */
    IWDG_SetReload(312);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();

}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
