/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    IIC demo GDMA master tx slave rx.
* @details
* @author   echo
* @date     2021-05-27
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gdma.h"
#include "rtl876x_rcc.h"
#include "rtl876x_i2c.h"
#include "trace.h"

/* Defines ------------------------------------------------------------------*/

/* I2C pin define*/

#define I2C_MASTER_SCL_PIN         I2C0_SCL_PIN
#define I2C_MASTER_SDA_PIN         I2C0_SDA_PIN

#define I2C_SLAVER_SCL_PIN         I2C1_SCL_PIN
#define I2C_SLAVE_SDA_PIN          I2C1_SDA_PIN

#define I2C0_SCL_PIN               P4_0
#define I2C0_SDA_PIN               P4_1

#define I2C1_SCL_PIN               P4_2
#define I2C1_SDA_PIN               P4_3

#define I2C0_GDMA_CHANNEL_NUM                 0
#define I2C0_GDMA_Channel                     GDMA_Channel0
#define I2C0_GDMA_Channel_IRQn                GDMA0_Channel0_IRQn
#define I2C0_GDMA_Channel_Handler             GDMA0_Channel0_Handler
#define I2C1_GDMA_CHANNEL_NUM                 1
#define I2C1_GDMA_Channel                     GDMA_Channel1
#define I2C1_GDMA_Channel_IRQn                GDMA0_Channel1_IRQn
#define I2C1_GDMA_Channel_Handler             GDMA0_Channel1_Handler

#define I2C0_READ_CMD_GDMA_CHANNEL_NUM        2
#define I2C0_READ_CMD_GDMA_Channel            GDMA_Channel2
#define I2C0_READ_CMD_GDMA_Channel_IRQn       GDMA0_Channel2_IRQn
#define I2C0_READ_CMD_GDMA_Channel_Handler    GDMA0_Channel2_Handler

#define GDMA_TRANSFER_SIZE                    200


/* Globals ------------------------------------------------------------------*/
uint16_t GDMA_I2C_Send_Buf[GDMA_TRANSFER_SIZE];
uint8_t GDMA_I2C_Recv_Buf[GDMA_TRANSFER_SIZE];
uint32_t GDMA_I2C_Tx_Read_Buf[GDMA_TRANSFER_SIZE];
/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_i2c_master_init(void)
{

    Pad_Config(I2C_MASTER_SCL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(I2C_MASTER_SDA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);

    Pinmux_Config(I2C_MASTER_SCL_PIN, I2C0_CLK);
    Pinmux_Config(I2C_MASTER_SDA_PIN, I2C0_DAT);
}

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void board_i2c_slave_init(void)
{
    Pad_Config(I2C_SLAVER_SCL_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);
    Pad_Config(I2C_SLAVE_SDA_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE,
               PAD_OUT_HIGH);

    Pinmux_Config(I2C_SLAVER_SCL_PIN, I2C1_CLK);
    Pinmux_Config(I2C_SLAVE_SDA_PIN, I2C1_DAT);
}

/**
  * @brief  Initialize I2C Master peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_i2c_master_init(void)
{
    /* Initialize I2C peripheral */
    RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);

    I2C_InitTypeDef  I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.I2C_ClockSpeed    = 100000;
    I2C_InitStruct.I2C_DeviveMode    = I2C_DeviveMode_Master;
    I2C_InitStruct.I2C_AddressMode   = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress  = 0x50;
    I2C_InitStruct.I2C_Ack           = I2C_Ack_Enable;
    I2C_InitStruct.I2C_RxDmaEn       = ENABLE;
    I2C_InitStruct.I2C_TxDmaEn       = ENABLE;
    I2C_InitStruct.I2C_RxWaterlevel  = 1;
    I2C_InitStruct.I2C_TxWaterlevel  = 23;
    I2C_Init(I2C0, &I2C_InitStruct);

    I2C_Cmd(I2C0, ENABLE);
}

/**
  * @brief  Initialize I2C Slave peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_i2c_slave_init(void)
{
    /* Initialize I2C peripheral */
    RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);

    I2C_InitTypeDef  I2C_InitStruct;
    I2C_StructInit(&I2C_InitStruct);

    I2C_InitStruct.I2C_ClockSpeed    = 100000;
    I2C_InitStruct.I2C_DeviveMode    = I2C_DeviveMode_Slave;
    I2C_InitStruct.I2C_AddressMode   = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress  = 0x50;
    I2C_InitStruct.I2C_Ack           = I2C_Ack_Enable;
    I2C_InitStruct.I2C_TxDmaEn       = ENABLE;
    I2C_InitStruct.I2C_TxWaterlevel  = 20;
    I2C_Init(I2C1, &I2C_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel         = I2C1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);

    /* Configure interrupt type, please reference to i2c document about all interrupt type description */
    I2C_INTConfig(I2C1, I2C_INT_RD_REQ, ENABLE);

    I2C_Cmd(I2C1, ENABLE);
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
*/
void gdma_i2c_slave_tx_init(void)
{
    /* Initialize data buffer */
    for (uint32_t i = 0; i < GDMA_TRANSFER_SIZE; i++)
    {
        GDMA_I2C_Send_Buf[i] = i;
    }

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* GDMA init */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);

    GDMA_InitStruct.GDMA_ChannelNum      = I2C1_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize      = GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_SourceMsize        = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize   = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)GDMA_I2C_Send_Buf;
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)(&(I2C1->IC_DATA_CMD));
    GDMA_InitStruct.GDMA_DestHandshake      = GDMA_Handshake_I2C1_TX;

    GDMA_Init(I2C1_GDMA_Channel, &GDMA_InitStruct);

    /* GDMA irq config */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel         = I2C1_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_INTConfig(I2C1_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
*/
void gdma_i2c_master_rx_init(void)
{
    /* Initialize data buffer */
    for (uint32_t i = 0; i < GDMA_TRANSFER_SIZE; i++)
    {
        GDMA_I2C_Recv_Buf[i] = 0;
    }

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    /* GDMA init */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);

    GDMA_InitStruct.GDMA_ChannelNum      = I2C0_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize      = GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize        = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize   = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)(&(I2C0->IC_DATA_CMD));
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)(GDMA_I2C_Recv_Buf);
    GDMA_InitStruct.GDMA_SourceHandshake    = GDMA_Handshake_I2C0_RX;

    GDMA_Init(I2C0_GDMA_Channel, &GDMA_InitStruct);

    /* GDMA irq config */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel         = I2C0_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_INTConfig(I2C0_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    /* Start to receive data */
    GDMA_Cmd(I2C0_GDMA_CHANNEL_NUM, ENABLE);
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
*/
void i2c_master_send_read_cmd(void)
{
    /* Initialize data buffer */
    for (uint32_t i = 0; i < (GDMA_TRANSFER_SIZE - 1); i++)
    {
        GDMA_I2C_Tx_Read_Buf[i] = (0x01 << 8);
    }
    GDMA_I2C_Tx_Read_Buf[GDMA_TRANSFER_SIZE - 1] = (0x03 << 8);

    /* GDMA init */
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);

    GDMA_InitStruct.GDMA_ChannelNum      = I2C0_READ_CMD_GDMA_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize      = GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize        = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize   = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)GDMA_I2C_Tx_Read_Buf;
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)(&(I2C0->IC_DATA_CMD));
    GDMA_InitStruct.GDMA_DestHandshake      = GDMA_Handshake_I2C0_TX;

    GDMA_Init(I2C0_READ_CMD_GDMA_Channel, &GDMA_InitStruct);

    /* GDMA irq config */
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel         = I2C0_READ_CMD_GDMA_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&NVIC_InitStruct);

    GDMA_INTConfig(I2C0_READ_CMD_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    GDMA_Cmd(I2C0_READ_CMD_GDMA_CHANNEL_NUM, ENABLE);
}

/**
  * @brief  Demo code of i2c.
  * @param  No parameter.
  * @return void
*/
void i2c_demo(void)
{
    /* Configure pad and pinmux firstly! */
    board_i2c_master_init();
    board_i2c_slave_init();

    /* Initialize i2c peripheral */
    driver_i2c_master_init();
    driver_i2c_slave_init();

    /* Initialize gdma peripheral */
    gdma_i2c_slave_tx_init();
    gdma_i2c_master_rx_init();

    i2c_master_send_read_cmd();
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
    i2c_demo();

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
  * @brief  I2C1 interrupt handler function.
  * @param  No parameter.
  * @return void
*/
void I2C1_Handler(void)
{
    /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
     */
//    DBG_DIRECT("I2C1_Handler!");
    if (I2C_GetINTStatus(I2C1, I2C_INT_RD_REQ) == SET)
    {
//        DBG_DIRECT("I2C1_Handler I2C_INT_RD_REQ!");
        GDMA_Cmd(I2C1_GDMA_CHANNEL_NUM, ENABLE);
        //Add user code here
        /* Clear interrupt */
        I2C_ClearINTPendingBit(I2C1, I2C_INT_RD_REQ);
        I2C_INTConfig(I2C1, I2C_INT_RD_REQ, DISABLE);
    }
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
*/
void I2C1_GDMA_Channel_Handler(void)
{
    GDMA_INTConfig(I2C1_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);
    /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
    */
    DBG_DIRECT("In I2C1_GDMA_Channel_Handler! I2C slave send data by GDMA successfully");
    //Add user code here
    GDMA_ClearINTPendingBit(I2C1_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
*/
void I2C0_GDMA_Channel_Handler(void)
{
    GDMA_INTConfig(I2C0_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);
    /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
    */
    DBG_DIRECT("I2C0_GDMA_Channel_Handler!I2C master read data by GDMA successfully");
    //Add user code here
    GDMA_ClearINTPendingBit(I2C0_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);

    for (uint32_t i = 0; i < GDMA_TRANSFER_SIZE; i++)
    {
        if (GDMA_I2C_Send_Buf[i] != GDMA_I2C_Recv_Buf[i])
        {
            DBG_DIRECT(" Data transmission error! GDMA_Send_Buf = %d, GDMA_Recv_Buf = %d",
                       GDMA_I2C_Send_Buf[i], GDMA_I2C_Recv_Buf[i]);
        }
        GDMA_I2C_Recv_Buf[i] = 0;
    }

}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
*/
void I2C0_READ_CMD_GDMA_Channel_Handler(void)
{
    GDMA_INTConfig(I2C0_READ_CMD_GDMA_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);
    /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
    */
    DBG_DIRECT("I2C0_READ_CMD_GDMA2_Channel_Handler!");
    //Add user code here
    GDMA_ClearINTPendingBit(I2C0_READ_CMD_GDMA_CHANNEL_NUM, GDMA_INT_Transfer);
}



/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/
