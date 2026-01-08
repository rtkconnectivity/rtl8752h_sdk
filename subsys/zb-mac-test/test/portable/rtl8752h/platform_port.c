/**************************************************************************//**
 * @file     platform_port.c
 * @brief    Source file for BLE peripheral project, mainly used for initialize modules
 * @author   chengruei.wei
 * @version  V1.00
 * @date     2022-09-21
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "mac_test_config.h"
#include <trace.h>
#include "osif.h"
#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
//#include "rtl876x_gpio.h"
#include "rtl876x_uart.h"
#include "rtl876x_tim.h"
#include "rtl876x_nvic.h"
#include "vector_table.h"
#include "platform_port.h"
#include "zb_main.h"
#include "stdio_port.h"
#include "shell.h"
#include "protothread.h"
#include <stdarg.h> /* va_list, va_arg() */
//#include <stdio.h>
#include <string.h>


/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/

/*============================================================================*
 *                              Variables
 *============================================================================*/


/*============================================================================*
 *                              Functions
 *============================================================================*/
/**
 * @brief    Contains the initialization of pinmux settings and pad settings
 * @note     All the pinmux settings and pad settings shall be initiated in this function,
 *           but if legacy driver is used, the initialization of pinmux setting and pad setting
 *           should be peformed with the IO initializing.
 * @return   void
 */


/**
 * @brief    Contains the initialization of peripherals
 * @note     Both new architecture driver and legacy driver initialization method can be used
 * @return   void
 */

void zb_pin_mux_init(void)
{
    // UART for CLI/DBG
    Pad_Config(ZB_DBG_UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(ZB_DBG_UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(ZB_DBG_UART_TX_PIN, ZB_DBG_UART_TX);
    Pinmux_Config(ZB_DBG_UART_RX_PIN, ZB_DBG_UART_RX);
}


void ZB_DBG_UART_Handler(void)
{
    uint32_t int_status;
    uint16_t rx_len;
    uint8_t tmp;
    int_status = UART_GetIID(ZB_DBG_UART);
    UART_INTConfig(ZB_DBG_UART, UART_INT_RD_AVA, DISABLE);
    switch (int_status & 0x0E)
    {
    case UART_INT_ID_RX_LEVEL_REACH:
    case UART_INT_ID_RX_TMEOUT:
        rx_len = UART_GetRxFIFOLen(ZB_DBG_UART);
        while (rx_len--)
        {
            tmp = (uint8_t)ZB_DBG_UART->RB_THR;
            if (ringbuf_put(&rxbuf_ring, tmp) == 0)
            {
                DBG_DIRECT("rxbuf full");
            }
        }
        if (rx_len)
        {
            osif_task_notify_give(zb_task_handle);
        }
        break;

    default:
        break;
    }
    UART_INTConfig(ZB_DBG_UART, UART_INT_RD_AVA, ENABLE);
}

void zb_periheral_drv_init(void)
{
    ringbuf_init(&rxbuf_ring, rxbuf, RX_BUF_SIZE);
    RCC_PeriphClockCmd(APBPeriph_UART2, APBPeriph_UART2_CLOCK, ENABLE);
    UART_InitTypeDef UART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    UART_StructInit(&UART_InitStruct);
    // Baud rate = 2000000
    UART_InitStruct.div         = 2;
    UART_InitStruct.ovsr        = 5;
    UART_InitStruct.ovsr_adj     = 0;
    UART_InitStruct.rxTriggerLevel  = 1;
    UART_InitStruct.idle_time       = UART_RX_IDLE_2BYTE;      //idle interrupt wait time
    UART_InitStruct.dmaEn          = UART_DMA_ENABLE;
    UART_InitStruct.TxWaterlevel   = 15;     //Better to equal TX_FIFO_SIZE(16)- GDMA_MSize
    UART_InitStruct.RxWaterlevel   = 1;      //Better to equal GDMA_MSize
    UART_InitStruct.TxDmaEn   = ENABLE;
    UART_Init(ZB_DBG_UART, &UART_InitStruct);
    UART_INTConfig(ZB_DBG_UART, UART_INT_RD_AVA, ENABLE);
    NVIC_InitStruct.NVIC_IRQChannel = ZB_DBG_UART_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);
    RamVectorTableUpdate(ZB_DBG_UART_VECTORn, ZB_DBG_UART_Handler);
    DBG_DIRECT("RamVectorTableUpdate");
}

void stdio_putc_wrap(void *adapter, const char data)
{
    while (UART_GetFlagState((UART_TypeDef *)adapter, UART_FLAG_THR_EMPTY) == 0);
    UART_SendByte(adapter, data);
}

void _out_char(char character, void *buffer, size_t idx, size_t maxlen)
{
    (void)buffer; (void)idx; (void)maxlen;
    while (UART_GetFlagState(ZB_DBG_UART, UART_FLAG_THR_EMPTY) == 0);
    UART_SendByte(ZB_DBG_UART, character);
}

#if TEST_GPIO_DBG_EN
void debug_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    Pad_Config(GPIO_OUTPUT_PIN_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GPIO_OUTPUT_PIN_0, DWGPIO);
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(GPIO_OUTPUT_PIN_0);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
    GPIO_WriteBit(GPIO_GetPin(GPIO_OUTPUT_PIN_0), (BitAction)(0));

    Pad_Config(GPIO_OUTPUT_PIN_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pinmux_Config(GPIO_OUTPUT_PIN_1, DWGPIO);
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin    = GPIO_GetPin(GPIO_OUTPUT_PIN_1);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
    GPIO_Init(&GPIO_InitStruct);
    GPIO_WriteBit(GPIO_GetPin(GPIO_OUTPUT_PIN_1), (BitAction)(0));
    dbg_printf("debug_gpio_init\r\n");
}

void debug_gpio_toggle(uint32_t GPIO_Pin)
{
    uint8_t output_bit = GPIO_ReadOutputDataBit(GPIO_GetPin(GPIO_Pin));

    if (output_bit)
    {
        GPIO_WriteBit(GPIO_GetPin(GPIO_Pin), (BitAction)(0));
    }
    else
    {
        GPIO_WriteBit(GPIO_GetPin(GPIO_Pin), (BitAction)(1));
    }
}

void debug_gpio_high(uint32_t GPIO_Pin)
{
    GPIO_WriteBit(GPIO_GetPin(GPIO_Pin), (BitAction)(1));
}

void debug_gpio_low(uint32_t GPIO_Pin)
{
    GPIO_WriteBit(GPIO_GetPin(GPIO_Pin), (BitAction)(0));
}

uint8_t debug_gpio_get(uint32_t GPIO_Pin)
{
    return GPIO_ReadOutputDataBit(GPIO_GetPin(GPIO_Pin));
}
#endif

/** @} */ /* End of group PERIPH_DEMO_MAIN */
