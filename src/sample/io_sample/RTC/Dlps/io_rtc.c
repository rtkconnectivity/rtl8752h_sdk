/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
* @file     io_rtc.c
* @brief    This file provides demo code of rtc comparator.
* @author   Grace_yan
* @date     2023-03-24
* @version  v1.0
*****************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_rcc.h"
#include "rtl876x_nvic.h"
#include "trace.h"

#include "rtl876x_rtc.h"
#include "io_rtc.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/** Prescaler value.
  * 12 bits prescaler for COUNTER frequency (32768/(PRESCALER+1)).
  * If use internal 32KHz, (32000/(PRESCALER+1)).
  * Must be written when RTC 32-bit counter is stopped.
  */
#define RTC_PRESCALER_VALUE     (3200-1)//f = 10Hz
/* RTC has 4 comparators,comparator0~3 . */
#define RTC_COMP_VALUE_0        (2)
#define RTC_COMP_VALUE_1        (4)
#define RTC_COMP_VALUE_2        (6)
#define RTC_COMP_VALUE_3        (8)

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initialize rtc peripheral.
  * @param   No parameter.
  * @return  void
  */
void driver_rtc_init(void)
{
    RTC_DeInit();
    RTC_SetPrescaler(RTC_PRESCALER_VALUE);
    RTC_SetPreCompValue(0);

    RTC_INTConfig(RTC_INT_TICK, ENABLE);

//    RTC_SetCompValue(RTC_COMP0, RTC_COMP_VALUE_0);
//    RTC_INTConfig(RTC_INT_COMP0, ENABLE);
//    RTC_WKConfig(RTC_WK_COMP0, ENABLE);

//    RTC_SetCompValue(RTC_COMP1, RTC_COMP_VALUE_1);
//    RTC_INTConfig(RTC_INT_COMP1, ENABLE);
//    RTC_WKConfig(RTC_WK_COMP1, ENABLE);

//    RTC_SetCompValue(RTC_COMP2, RTC_COMP_VALUE_2);
//    RTC_INTConfig(RTC_INT_COMP2, ENABLE);
//    RTC_WKConfig(RTC_WK_COMP2, ENABLE);

//    RTC_SetCompValue(RTC_COMP3, RTC_COMP_VALUE_3);
//    RTC_INTConfig(RTC_INT_COMP3, ENABLE);
//    RTC_WKConfig(RTC_WK_COMP3, ENABLE);

    /* Config RTC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    RTC_NvCmd(ENABLE);
    RTC_SystemWakeupConfig(ENABLE);

    /* Start RTC */
    RTC_ResetCounter();
    RTC_Cmd(ENABLE);
}

/**
  * @brief  RTC handler function.
  * @param  None.
  * @return None.
  */
void RTC_Handler(void)
{
    DBG_DIRECT("RTC_Handler");

    /* RTC tick interrupt handle */
    if (RTC_GetINTStatus(RTC_INT_TICK) == SET)
    {
        DBG_DIRECT("RTC_INT_TICK");
        RTC_ClearINTPendingBit(RTC_INT_TICK);
    }
    /* RTC overflow interrupt handle */
    else if (RTC_GetINTStatus(RTC_INT_OVF) == SET)
    {
        // Add application code here
        DBG_DIRECT("RTC_INT_OVF");
        RTC_ClearINTPendingBit(RTC_INT_OVF);
        DBG_DIRECT("after  overflow  RTC_GetCounter %d", RTC_GetCounter());
    }

    else
    {
        if ((RTC_GetINTStatus(RTC_INT_COMP0) == SET) && (RTC_GetWakeupStatus(RTC_WK_COMP0) == SET))
        {
            DBG_DIRECT("RTC_INT_CMP_0");
            RTC_SetCompValue(RTC_COMP0, RTC_GetCounter() + RTC_COMP_VALUE_0);
            RTC_ClearINTPendingBit(RTC_INT_COMP0);
            RTC_ClearWakeupStatusBit(RTC_WK_COMP0);
        }
        else if ((RTC_GetINTStatus(RTC_INT_COMP1) == SET) && (RTC_GetWakeupStatus(RTC_WK_COMP1) == SET))
        {
            DBG_DIRECT("RTC_INT_CMP_1");
            RTC_SetCompValue(RTC_COMP1, RTC_GetCounter() + RTC_COMP_VALUE_1);
            RTC_ClearINTPendingBit(RTC_INT_COMP1);
            RTC_ClearWakeupStatusBit(RTC_WK_COMP1);
        }
        else if ((RTC_GetINTStatus(RTC_INT_COMP2) == SET) && (RTC_GetWakeupStatus(RTC_WK_COMP2) == SET))
        {
            DBG_DIRECT("RTC_INT_CMP_2");
            RTC_SetCompValue(RTC_COMP2, RTC_GetCounter() + RTC_COMP_VALUE_2);
            RTC_ClearINTPendingBit(RTC_INT_COMP2);
            RTC_ClearWakeupStatusBit(RTC_WK_COMP2);
        }
        else if ((RTC_GetINTStatus(RTC_INT_COMP3) == SET) && (RTC_GetWakeupStatus(RTC_WK_COMP3) == SET))
        {
            DBG_DIRECT("RTC_INT_CMP_3");
            RTC_SetCompValue(RTC_COMP3, RTC_GetCounter() + RTC_COMP_VALUE_3);
            RTC_ClearINTPendingBit(RTC_INT_COMP3);
            RTC_ClearWakeupStatusBit(RTC_WK_COMP3);
        }
        else
        {
            DBG_DIRECT("RTC_INT_NONE");
        }
    }
}
/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/
