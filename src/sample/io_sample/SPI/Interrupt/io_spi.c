/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_spi.c
* @brief    This file provides demo code of spi master.
            GD25Q128E is slave.
            Read the chip id of GD25Q128E.
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "io_spi.h"

#include "spi_flash.h"
#include "app_task.h"
#include "trace.h"

/* Globals ------------------------------------------------------------------*/

uint8_t flash_id_type = 0;
PMCheckResult IO_SPI_DLPS_Enter_Allowed = PM_CHECK_FAIL;

/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
*/
void board_spi_init(void)
{
    Pad_Config(SPI0_SCK_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI0_MOSI_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI0_MISO_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(SPI0_CS_PIN, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
	
    Pinmux_Deinit(P4_0);
    Pinmux_Deinit(P4_1);
    Pinmux_Deinit(P4_2);
    Pinmux_Deinit(P4_3);

    Pinmux_Config(SPI0_SCK_PIN, SPI0_CLK_MASTER);
    Pinmux_Config(SPI0_MOSI_PIN, SPI0_MO_MASTER);
    Pinmux_Config(SPI0_MISO_PIN, SPI0_MI_MASTER);
    Pinmux_Config(SPI0_CS_PIN, SPI0_SS_N_0_MASTER);
}

/**
  * @brief  Initialize spi peripheral.
  * @param  No parameter.
  * @return void
  */
void driver_spi_init(void)
{
    RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);

    SPI_InitTypeDef  SPI_InitStruct;
    SPI_StructInit(&SPI_InitStruct);

    SPI_InitStruct.SPI_Direction   = SPI_Direction_TxOnly;
    SPI_InitStruct.SPI_Mode        = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize    = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL        = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA        = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_BaudRatePrescaler  = 64;
    SPI_InitStruct.SPI_FrameFormat = SPI_Frame_Motorola;

    SPI_Init(SPI0, &SPI_InitStruct);
    SPI_Cmd(SPI0, ENABLE);

    /* Allow DLPS after SPI init */
    IO_SPI_DLPS_Enter_Allowed = PM_CHECK_PASS;
}

/**
  * @brief  IO enter dlps call back function.
  * @param  No parameter.
  * @return void
  */
void io_spi_dlps_enter(void)
{
    DBG_DIRECT("[io_spi] enter dlps");
    /* Switch pad to Software mode */
    Pad_ControlSelectValue(SPI0_SCK_PIN, PAD_SW_MODE);
    Pad_ControlSelectValue(SPI0_MOSI_PIN, PAD_SW_MODE);
    Pad_ControlSelectValue(SPI0_MISO_PIN, PAD_SW_MODE);
    Pad_ControlSelectValue(SPI0_CS_PIN, PAD_SW_MODE);
}

/**
  * @brief  IO exit dlps call back function.
  * @param  No parameter.
  * @return void
  */
void io_spi_dlps_exit(void)
{
    /* Switch pad to Pinmux mode */
    Pad_ControlSelectValue(SPI0_SCK_PIN, PAD_PINMUX_MODE);
    Pad_ControlSelectValue(SPI0_MOSI_PIN, PAD_PINMUX_MODE);
    Pad_ControlSelectValue(SPI0_MISO_PIN, PAD_PINMUX_MODE);
    Pad_ControlSelectValue(SPI0_CS_PIN, PAD_PINMUX_MODE);
    DBG_DIRECT("[io_spi] exit dlps");
}

/**
  * @brief  IO enter dlps check function.
  * @param  No parameter.
  * @return PMCheckResult
  */
PMCheckResult io_spi_dlps_check(void)
{
    return IO_SPI_DLPS_Enter_Allowed;
}

/**
  * @brief  Demo code of operation about spi.
  * @param  No parameter.
  * @return void
*/
void spi_demo(void)
{
    //uint8_t id[10];
    //APP_PRINT_INFO0("[io_spi] spi_demo: Read flash id.");
    //spi_flash_read_id(DEVICE_ID, id);
    //flash_id_type = 0;
}

/**
  * @brief  Handle i2c data function.
  * @param  No parameter.
  * @return void
  */
void io_spi_handle_msg(T_IO_MSG *io_spi_msg)
{
    uint8_t *p_buf = io_spi_msg->u.buf;
    uint8_t data_lenth = p_buf[0];
    APP_PRINT_INFO2("[io_spi] io_spi_handle_msg: data_lenth = %d, data = %b ", data_lenth,
                    TRACE_BINARY(data_lenth, &p_buf[1]));
    uint8_t id[1];
    flash_id_type++;
    if (flash_id_type < 3)
    {
        spi_flash_read_id((Flash_ID_Type)flash_id_type, id);
    }

    /* Allow DLPS after SPI operation is handled */
    IO_SPI_DLPS_Enter_Allowed = PM_CHECK_PASS;
}

/**
  * @brief  Handle i2c data function.
  * @param  No parameter.
  * @return void
  */
void io_handle_spi_msg(T_IO_MSG *io_spi_msg)
{
    io_spi_handle_msg(io_spi_msg);
}


/**
  * @brief  SPI0 interrupt handle function.
  * @param  None.
  * @return None.
  */
void SPI0_Handler(void)
{
    volatile uint8_t data_len = 0;
    volatile uint8_t SPI_ReadINTBuf[70] = {0};

    if (SPI_GetINTStatus(SPI0, SPI_INT_RXF) == SET)
    {
        SPI_ClearINTPendingBit(SPI0, SPI_INT_RXF);

        while (SPI_GetRxFIFOLen(SPI0) < Flash_ID_Length);
        data_len = SPI_GetRxFIFOLen(SPI0);
        Flash_Data[0] = data_len;

        for (uint8_t i = 0; i < data_len; i++)
        {
            /* Must read all data in receive FIFO , otherwise cause SPI_INT_RXF interrupt again. */
            Flash_Data[1 + i] = SPI_ReceiveData(SPI0);
        }

        T_IO_MSG int_spi_msg;
        int_spi_msg.type = IO_MSG_TYPE_SPI;
        int_spi_msg.subtype = 0;
        int_spi_msg.u.buf = (void *)(Flash_Data);
        if (false == app_send_msg_to_apptask(&int_spi_msg))
        {
            APP_PRINT_ERROR0("[io_spi] SPI0_Handler: Send int_spi_msg failed!");
            SPI_ClearINTPendingBit(SPI0, SPI_INT_RXF);
            //Add user code here!
            return;
        }
    }
}
