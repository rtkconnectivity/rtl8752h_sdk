/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code to realize RTC tick function.
            RTC is a 32-bit counter.
* @details
* @author   yuan
* @date     2018-05-25
* @version  v1.0
*********************************************************************************************************
*/

/* Defines ------------------------------------------------------------------*/
/** Prescaler value.
  * 12 bits prescaler for COUNTER frequency (32768/(PRESCALER+1)).
  * If use internal 32KHz, (32000/(PRESCALER+1)).
  * Must be written when RTC 32-bit counter is stopped.
  */
#define RTC_PRESCALER_VALUE     (3200-1) //f = 10Hz

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_rtc.h"
#include "trace.h"

/**
  * @brief  Initialize rtc peripheral.
  * @param   No parameter.
  * @return  void
  */
void driver_rtc_init(void)
{
    RTC_DeInit();
    RTC_SetPrescaler(RTC_PRESCALER_VALUE);

    RTC_INTConfig(RTC_INT_TICK, ENABLE);

    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
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
  * @brief    Entry of app code
  * @return   int (To avoid compile warning)
  */
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
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

void RTC_Handler(void)
{
    /* RTC tick interrupt handle */
    if (RTC_GetINTStatus(RTC_INT_TICK) == SET)
    {
        /* Notes: DBG_DIRECT function is only used for debugging demonstrations, not for application projects.*/
        DBG_DIRECT("[main]RTC_Handler: RTC_INT_TICK");
        // Add application code here
        RTC_ClearTickINT();
    }
}

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/

