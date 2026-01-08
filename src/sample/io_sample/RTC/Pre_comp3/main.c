/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code to realize RTC prescaler + comparator3 function.
            RTC is a 32-bit counter.
* @details
* @author   yuan
* @date     2018-05-25
* @version  v1.0
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
#define RTC_PRESCALER_VALUE     (3200 - 1)//max 4095
#define RTC_PRECOMP_VALUE       (320)//max 4095
#define RTC_COMP3_VALUE         (10)

/** Define prescaler & comparator3 interrupt.
  * 1:prescaler & comparator3 interrupt,
    (prescaler_value = prescaler_counter)&&(comparator3_value = counter),
    RTC_PRECOMP_VALUE <= RTC_PRESCALER_VALUE
  * 0:prescaler comparator interrupt,(prescaler_value = prescaler_counter)
  */
#define RTC_PRECOMP3_INT        1

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_rtc.h"
#include "trace.h"

/**
  * @brief  Initialize LPC peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_rtc_init(void)
{
    RTC_DeInit();

#if (RTC_PRECOMP3_INT)
    RTC_SetPrescaler(RTC_PRESCALER_VALUE);
    RTC_SetPreCompValue(RTC_PRECOMP_VALUE);
    RTC_SetCompValue(RTC_COMP3, RTC_COMP3_VALUE);

    RTC_INTConfig(RTC_INT_PRE_COMP3, ENABLE);

#else
    RTC_SetPrescaler(RTC_PRESCALER_VALUE);
    RTC_SetPreCompValue(RTC_PRECOMP_VALUE);

    RTC_INTConfig(RTC_INT_PRE_COMP, ENABLE);
#endif
    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RTC_NvCmd(ENABLE);
    /* Start RTC */
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
}

/**
  * @brief  Demo code of operation about RTC.
  * @param  No parameter.
  * @return void
*/
void rtc_demo(void)
{
    /* Initialize RTC peripheral */
    driver_rtc_init();

    //Add application code here
}

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
int main(void)
{
//    extern uint32_t random_seed_value;
//    srand(random_seed_value);
    __enable_irq();

    rtc_demo();

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

/**
  * @brief  RTC interrupt handle function.
  * @param  None.
  * @return None.
  */
void RTC_Handler(void)
{
    DBG_DIRECT("RTC_Handler");
#if (RTC_PRECOMP3_INT)
    if (RTC_GetINTStatus(RTC_INT_PRE_COMP3) == SET)
    {
        DBG_DIRECT("RTC_INT_PRE_COMP3");
        uint32_t counter  = RTC_GetCounter();
        RTC_ResetPrescalerCounter();
        RTC_SetCompValue(RTC_COMP3, counter + RTC_COMP3_VALUE);
        RTC_ClearINTPendingBit(RTC_INT_PRE_COMP3);
    }
#else
    if (RTC_GetINTStatus(RTC_INT_PRE_COMP) == SET)
    {
        DBG_DIRECT("RTC_INT_PRE_COMP");
        RTC_ResetPrescalerCounter();
        RTC_ClearINTPendingBit(RTC_INT_PRE_COMP);
    }
#endif
}

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/

