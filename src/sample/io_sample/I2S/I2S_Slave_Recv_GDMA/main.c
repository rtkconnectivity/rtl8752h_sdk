/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of i2s receive data with dma.
* @details
* @author   echo
* @date     2021-07-21
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_i2s.h"
#include "rtl876x_nvic.h"
#include "rtl876x_uart.h"
#include "rtl876x_gdma.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "trace.h"

/* Defines -------------------------------------------------------------------*/
#define I2S_LRCK_PIN                P3_2
#define I2S_BCLK_PIN                P3_3
#define I2S_DATA_TX_PIN             P2_0

#define I2S_NUM                     I2S0
#define I2S_LRCK_PINMUX             LRC_SPORT0
#define I2S_BCLK_PINMUX             BCLK_SPORT0
#define I2S_DATA_RX_PINMUX          ADCDAT_SPORT0

#define APB_I2S                     APBPeriph_I2S0
#define APB_I2S_CLOCK               APBPeriph_I2S0_CLOCK

#define I2S_CLK_SOURCE_REG_224 *((volatile uint32_t *)0x40000224UL)

/* GDMA RX defines */
#define GDMA_Channel_NUM           0
#define GDMA_Channel               GDMA_Channel0
#define GDMA_Channel_IRQn          GDMA0_Channel0_IRQn
#define GDMA_Channel_Handler       GDMA0_Channel0_Handler


#define UART_TX_PIN                     P2_4
#define UART_RX_PIN                     P2_5

void board_uart_init(void)
{
    Pad_Config(UART_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(UART_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    Pinmux_Config(UART_TX_PIN, UART0_TX);
    Pinmux_Config(UART_RX_PIN, UART0_RX);
}
/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return Void
  */
void board_i2s_init(void)
{
    Pad_Config(I2S_BCLK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(I2S_LRCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(I2S_DATA_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_LOW);

    Pinmux_Config(I2S_BCLK_PIN, I2S_BCLK_PINMUX);
    Pinmux_Config(I2S_LRCK_PIN, I2S_LRCK_PINMUX);
    Pinmux_Config(I2S_DATA_TX_PIN, I2S_DATA_RX_PINMUX);
}

/**
  * @brief  Initialize I2S peripheral.
  * @param  No parameter.
  * @return Void
  */
void driver_i2s_init(void)
{
    RCC_PeriphClockCmd(APB_I2S, APB_I2S_CLOCK, ENABLE);

    I2S_InitTypeDef I2S_InitStruct;

    I2S_StructInit(&I2S_InitStruct);
    I2S_InitStruct.I2S_ClockSource      = I2S_CLK_40M;
    /* BCLK = 40MHz*(ni/mi),  LRCK = BCLK/64 */
    I2S_InitStruct.I2S_BClockMi         = 0x271;    /* <!LRCK = 16K */
    I2S_InitStruct.I2S_BClockNi         = 0x30;     /* <!BCLK = 1024K */
    I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Slave;
    I2S_InitStruct.I2S_ChannelType      = I2S_Channel_stereo;
    I2S_InitStruct.I2S_DataWidth        = I2S_Width_16Bits;
    I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
    I2S_InitStruct.I2S_DMACmd           = I2S_DMA_ENABLE;
    I2S_Init(I2S_NUM, &I2S_InitStruct);

    /* Set SPORT0 with external codec */
    I2S_CLK_SOURCE_REG_224 |= BIT(3);

    I2S_Cmd(I2S_NUM, I2S_MODE_RX, ENABLE);
}

void driver_uart_init(void)
{
    /* Enable clock */
    RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);

    /* UART init */
    UART_InitTypeDef UART_InitStruct;

    UART_StructInit(&UART_InitStruct);
    /* change to 3M baudrate */
    UART_InitStruct.div             = 1;
    UART_InitStruct.ovsr            = 8;
    UART_InitStruct.ovsr_adj        = 0x492;
    UART_InitStruct.TxWaterlevel    = 12;

    UART_InitStruct.TxDmaEn         = ENABLE;
    UART_InitStruct.dmaEn           = UART_DMA_ENABLE;
    UART_Init(UART0, &UART_InitStruct);

}

void driver_gdma_init(void)
{
    /* Enable GDMA clock */
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* Initialize GDMA peripheral */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_Channel_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = 1000 / 4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)(&(I2S0->RX_DR));
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(UART0->RB_THR));
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_I2S0_RX;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_UART0_TX;
    GDMA_Init(GDMA_Channel, &GDMA_InitStruct);
    GDMA_INTConfig(GDMA_Channel_NUM, GDMA_INT_Transfer, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    GDMA_Cmd(GDMA_Channel_NUM, ENABLE);
}

/**
  * @brief  Demo code of I2S communication.
  * @param  No parameter.
  * @return Void
*/
void i2s_demo(void)
{
    board_i2s_init();

    board_uart_init();

    driver_gdma_init();

    driver_i2s_init();

    driver_uart_init();
}

/**
  * @brief    Entry of app code
  * @return   int (To avoid compile warning)
  */
int main(void)
{
    __enable_irq();
    i2s_demo();

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
  * @brief  I2S0_RX_Handler.
  * @param  No parameter.
  * @return void
*/
void I2S0_RX_Handler(void)
{
//    DBG_DIRECT("In I2S0_RX_Handler");

    if (I2S_GetINTStatus(I2S_NUM, I2S_INT_RX_READY))
    {
        uint8_t len = I2S_GetRxFIFOLen(I2S_NUM);
//        DBG_DIRECT("length = %d", len);
//        for (uint8_t i = 0; i < len; i++)
//        {
//            DBG_DIRECT("data[%d] = 0x%X", i, I2S_ReceiveData(I2S_NUM));
//        }
        I2S_ClearINTPendingBit(I2S_NUM, I2S_INT_RX_READY);
    }
}


void GDMA_Channel_Handler(void)
{
//    DBG_DIRECT("in gdma handler");
    GDMA_SetSourceAddress(GDMA_Channel, (uint32_t)(&(I2S0->RX_DR)));
    GDMA_SetDestinationAddress(GDMA_Channel, (uint32_t)(&(UART0->RB_THR)));

    GDMA_SetBufferSize(GDMA_Channel, 1000 / 4);

    GDMA_ClearINTPendingBit(GDMA_Channel_NUM, GDMA_INT_Transfer);
    GDMA_Cmd(GDMA_Channel_NUM, ENABLE);
}


/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/

