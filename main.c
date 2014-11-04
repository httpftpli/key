

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "beep.h"
#include "keyscan.h"
#include "uart.h"
#include "touch.h"
#include "lcdbl.h"
#include "keyled.h"
#include "uart.h"
#include "timer.h"


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
    //系统外部时钟8M,PLL6倍，48M
    SystemCoreClockUpdate();
    timerTick05msInit();
    LcdBlInit();
    keyScanInit();
    beepInit();
    TouchInit();
    //Timer14ini();
    Usart2_ini();
    keyLedInit();

    //IWDG_Config();
    beep(100);

    while (1) {
        IWDG_ReloadCounter();
        beep(0);
        keyScan();
        if (keytouched) {
            beep(20);
        }
        TouchScan();
        if (keytouched||touched) {
            CMD_SendKeyTouch(keytouched ? keycode : 0xff,
                             touched ? touchpointX : 0xffff,
                             touched ? touchpointY : 0xffff);
        }
        cmdProcess();
        keyLedProcess();
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
