/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of i2s receive data.
* @details
* @author   yuan
* @date     2021-06-29
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_i2s.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "trace.h"

/* Defines -------------------------------------------------------------------*/
#define I2S_LRCK_PIN                P3_2
#define I2S_BCLK_PIN                P3_3
#define I2S_DATA_RX_PIN             P4_0

#define I2S_NUM                     I2S0
#define I2S_LRCK_PINMUX             LRC_SPORT0
#define I2S_BCLK_PINMUX             BCLK_SPORT0
#define I2S_DATA_RX_PINMUX          ADCDAT_SPORT0

#define APB_I2S                     APBPeriph_I2S0
#define APB_I2S_CLOCK               APBPeriph_I2S0_CLOCK

#define I2S_CLK_SOURCE_REG_224 *((volatile uint32_t *)0x40000224UL)

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return Void
  */
void board_i2s_init(void)
{
    Pad_Config(I2S_BCLK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(I2S_LRCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    Pad_Config(I2S_DATA_RX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
               PAD_OUT_LOW);

    Pinmux_Config(I2S_BCLK_PIN, I2S_BCLK_PINMUX);
    Pinmux_Config(I2S_LRCK_PIN, I2S_LRCK_PINMUX);
    Pinmux_Config(I2S_DATA_RX_PIN, I2S_DATA_RX_PINMUX);
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
    I2S_InitStruct.I2S_BClockNi         = 0x10;     /* <!BCLK = 1024K */
    I2S_InitStruct.I2S_DeviceMode       = I2S_DeviceMode_Slave;
    I2S_InitStruct.I2S_ChannelType      = I2S_Channel_stereo;
    I2S_InitStruct.I2S_DataWidth        = I2S_Width_16Bits;
    I2S_InitStruct.I2S_DataFormat       = I2S_Mode;
    I2S_InitStruct.I2S_DMACmd           = I2S_DMA_DISABLE;
    I2S_Init(I2S_NUM, &I2S_InitStruct);
    /* Change the I2S transmission into external codec .*/
    I2S_CLK_SOURCE_REG_224 |= BIT(3);
//    DBG_DIRECT("I2S_CLK_SOURCE_REG_224 = 0x%X",I2S_CLK_SOURCE_REG_224);
    I2S_Cmd(I2S_NUM, I2S_MODE_RX, ENABLE);
    I2S_INTConfig(I2S_NUM, I2S_INT_RX_READY, ENABLE);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = I2S0_RX_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/**
  * @brief  Demo code of I2S communication.
  * @param  No parameter.
  * @return Void
*/
void i2s_demo(void)
{
    board_i2s_init();
    driver_i2s_init();
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
  * @brief  I2S0 interrupt handle function.
  * @param  None.
  * @return None.
*/
void I2S0_RX_Handler(void)
{
    /* Notes: DBG_DIRECT is only used in debug demo, do not use in app project.*/
    DBG_DIRECT("I2S0_RX_Handler");
    if (I2S_GetINTStatus(I2S_NUM, I2S_INT_RX_READY))
    {
        uint8_t len = I2S_GetRxFIFOLen(I2S_NUM);
        DBG_DIRECT("length = %d", len);
        for (uint8_t i = 0; i < len; i++)
        {
            DBG_DIRECT("data[%d] = 0x%X", i, I2S_ReceiveData(I2S_NUM));
        }
        I2S_ClearINTPendingBit(I2S_NUM, I2S_INT_RX_READY);
    }
}

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/
