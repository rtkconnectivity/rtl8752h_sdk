/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    IIC demo self test.
* @details
* @author   echo
* @date     2021-05-27
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
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

#define I2C_MASTER_SEND_SLAVE_RECEIVE   1
#define I2C_MASTER_RECEIVE_SLAVE_SEND   0
#define I2C_MASTER_REPEAT_READ          0

/* Globals ------------------------------------------------------------------*/
#define I2C_TX_FIFO_SIZE           24
#define TransferLength             24

uint8_t I2C_Rev_Data[255];
uint8_t I2C_Rev_Index;
uint8_t I2C_Send_Index = 0;
volatile uint8_t I2C_Rev_Data_Lenth;

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

    I2C_InitStruct.I2C_ClockSpeed       = 100000;
    I2C_InitStruct.I2C_DeviveMode       = I2C_DeviveMode_Master;
    I2C_InitStruct.I2C_AddressMode      = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress     = 0x50;
    I2C_InitStruct.I2C_RxThresholdLevel = 8;
    I2C_InitStruct.I2C_Ack              = I2C_Ack_Enable;

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

    I2C_InitStruct.I2C_ClockSpeed        = 100000;
    I2C_InitStruct.I2C_DeviveMode        = I2C_DeviveMode_Slave;
    I2C_InitStruct.I2C_AddressMode       = I2C_AddressMode_7BIT;
    I2C_InitStruct.I2C_SlaveAddress      = 0x50;
    I2C_InitStruct.I2C_RxThresholdLevel  = 8;
    I2C_InitStruct.I2C_Ack               = I2C_Ack_Enable;

    I2C_Init(I2C1, &I2C_InitStruct);
    I2C_Cmd(I2C1, ENABLE);
}

/**
  * @brief  Config i2c nvic.
  * @param  No parameter.
  * @return void
  */
void nvic_i2c_config(void)
{
    /* Detect stop signal */
    I2C_ClearINTPendingBit(I2C0, I2C_INT_STOP_DET);
    I2C_ClearINTPendingBit(I2C1, I2C_INT_STOP_DET);
    /* Detect read require signal */
    I2C_ClearINTPendingBit(I2C1, I2C_INT_RD_REQ);
    /* Detect rx fifo full signal */
    I2C_ClearINTPendingBit(I2C1, I2C_INT_RX_FULL);
    /* Detect rx transmission finished signal */
    I2C_ClearINTPendingBit(I2C1, I2C_INT_RX_DONE);

#if I2C_MASTER_SEND_SLAVE_RECEIVE
    I2C_INTConfig(I2C0, I2C_INT_STOP_DET, ENABLE);
    I2C_INTConfig(I2C1, I2C_INT_RX_FULL, ENABLE);
    I2C_INTConfig(I2C1,  I2C_INT_STOP_DET, ENABLE);
    I2C_INTConfig(I2C1,  I2C_INT_RX_DONE, ENABLE);
#endif

#if I2C_MASTER_RECEIVE_SLAVE_SEND
    I2C_INTConfig(I2C1, I2C_INT_RD_REQ, ENABLE);
#endif

#if I2C_MASTER_REPEAT_READ
    I2C_INTConfig(I2C0, I2C_INT_STOP_DET, ENABLE);
    I2C_INTConfig(I2C1, I2C_INT_RX_FULL, ENABLE);
    I2C_INTConfig(I2C1,  I2C_INT_STOP_DET, ENABLE);
    I2C_INTConfig(I2C1,  I2C_INT_RX_DONE, ENABLE);
    I2C_INTConfig(I2C1, I2C_INT_RD_REQ, ENABLE);
#endif

    /* Config I2C interrupt */
    NVIC_InitTypeDef NVIC_InitStruct;

    NVIC_InitStruct.NVIC_IRQChannel = I2C0_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = I2C1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
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

    /* Config i2c nvic */
    nvic_i2c_config();

    /* I2C master write */
    uint8_t write_data[TransferLength] ;

    for (uint8_t i = 0; i < TransferLength; i++)
    {
        write_data[i] = i;
    }

#if I2C_MASTER_SEND_SLAVE_RECEIVE
    I2C_MasterWrite(I2C0, write_data, TransferLength);
#endif

#if I2C_MASTER_RECEIVE_SLAVE_SEND
    /* I2C master read */
    uint8_t data[TransferLength] = {0};
    I2C_MasterRead(I2C0, data, TransferLength);

    for (uint32_t i = 0; i < TransferLength ; i++)
    {
        DBG_DIRECT("Master Read data = %d\n", data[i]);
    }
#endif

#if I2C_MASTER_REPEAT_READ
    /* I2C master repeat read */
    uint8_t tx_data[10] = {01, 02, 03, 04};
    uint8_t rx_data[TransferLength] = {0};
    I2C_RepeatRead(I2C0, tx_data, 4, rx_data, TransferLength);

    for (uint32_t i = 0; i < TransferLength ; i++)
    {
        DBG_DIRECT("Master Repeat Read data = %d\n", rx_data[i]);
    }
#endif
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

void I2C0_Handler(void)
{
    if (I2C_GetINTStatus(I2C0, I2C_INT_STOP_DET) == SET)
    {
        /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
        */
        DBG_DIRECT("I2C0 Stop signal detected");
        I2C_ClearINTPendingBit(I2C0, I2C_INT_STOP_DET);
    }
}

/**
  * @brief  I2C1 interrupt handle function.
  * @param  None.
  * @return None.
*/
void I2C1_Handler(void)
{
    uint16_t lenth;

    uint8_t send_data_buffer[100];
    for (uint32_t i = 0; i < TransferLength; i++)
    {
        send_data_buffer[i] = i + 10;
    }
    if (I2C_GetINTStatus(I2C1, I2C_INT_RD_REQ) == SET)
    {
        /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
        */
        DBG_DIRECT("Enter I2C1 interrupt I2C1_INT_RD_REQ!");

        for (uint32_t i = 0; i < TransferLength; i++)
        {
            I2C_SendCmd(I2C1, I2C_WRITE_CMD, send_data_buffer[i], I2C_STOP_DISABLE);
        }

        I2C_ClearINTPendingBit(I2C1, I2C_INT_RD_REQ);

    }

    if (I2C_GetINTStatus(I2C1, I2C_INT_STOP_DET) == SET)
    {
        /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
        */
        DBG_DIRECT("I2C1 Stop signal detected");

        /*read I2C receive data*/
        lenth = I2C_GetRxFIFOLen(I2C1);
        for (uint32_t i = 0; i < lenth; i++)
        {

            I2C_Rev_Data[I2C_Rev_Index++] = I2C_ReceiveData(I2C1);
        }
        I2C_Rev_Data_Lenth += lenth;
        I2C_ClearINTPendingBit(I2C1, I2C_INT_STOP_DET);

        DBG_DIRECT("Slave Recv Date Lenth = %d", I2C_Rev_Data_Lenth);

        for (uint32_t i = 0; i < I2C_Rev_Data_Lenth; i++)
        {
            DBG_DIRECT("I2C1_Slave_ReceiveData=%d\n", I2C_Rev_Data[i]);
        }

    }
    if (I2C_GetINTStatus(I2C1, I2C_INT_RX_DONE) == SET)
    {
        /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
        */
        DBG_DIRECT("I2C1 RX_DONE detected");
        I2C_ClearINTPendingBit(I2C1, I2C_INT_RX_DONE);

    }
    if (I2C_GetINTStatus(I2C1, I2C_INT_RX_FULL) == SET)
    {
        /* Notes: DBG_DIRECT is only used for debug demo, do not use in app!
           In app project, the APP_PRINT_INFO can be used.
        */
        DBG_DIRECT("I2C1 Rx Full detected");
        lenth = I2C_GetRxFIFOLen(I2C1);

        /*read I2C data*/
        for (uint32_t i = 0; i < lenth; i++)
        {
            I2C_Rev_Data[I2C_Rev_Index++] = I2C_ReceiveData(I2C1);
        }
        I2C_Rev_Data_Lenth += lenth;

        I2C_ClearINTPendingBit(I2C1, I2C_INT_RX_FULL);
    }

}
/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/
