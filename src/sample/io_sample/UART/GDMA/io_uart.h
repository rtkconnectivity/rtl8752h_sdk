/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_uart.h
* @brief
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __IO_ADC_H
#define __IO_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"

#include "app_msg.h"
#include "board.h"

/* Defines ------------------------------------------------------------------*/
#define UART_TX_PIN                         P3_0
#define UART_RX_PIN                         P3_1

#define UART_NUM                            0
#define UART_BAUDRATE                       (BAUD_RATE_115200)

#define EVB_ENABLE                          1
#define EVB_DISABLE                         0
#define UART_GDMA_TX_ENABLE                 EVB_ENABLE
#define UART_GDMA_RX_ENABLE                 EVB_ENABLE

#define UART_TX_GDMA_CHANNEL_NUM            0
#define UART_TX_GDMA_CHANNEL                GDMA_Channel0
#define UART_TX_GDMA_CHANNEL_IRQN           GDMA0_Channel0_IRQn
#define UART_TX_GDMA_BUFFER_SIZE            4095//max size=65535
#define UART_TX_GDMA_Handler                GDMA0_Channel0_Handler

#define UART_RX_GDMA_CHANNEL_NUM            1
#define UART_RX_GDMA_CHANNEL                GDMA_Channel1
#define UART_RX_GDMA_CHANNEL_IRQN           GDMA0_Channel1_IRQn
#define UART_RX_GDMA_Handler                GDMA0_Channel1_Handler
#define UART_RX_GDMA_BUFFER_SIZE            4095//max size=65535

typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRateValue;


void board_uart_init(void);
void driver_uart_init(void);
void driver_gdma_uart_init(void);
//void io_handle_uart_msg(T_IO_MSG *io_adc_msg);
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg);
void io_uart_demo(void);

#ifdef __cplusplus
}
#endif

#endif

