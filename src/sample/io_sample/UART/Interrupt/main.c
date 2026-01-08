/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    uart demo interrupt
* @details
* @author   yuan
* @date     2018-06-28
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"
#include "trace.h"

/* Defines ------------------------------------------------------------------*/
#define UART_TX_PIN                P3_0
#define UART_RX_PIN                P3_1

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
uint8_t UART_Recv_Buf[256];
uint8_t UART_Send_Buf[256];
uint16_t UART_Recv_Buf_Lenth = 0;
bool receive_flag = false;

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
    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    /* uart init */
    UART_InitTypeDef UART_InitStruct;
    UART_StructInit(&UART_InitStruct);

    UART_Init(UART0, &UART_InitStruct);

    //enable rx interrupt and line status interrupt
    UART_INTConfig(UART0, UART_INT_RD_AVA, ENABLE);
    UART_INTConfig(UART0, UART_INT_RX_IDLE, ENABLE);

    /*  Enable UART IRQ  */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel         = UART0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);
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
    uint16_t demoStrLen = 0;

    board_uart_init();
    driver_uart_init();

    char *demoStr = "### Uart interrupt demo ###\r\n";
    demoStrLen = strlen(demoStr);
    memcpy(String_Buf, demoStr, demoStrLen);
    uart_senddata_continuous(UART0, String_Buf, demoStrLen);

    /* Loop rx and tx */
    while (1)
    {
        if (receive_flag == true)
        {
            receive_flag = false;
            uart_senddata_continuous(UART0, UART_Send_Buf, UART_Recv_Buf_Lenth);

            for (uint16_t i = 0; i < UART_Recv_Buf_Lenth; i++)
            {
                UART_Recv_Buf[i] = 0;
            }
            UART_Recv_Buf_Lenth = 0;
        }
    }
}

/**
  * @brief  main()
  * @param  No parameter.
  * @return void
*/
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    __enable_irq();
    uart_demo();
}

/**
  * @brief  UART_Handler.
  * @param  No parameter.
  * @return void
*/
void UART0_Handler()
{
    uint16_t lenth = 0;
    uint32_t int_status = UART_GetIID(UART0);

    UART_INTConfig(UART0, UART_INT_RD_AVA, DISABLE);

    if (UART_GetFlagStatus(UART0, UART_FLAG_RX_IDLE) == SET)
    {
        UART_INTConfig(UART0, UART_INT_RX_IDLE, DISABLE);

        UART_ClearRxFIFO(UART0);
        UART_INTConfig(UART0, UART_INT_RX_IDLE, ENABLE);
        receive_flag = true;
    }

    switch (int_status & 0x0E)
    {
    case UART_INT_ID_RX_DATA_TIMEOUT:
        {
            DBG_DIRECT("UART_INT_ID_RX_TMEOUT");
            lenth = UART_GetRxFIFODataLen(UART0);
            UART_ReceiveData(UART0, UART_Recv_Buf, lenth);
            for (uint8_t i = 0; i < lenth; i++)
            {
                DBG_DIRECT("data=0x%x", UART_Recv_Buf[i]);
                UART_Send_Buf[UART_Recv_Buf_Lenth + i] = UART_Recv_Buf[i];
            }
            UART_Recv_Buf_Lenth += lenth;
            break;
        }
    case UART_INT_ID_LINE_STATUS:
        break;

    case UART_INT_ID_RX_LEVEL_REACH:
        {
            DBG_DIRECT("UART_INT_ID_RX_LEVEL_REACH");

            lenth = UART_GetRxFIFODataLen(UART0);
            UART_ReceiveData(UART0, UART_Recv_Buf, lenth);
            for (uint8_t i = 0; i < lenth; i++)
            {
                DBG_DIRECT("data=0x%x", UART_Recv_Buf[i]);
                UART_Send_Buf[UART_Recv_Buf_Lenth + i] = UART_Recv_Buf[i];
            }
            UART_Recv_Buf_Lenth += lenth;
            break;
        }
    case UART_INT_ID_TX_EMPTY:
        break;

    case UART_INT_ID_MODEM_STATUS:
        break;

    default:
        break;
    }

    UART_INTConfig(UART0, UART_INT_RD_AVA, ENABLE);
}

