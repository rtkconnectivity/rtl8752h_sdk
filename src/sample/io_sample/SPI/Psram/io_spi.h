/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_spi.h
* @brief
* @details
* @author   yuan
* @date     2019-01-11
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __IO_GDMA_H
#define __IO_GDMA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtl876x_gdma.h"
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_spi.h"
#include "trace.h"

#include "app_task.h"
#include "board.h"


/* Defines ------------------------------------------------------------------*/
#define SPI_PSRAM_WRITE                 0x02
#define SPI_PSRAM_READ_DATA             0x03
#define SPI_PSRAM_FAST_READ             0x0B
#define SPI_PSRAM_READ_ID               0x9F

#define DUMMY_DATA                      0x55

#define GDMA_TRANSFER_SIZE              1000

void board_spi_init(void);
void io_handle_gdma_msg(T_IO_MSG *io_gdma_msg);
void spi_demo(void);

#ifdef __cplusplus
}
#endif

#endif

