/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
* @file      main.c
* @brief     Source file for BLE peripheral project, mainly used for initialize modules
* @author
* @date      2021-06-06
* @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2021 Realtek Semiconductor Corporation</center></h2>
**************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "rtl876x_enh_tim.h"
#include "rtl876x_gpio.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"

#include "trace.h"

/* Defines -------------------------------------------------------------------*/
#define ENHTIMER_NUM                    ENH_TIM0
#define ENHTIMER_IRQn                   ENHANCED_TIMER0_IRQn

#define GPIO_INPUT_PIN_0                P2_2
#define GPIO_PIN_INPUT_0                GPIO_GetPin(GPIO_INPUT_PIN_0)

#define GPIO_OUTPUT_PIN_0               P2_4
#define GPIO_PIN_OUTPUT_0               GPIO_GetPin(GPIO_OUTPUT_PIN_0)

/* Globals -------------------------------------------------------------------*/
uint8_t Latch_Count_Thd = 4;

/**
  * @brief  Initialize TIM-Enhance peripheral.
  * @param  No parameter.
  * @return None.
*/
void driver_enhance_timer_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_ENHTIMER, APBPeriph_ENHTIMER_CLOCK, ENABLE);

    ENHTIM_InitTypeDef ENHTIM_InitStruct;
    ENHTIM_StructInit(&ENHTIM_InitStruct);

    ENHTIM_InitStruct.ENHTIM_ClockDiv               = ENHTIM_CLOCK_DIVIDER_1;
    ENHTIM_InitStruct.ENHTIM_Mode                   = ENHTIM_MODE_FreeRun;
    /* Only enhtim_ ENHTIM_LatchCountEn[2] latch triggered by GPIO. */
    ENHTIM_InitStruct.ENHTIM_LatchCountEn[2]        = ENHTIM_LATCH_COUNT_ENABLE;
    ENHTIM_InitStruct.ENHTIM_LatchCountTrigger[2]   = ENHTIM_LATCH_TRIGGER_RISING_EDGE;
    ENHTIM_InitStruct.ENHTIM_LatchCount2Thd         = 3;
    ENHTIM_InitStruct.ENHTIM_LatchTriggerPad        = GPIO_INPUT_PIN_0;
    ENHTIM_InitStruct.ENHTIM_TimerGPIOTriggerEn     = ENABLE;

    ENHTIM_Init(ENHTIMER_NUM, &ENHTIM_InitStruct);

    /*  Enable TIMER IRQ  */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = ENHTIMER_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    ENHTIM_ClearINTPendingBit(ENHTIMER_NUM, ENHTIM_INT_TIM);
    ENHTIM_INTConfig(ENHTIMER_NUM, ENHTIM_INT_TIM, ENABLE);

    ENHTIM_ClearINTPendingBit(ENHTIMER_NUM, ENHTIM_INT_LATCH_CNT2_FIFO_FULL);
    ENHTIM_INTConfig(ENHTIMER_NUM, ENHTIM_INT_LATCH_CNT2_FIFO_FULL, ENABLE);

    ENHTIM_ClearINTPendingBit(ENHTIMER_NUM, ENHTIM_INT_LATCH_CNT2_FIFO_THD);
    ENHTIM_INTConfig(ENHTIMER_NUM, ENHTIM_INT_LATCH_CNT2_FIFO_THD, ENABLE);

    ENHTIM_Cmd(ENHTIMER_NUM, ENABLE);
}

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return None.
*/
void board_gpio_enhtim_init(void)
{
    Pad_Config(GPIO_INPUT_PIN_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GPIO_INPUT_PIN_0,  DWGPIO);
}

/**
  * @brief  Initialize GPIO peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_gpio_enhtim_init(void)
{
    /* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_PIN_INPUT_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_WriteBit(GPIO_PIN_OUTPUT_0, (BitAction)(1));
}

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return None.
*/
void board_gpio_init(void)
{
    Pad_Config(GPIO_OUTPUT_PIN_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GPIO_OUTPUT_PIN_0, DWGPIO);
}

/**
  * @brief  Initialize GPIO peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_gpio_init(void)
{
    /* Initialize GPIO peripheral */
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin  = GPIO_PIN_OUTPUT_0;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(&GPIO_InitStruct);

    GPIO_WriteBit(GPIO_PIN_OUTPUT_0, (BitAction)(1));
}

/**
  * @brief  Demo code of timer.
  * @param  No parameter.
  * @return void
*/
void enhance_tim_demo(void)
{
    board_gpio_enhtim_init();
    driver_gpio_enhtim_init();
    driver_enhance_timer_init();

    /* GPIO output is only used to simulate the input signal, only for demo debugging. */
    board_gpio_init();
    driver_gpio_init();
    while (1)
    {
        /* Simulate GPIO trigger signal */
        for (uint32_t i = 0; i < 100000; i++);
        GPIO_WriteBit(GPIO_PIN_OUTPUT_0, (BitAction)(1));
        for (uint32_t i = 0; i < 100000; i++);
        GPIO_WriteBit(GPIO_PIN_OUTPUT_0, (BitAction)(0));
    }
}

/**
 * @brief   Entry of APP code.
 * @return  int (To avoid compile warning).
 */
int main(void)
{
    __enable_irq();

    enhance_tim_demo();

    while (1)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

void Enhanced_Timer0_Handler()
{
    if (ENHTIM_GetINTStatus(ENH_TIM0, ENHTIM_INT_TIM))
    {
        APP_PRINT_INFO0("ENH_TIM0 ENHTIM_INT_TIM\r\n");
        //add user code here
        ENHTIM_ClearINTPendingBit(ENH_TIM0, ENHTIM_INT_TIM);
    }
    if (ENHTIM_GetINTStatus(ENH_TIM0, ENHTIM_INT_LATCH_CNT2_FIFO_FULL))
    {
        APP_PRINT_INFO0("ENH_TIM0 ENHTIM_INT_LATCH_CNT2_FIFO_FULL\r\n");
        //add user code here
        ENHTIM_ClearINTPendingBit(ENH_TIM0, ENHTIM_INT_LATCH_CNT2_FIFO_FULL);
    }
    if (ENHTIM_GetINTStatus(ENH_TIM0, ENHTIM_INT_LATCH_CNT2_FIFO_THD))
    {
        APP_PRINT_INFO0("ENH_TIM0 ENHTIM_INT_LATCH_CNT2_FIFO_THD\r\n");
        ENHTIM_INTConfig(ENHTIMER_NUM, ENHTIM_INT_LATCH_CNT2_FIFO_THD, DISABLE);
        uint8_t length = ENHTIM_GetLatchCount2FIFOLength(ENH_TIM0);
        uint32_t data[4] = {0};
        ENHTIM_ReadLatchCount2FIFO(ENH_TIM0, data, length);
        /* Only for debugging, removed in actual application. */
        APP_PRINT_INFO1("ENH_TIM0 fifo length = %d\r\n", length);
        for (uint8_t i = 0; i < length; i++)
        {
            /* Only for debugging, removed in actual application. */
            APP_PRINT_INFO2("ENH_TIM0 data[%d] = 0x%x\r\n", i, data[i]);
        }
        //add user code here
        ENHTIM_ClearINTPendingBit(ENH_TIM0, ENHTIM_INT_LATCH_CNT2_FIFO_THD);
        ENHTIM_INTConfig(ENH_TIM0, ENHTIM_INT_LATCH_CNT2_FIFO_THD, ENABLE);
    }
}
