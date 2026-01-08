/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      io_lpc.c
   * @brief
   * @author    yuan
   * @date      2018-05-04
   * @version   v0.1
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "io_lpc.h"
#include "app_task.h"

/**
  * @brief  initialization of pinmux settings and pad settings.
  * @param   No parameter.
  * @return  void
  */
void board_lpc_init(void)
{
    Pad_Config(LPC_CAPTURE_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(LPC_CAPTURE_PIN, IDLE_MODE);
}

/**
  * @brief  Initialize RTC peripheral.
  * @param   No parameter.
  * @return  void
  */
void driver_lpc_init(void)
{
    DBG_DIRECT("driver_lpc_init");
    LPC_DeInit();
    LPC_InitTypeDef LPC_InitStruct;
    LPC_StructInit(&LPC_InitStruct);

    LPC_InitStruct.LPC_Channel   = LPC_CAPTURE_CHANNEL;
    LPC_InitStruct.LPC_Edge      = LPC_VOLTAGE_DETECT_EDGE;
    LPC_InitStruct.LPC_Threshold = LPC_VOLTAGE_DETECT_THRESHOLD;
    LPC_Init(&LPC_InitStruct);
    LPC_Cmd(ENABLE);

    LPC_INTConfig(LPC_INT_LPCOMP_VOL, ENABLE);
}

void nvic_lpc_init(void)
{
    /* Config LPC interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = LPCOMP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    LPC_INTCmd(ENABLE);
}

/**
  * @brief  LPC battery detection interrupt handle function.
  * @param  None.
  * @return None.
  */
void LPCOMP_Handler(void)
{
    LPC_INTConfig(LPC_INT_LPCOMP_VOL, DISABLE);
    APP_PRINT_INFO0("LPCOMP_Handler\r\n");
    T_IO_MSG int_lpc_msg;

    int_lpc_msg.type = IO_MSG_TYPE_BAT_LPC;
    if (false == app_send_msg_to_apptask(&int_lpc_msg))
    {
        APP_PRINT_ERROR0("[io_lpc] LPCOMP_Handler: Send int_lpc_msg failed!");
        return;
    }
    //Add application code here
}

