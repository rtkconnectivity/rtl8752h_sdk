/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     io_gdma.c
* @brief    This file provides demo code of meomory to memory transfer by gdma.
* @details
* @author   echo
* @date     2021-05-24
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "io_gdma.h"
#include "io_UART.h"


/* Globals ------------------------------------------------------------------*/
uint8_t GDMA_WriteCmdBuffer[5] = {0x9f};
uint8_t GDMA_Recv_Buffer[GDMA_TRANSFER_SIZE];

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_spi_gdma_init(void)
{
    /*----------------test buffer init------------------*/
    memset(GDMA_Recv_Buffer, 0, sizeof(GDMA_Recv_Buffer) / sizeof(GDMA_Recv_Buffer[0]));

    SPI_InitTypeDef  SPI_InitStructure;
    GDMA_InitTypeDef GDMA_InitStruct;

    SPI_DeInit(FLASH_SPI);
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

    /*----------------------SPI init---------------------------------*/
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_EEPROM;//SPI_Direction_RxOnly;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler = 100;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_InitStructure.SPI_NDF         = GDMA_READ_SIZE_MAX - 1;
    SPI_InitStructure.SPI_RxDmaEn     = ENABLE;
    SPI_InitStructure.SPI_TxDmaEn     = ENABLE;
    SPI_InitStructure.SPI_RxWaterlevel = 1;
    SPI_InitStructure.SPI_TxWaterlevel = 1;
    SPI_Init(FLASH_SPI, &SPI_InitStructure);
    /*---------------------TX GDMA initial------------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_TX_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = 0;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)GDMA_WriteCmdBuffer;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)FLASH_SPI->DR;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPI0_TX;
    GDMA_InitStruct.GDMA_ChannelPriority     = 1;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 0;

    GDMA_Init(GDMA_TX_Channel, &GDMA_InitStruct);

    /*-----------------GDMA IRQ-----------------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GDMA_TX_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Enable transfer interrupt */
    GDMA_INTConfig(GDMA_TX_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);

    /*---------------------RX GDMA initial------------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_RX_CHANNEL_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize          = GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_1;

    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)FLASH_SPI->DR;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)GDMA_Recv_Buffer;
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_SPI0_RX;
    GDMA_InitStruct.GDMA_ChannelPriority     = 1;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 0;

    GDMA_Init(GDMA_RX_Channel, &GDMA_InitStruct);

    /*-----------------GDMA IRQ-----------------------------*/
    NVIC_InitStruct.NVIC_IRQChannel = GDMA_RX_Channel_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Enable transfer interrupt */
    GDMA_INTConfig(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
    GDMA_Cmd(GDMA_RX_CHANNEL_NUM, ENABLE);

    SPI_Cmd(FLASH_SPI, ENABLE);

    /* Send read data command */
    GDMA_SetBufferSize(GDMA_TX_Channel, 1);
    GDMA_SetSourceAddress(GDMA_TX_Channel, (uint32_t)GDMA_WriteCmdBuffer);
    GDMA_Cmd(GDMA_TX_CHANNEL_NUM, ENABLE);

}

/**
  * @brief  Handle gdma data function.
  * @param  No parameter.
  * @return void
  */
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg)
{
    uint8_t *p_buf = io_gdma_msg->u.buf;
    uint16_t data_len = (GDMA_TRANSFER_SIZE);
    APP_PRINT_INFO1("[io_gdma] io_handle_gdma_msg: read data complete,data_len = %d", data_len);
    uart_senddata_continuous(UART0, p_buf, data_len);
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void GDMA_RX_Channel_Handler(void)
{
    GDMA_INTConfig(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);

    T_IO_MSG int_gdma_msg;

    int_gdma_msg.type = IO_MSG_TYPE_GDMA;
    int_gdma_msg.subtype = 0;
    int_gdma_msg.u.buf = (void *)GDMA_Recv_Buffer;
    if (false == app_send_msg_to_apptask(&int_gdma_msg))
    {
        APP_PRINT_ERROR0("[io_gdma]GDMA_Channel_Handler: Send int_gdma_msg failed!");
        //Add user code here!
        GDMA_ClearINTPendingBit(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer);
        return;
    }
    GDMA_ClearINTPendingBit(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer);
    GDMA_INTConfig(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}

void GDMA_TX_Channel_Handler(void)
{
    GDMA_INTConfig(GDMA_TX_CHANNEL_NUM, GDMA_INT_Transfer, DISABLE);
    DBG_DIRECT("[io_gdma]GDMA_TX_Channel_Handler: TX completed!");
    GDMA_ClearINTPendingBit(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer);
    GDMA_INTConfig(GDMA_RX_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
}
/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/
