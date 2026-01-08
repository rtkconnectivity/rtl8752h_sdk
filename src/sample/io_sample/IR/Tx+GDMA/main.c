/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code of ir tx with dma.
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "rtl876x_gdma.h"
#include "rtl876x_ir.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "trace.h"


#define IR_TX_PIN                       P2_5
#define IR_TX_FIFO_THR_LEVEL            2
#define IR_DATA_SIZE_MAX                68

#define IO_TEST_GDMA_TRANSFER_SIZE              IR_DATA_SIZE_MAX

#define IO_TEST_GDMA_CHANNEL_MUM                1

#if (IO_TEST_GDMA_CHANNEL_MUM == 0)
#define IO_TEST_GDMA_Channel                GDMA_Channel0
#define IO_TEST_GDMA_Channel_IRQn           GDMA0_Channel0_IRQn
#define IO_TEST_GDMA_Channel_Handler        GDMA0_Channel0_Handler
#elif IO_TEST_GDMA_CHANNEL_MUM == 1
#define IO_TEST_GDMA_Channel                GDMA_Channel1
#define IO_TEST_GDMA_Channel_IRQn           GDMA0_Channel1_IRQn
#define IO_TEST_GDMA_Channel_Handler        GDMA0_Channel1_Handler
#elif IO_TEST_GDMA_CHANNEL_MUM == 2
#define IO_TEST_GDMA_Channel                GDMA_Channel2
#define IO_TEST_GDMA_Channel_IRQn           GDMA0_Channel2_IRQn
#define IO_TEST_GDMA_Channel_Handler        GDMA0_Channel2_Handler
#elif IO_TEST_GDMA_CHANNEL_MUM == 3
#define IO_TEST_GDMA_Channel                GDMA_Channel3
#define IO_TEST_GDMA_Channel_IRQn           GDMA0_Channel3_IRQn
#define IO_TEST_GDMA_Channel_Handler        GDMA0_Channel3_Handler
#endif

/**
  * @brief  IR data structure definition
  */
typedef struct
{
    /* Unit of carrierFreq is KHz */
    uint32_t    CarrierFreq;
    uint32_t    DataBuf[IR_DATA_SIZE_MAX];
    uint16_t    DataLen;
} IR_Data_TypeDef;

IR_Data_TypeDef IR_Send_Data;
uint32_t GDMA_Send_Buf[100];

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
void board_ir_init(void)
{
    Pad_Config(IR_TX_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

    Pinmux_Config(IR_TX_PIN, IRDA_TX);
}

/**
  * @brief  Initialize ir peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_ir_init(uint32_t vFreq)
{
    /* Enable ir clock */
    RCC_PeriphClockCmd(APBPeriph_IR, APBPeriph_IR_CLOCK, ENABLE);

    /* Initialize ir */
    IR_InitTypeDef IR_InitStruct;
    IR_StructInit(&IR_InitStruct);
    IR_InitStruct.IR_Freq           = vFreq;//vFreq;
    IR_InitStruct.IR_DutyCycle      = 3; /* !< 1/3 duty cycle */
    IR_InitStruct.IR_Mode           = IR_MODE_TX;
    IR_InitStruct.IR_TxInverse      = IR_TX_DATA_NORMAL;
    IR_InitStruct.IR_TxFIFOThrLevel = IR_TX_FIFO_THR_LEVEL;
    IR_InitStruct.IR_TxDmaEn        = ENABLE;
    IR_InitStruct.IR_TxWaterLevel   = 15;

    IR_Init(&IR_InitStruct);
}

/**
  * @brief  Initialize GDMA peripheral.
  * @param   No parameter.
  * @return  void
  */
void driver_ir_gdma_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef GDMA_InitStruct;

    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum         = IO_TEST_GDMA_CHANNEL_MUM;
    GDMA_InitStruct.GDMA_BufferSize         = IO_TEST_GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_DIR                = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_SourceInc          = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc     = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize         = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_DestinationDataSize    = GDMA_DataSize_Word;
    GDMA_InitStruct.GDMA_SourceMsize            = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize       = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr         = (uint32_t)(GDMA_Send_Buf);
    GDMA_InitStruct.GDMA_DestinationAddr    = (uint32_t)(&IR->TX_FIFO);//0x40016014;
    GDMA_InitStruct.GDMA_DestHandshake      = GDMA_Handshake_IR_TX;
    GDMA_Init(IO_TEST_GDMA_Channel, &GDMA_InitStruct);

    GDMA_INTConfig(IO_TEST_GDMA_CHANNEL_MUM, GDMA_INT_Transfer, ENABLE);

    /*-----------------GDMA IRQ init-------------------*/
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = IO_TEST_GDMA_Channel_IRQn;
    nvic_init_struct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&nvic_init_struct);

    GDMA_Cmd(IO_TEST_GDMA_CHANNEL_MUM, ENABLE);
}

void ir_demo(void)
{
    /* Data to send */
    IR_Send_Data.CarrierFreq = 38000;
    IR_Send_Data.DataLen = IO_TEST_GDMA_TRANSFER_SIZE;
    IR_Send_Data.DataBuf[0] =  0x80000000 | 0x200;
    IR_Send_Data.DataBuf[1] =  0x00000000 | 0x100;
    for (uint16_t i = 2; i < IR_Send_Data.DataLen - 1;)
    {
        IR_Send_Data.DataBuf[i] =  0x80000000 | (0x0A + i * 5);
        IR_Send_Data.DataBuf[i + 1] =  0x00000000 | (0x14 + i * 5);
        i += 2;
    }
    IR_Send_Data.DataBuf[IR_Send_Data.DataLen - 1] =  0x80000000 | 0x800;

    /* Test data buffer */
    for (uint32_t i = 0; i < IO_TEST_GDMA_TRANSFER_SIZE; i++)
    {
        GDMA_Send_Buf[i] = IR_Send_Data.DataBuf[i];
    }

    board_ir_init();
    driver_ir_init(IR_Send_Data.CarrierFreq);
    driver_ir_gdma_init();
    IR_Cmd(IR_MODE_TX, ENABLE);
}

/**
  * @brief  Entry of APP code
  * @return int (To avoid compile warning)
*/
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);
    __enable_irq();

    ir_demo();

    while (1)
    {
        ;

    }
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void IO_TEST_GDMA_Channel_Handler(void)
{
    GDMA_INTConfig(IO_TEST_GDMA_CHANNEL_MUM, GDMA_INT_Transfer, DISABLE);
    GDMA_Cmd(IO_TEST_GDMA_CHANNEL_MUM, DISABLE);
    DBG_DIRECT("IO_TEST_GDMA_Channel_Handler\r\n");
    GDMA_ClearINTPendingBit(IO_TEST_GDMA_CHANNEL_MUM, GDMA_INT_Transfer);
}

