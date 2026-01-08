/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    uart demo polling tx and rx.
* @details
* @author   yuan
* @date     2018-06-28
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"

/* Defines ------------------------------------------------------------------*/
#define UART_TX_PIN                P3_0
#define UART_RX_PIN                P3_1


/** @brief  UART_BaudRate_Table
  *         div ovsr ovsr_adj :These three parameters set the baud rate calibration parameters of UART.
    baudrate    |   div     |   ovsr    |   ovsr_adj
--------------------------------------------------------
    1200Hz      |   2400    |   8       |   0x3EF
    2400Hz      |   1200    |   8       |   0x3EF
    4800Hz      |   600     |   8       |   0x3EF
    9600Hz      |   300     |   8       |   0x3EF
    14400Hz     |   250     |   6       |   0x010
    19200Hz     |   150     |   8       |   0x3EF
    28800Hz     |   125     |   6       |   0x010
    38400Hz     |   75      |   8       |   0x3EF
    57600Hz     |   50      |   8       |   0x3EF
    76800Hz     |   43      |   7       |   0x010
    115200Hz    |   20      |   12      |   0x252
    128000Hz    |   19      |   11      |   0x2AA
    153600Hz    |   17      |   10      |   0x292
    230400Hz    |   11      |   10      |   0x3BB
    460800Hz    |   6       |   9       |   0x0AA
    500000Hz    |   8       |   5       |   0
    921600Hz    |   3       |   9       |   0x0AA
    1000000Hz   |   4       |   5       |   0
    1382400Hz   |   2       |   9       |   0x0AA
    1444400Hz   |   2       |   8       |   0x3F7
    1500000Hz   |   2       |   8       |   0x292
    2000000Hz   |   2       |   5       |   0
    3000000Hz   |   1       |   8       |   0x292
-----------------------------------------------------
*/ /* End of UART_BaudRate_Table */

/* Globals ------------------------------------------------------------------*/
typedef struct
{
    uint16_t div;
    uint16_t ovsr;
    uint16_t ovsr_adj;
} UART_BaudRate_TypeDef;

typedef enum
{
    BAUD_RATE_9600,
    BAUD_RATE_19200,
    BAUD_RATE_115200,
    BAUD_RATE_230400,
    BAUD_RATE_256000,
    BAUD_RATE_384000,
    BAUD_RATE_460800,
    BAUD_RATE_921600,
    BAUD_RATE_1000000,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000
} Uart_BaudRate_TypeDef;

const UART_BaudRate_TypeDef BaudRate_Table[11] =
{
    {271, 10, 0x24A}, // BAUD_RATE_9600
    {150, 8,  0x3EF}, // BAUD_RATE_19200
    {20, 12,  0x252}, // BAUD_RATE_115200
    {11,  10, 0x3BB}, // BAUD_RATE_230400
    {11,  9,  0x084}, // BAUD_RATE_256000
    {7,   9,  0x3EF}, // BAUD_RATE_384000
    {6,   9,  0x0AA}, // BAUD_RATE_460800
    {3,   9,  0x0AA}, // BAUD_RATE_921600
    {4,   5,  0},     // BAUD_RATE_1000000
    {2,   5,  0},     // BAUD_RATE_2000000
    {1,   8,  0x292}, // BAUD_RATE_3000000
};

uint8_t String_Buf[100];

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_uart_init(void)
{
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pinmux_Config(UART_TX_PIN, UART0_TX);
    Pinmux_Config(UART_RX_PIN, UART0_RX);
}

/**
  * @brief  Initialize uart peripheral.
  * @param  No parameter.
  * @return void
*/
void driver_uart_init(void)
{
    UART_DeInit(UART0);

    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    /* uart init */
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    /* Config uart baudrate */
    UART_InitStruct.UART_Div            = BaudRate_Table[BAUD_RATE_115200].div;
    UART_InitStruct.UART_Ovsr           = BaudRate_Table[BAUD_RATE_115200].ovsr;
    UART_InitStruct.UART_OvsrAdj        = BaudRate_Table[BAUD_RATE_115200].ovsr_adj;

    UART_Init(UART0, &UART_InitStruct);
}

/**
  * @brief  UARt send data continuous.
  * @param  No parameter.
  * @return void
*/
void uart_senddata_continuous(UART_TypeDef *UARTx, const uint8_t *pSend_Buf, uint16_t vCount)
{
    uint8_t count;

    while (vCount / UART_TX_FIFO_SIZE > 0)
    {
        while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
        for (count = UART_TX_FIFO_SIZE; count > 0; count--)
        {
            UARTx->RB_THR = *pSend_Buf++;
        }
        vCount -= UART_TX_FIFO_SIZE;
    }

    while (UART_GetFlagStatus(UARTx, UART_FLAG_TX_FIFO_EMPTY) == 0);
    while (vCount--)
    {
        UARTx->RB_THR = *pSend_Buf++;
    }
}

/**
  * @brief  Demo code of uart.
  * @param  No parameter.
  * @return void
*/
void uart_demo(void)
{
    uint16_t demo_str_len = 0;
    uint8_t rx_byte = 0;

    board_uart_init();
    driver_uart_init();

    char *demo_str = "### Uart demo polling read uart data ###\r\n";
    demo_str_len = strlen(demo_str);
    memcpy(String_Buf, demo_str, demo_str_len);

    /* Send demo tips */
    uart_senddata_continuous(UART0, String_Buf, demo_str_len);

    /* Loop rx and tx */
    while (1)
    {
        if (UART_GetFlagStatus(UART0, UART_FLAG_RX_DATA_AVA) == SET)
        {
            rx_byte = UART_ReceiveByte(UART0);
            UART_SendByte(UART0, rx_byte);
        }
    }
}

/**
  * @brief    Entry of app code
  * @return   int (To avoid compile warning)
  */
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    uart_demo();

    while (1)
    {

    }
}

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/
