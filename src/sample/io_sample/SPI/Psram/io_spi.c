/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     io_spi.c
* @brief    This file provides demo code of meomory to memory transfer by gdma.
* @details
* @author   yuan
* @date     2019-01-11
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "io_spi.h"
#include "io_uart.h"


/* Globals ------------------------------------------------------------------*/
uint8_t GDMA_WriteCmdBuffer[5] = {SPI_PSRAM_WRITE, 0x01, 0x02, 0x00};
uint8_t GDMA_ReadCmdBuffer[5] = {SPI_PSRAM_FAST_READ, 0x01, 0x02, 0x00, DUMMY_DATA};

uint8_t GDMA_Send_Buffer[GDMA_TRANSFER_SIZE + 5];
uint8_t GDMA_Recv_Buffer[GDMA_TRANSFER_SIZE];

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_spi_init(void)
{
    Pad_Config(SPI_SCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI_MOSI_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI_MISO_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI_CS_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    Pinmux_Config(SPI_SCK_PIN, SPI0_CLK_MASTER);
    Pinmux_Config(SPI_MOSI_PIN, SPI0_MO_MASTER);
    Pinmux_Config(SPI_MISO_PIN, SPI0_MI_MASTER);
    Pinmux_Config(SPI_CS_PIN, SPI0_SS_N_0_MASTER);
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_spi_gdma_tx_init(void)
{
    /*----------------test buffer init------------------*/
    memcpy(GDMA_Send_Buffer, GDMA_WriteCmdBuffer, 4);
    for (uint32_t i = 4; i < GDMA_TRANSFER_SIZE + 4; i++)
    {
        GDMA_Send_Buffer[i] = (i + 0x0C) & 0xFF;
    }

    SPI_InitTypeDef  SPI_InitStructure;
    GDMA_InitTypeDef GDMA_InitStruct;

    SPI_DeInit(SPI0);
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

    /*----------------------SPI init---------------------------------*/
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_TxOnly;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler = 100;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_InitStructure.SPI_NDF         = 0;
    SPI_InitStructure.SPI_TxDmaEn     = ENABLE;
    SPI_InitStructure.SPI_TxWaterlevel = 15;
    SPI_Init(SPI0, &SPI_InitStructure);

    /*---------------------GDMA initial------------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_SPI_TX_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = GDMA_TRANSFER_SIZE + 4;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;

    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)GDMA_Send_Buffer;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)SPI0->DR;
    GDMA_InitStruct.GDMA_DestHandshake       = GDMA_Handshake_SPI0_TX;
    GDMA_InitStruct.GDMA_ChannelPriority     = 1;

    GDMA_Init(GDMA_Channel_Tx, &GDMA_InitStruct);

    /*-----------------GDMA IRQ-----------------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GDMA_Channel_Tx_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Enable transfer interrupt */
    GDMA_INTConfig(GDMA_SPI_TX_NUM, GDMA_INT_Transfer, ENABLE);
    SPI_Cmd(SPI0, ENABLE);
    GDMA_Cmd(GDMA_SPI_TX_NUM, ENABLE);
}

void driver_spi_gdma_rx_init(void)
{
    /*----------------test buffer init------------------*/
    memset(GDMA_Recv_Buffer, 0, sizeof(GDMA_Recv_Buffer));

    SPI_InitTypeDef  SPI_InitStructure;
    GDMA_InitTypeDef GDMA_InitStruct;

    SPI_DeInit(SPI0);

    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

    /*----------------------SPI init---------------------------------*/
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction   = SPI_Direction_EEPROM;
    SPI_InitStructure.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL        = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA        = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_BaudRatePrescaler = 100;
    SPI_InitStructure.SPI_FrameFormat = SPI_Frame_Motorola;
    SPI_InitStructure.SPI_NDF         = GDMA_TRANSFER_SIZE - 1;
    SPI_InitStructure.SPI_RxDmaEn     = ENABLE;
    SPI_InitStructure.SPI_RxWaterlevel = 3;
    SPI_Init(SPI0, &SPI_InitStructure);
    SPI_SetRxSampleDly(SPI0, 1);

    /*---------------------GDMA initial------------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = GDMA_SPI_RX_NUM;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_PeripheralToMemory;
    GDMA_InitStruct.GDMA_BufferSize          = GDMA_TRANSFER_SIZE;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Inc;

    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;

    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)SPI0->DR;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)GDMA_Recv_Buffer;
    GDMA_InitStruct.GDMA_SourceHandshake     = GDMA_Handshake_SPI0_RX;
    GDMA_InitStruct.GDMA_ChannelPriority     = 1;

    GDMA_Init(GDMA_Channel_Rx, &GDMA_InitStruct);

    /*-----------------GDMA IRQ-----------------------------*/
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = GDMA_Channel_Rx_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* Enable transfer interrupt */
    GDMA_INTConfig(GDMA_SPI_RX_NUM, GDMA_INT_Transfer, ENABLE);
    SPI_Cmd(SPI0, ENABLE);
    GDMA_Cmd(GDMA_SPI_RX_NUM, ENABLE);

    /* Send read data command */
    SPI_SendBuffer(SPI0, GDMA_ReadCmdBuffer, 5);
}

/**
  * @brief  Handle gdma data function.
  * @param  No parameter.
  * @return void
  */
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg)
{
    uint8_t sub_type = io_gdma_msg->subtype;
    uint8_t *p_buf = io_gdma_msg->u.buf;
    if (sub_type == 0)
    {
        APP_PRINT_INFO0("[io_gdma] io_handle_gdma_msg: send data complete");
        driver_spi_gdma_rx_init();
    }
    else if (sub_type == 1)
    {
        APP_PRINT_INFO1("[io_gdma] io_handle_gdma_msg: read data complete data_len = %d",
                        GDMA_TRANSFER_SIZE);
        uart_senddata_continuous(UART0, p_buf, GDMA_TRANSFER_SIZE);
    }
}

/**
  * @brief  Demo code of operation about spi.
  * @param  No parameter.
  * @return void
*/
void spi_demo(void)
{
    APP_PRINT_INFO0("[io_spi] spi_psram_demo ");
    driver_spi_gdma_tx_init();
}

/**
  * @brief  GDMA channel interrupt handler function.
  * @param  No parameter.
  * @return void
  */
void GDMA_SPI_TX_Handler(void)
{
    GDMA_INTConfig(GDMA_SPI_TX_NUM, GDMA_INT_Transfer, DISABLE);

    T_IO_MSG int_gdma_msg;
    int_gdma_msg.type = IO_MSG_TYPE_GDMA;
    int_gdma_msg.subtype = 0;
    int_gdma_msg.u.buf = (void *)GDMA_Send_Buffer;
    if (false == app_send_msg_to_apptask(&int_gdma_msg))
    {
        APP_PRINT_ERROR0("[io_gdma]GDMA_Channel_Handler: Send int_gdma_msg failed!");
        //Add user code here!
        GDMA_ClearINTPendingBit(GDMA_SPI_TX_NUM, GDMA_INT_Block);
        return;
    }

    GDMA_ClearINTPendingBit(GDMA_SPI_TX_NUM, GDMA_INT_Transfer);
    GDMA_Cmd(GDMA_SPI_TX_NUM, DISABLE);
}

void GDMA_SPI_RX_Handler(void)
{
    GDMA_INTConfig(GDMA_SPI_RX_NUM, GDMA_INT_Transfer, DISABLE);

    T_IO_MSG int_gdma_msg;
    int_gdma_msg.type = IO_MSG_TYPE_GDMA;
    int_gdma_msg.subtype = 1;
    int_gdma_msg.u.buf = (void *)GDMA_Recv_Buffer;
    if (false == app_send_msg_to_apptask(&int_gdma_msg))
    {
        APP_PRINT_ERROR0("[io_gdma]GDMA_Channel_Handler: Send int_gdma_msg failed!");
        //Add user code here!
        GDMA_ClearINTPendingBit(GDMA_SPI_RX_NUM, GDMA_INT_Transfer);
        return;
    }

    GDMA_ClearINTPendingBit(GDMA_SPI_RX_NUM, GDMA_INT_Transfer);
}

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/
