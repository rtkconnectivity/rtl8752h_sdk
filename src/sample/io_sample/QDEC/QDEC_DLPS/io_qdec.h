/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     io_qdec.h
* @brief
* @details
* @author   yuan
* @date     2018-12-07
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __IO_QDEC_H
#define __IO_QDEC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_nvic.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_qdec.h"
#include "rtl876x_rcc.h"

#include "board.h"

#include "app_msg.h"
#include "dlps.h"

/* Globals ------------------------------------------------------------------*/

/* QDEC data struct*/
typedef struct
{

    uint16_t AxisDirection;
    uint16_t AxisCount;
} QDEC_Data_TypeDef;

extern QDEC_Data_TypeDef   Y_Axis_Data;


extern PMCheckResult IO_QDEC_DLPS_Enter_Allowed;

void global_data_qdec_init(void);
PMCheckResult io_qdec_dlps_check(void);
void io_qdec_dlps_enter(void);
void io_qdec_dlps_exit(void);
void board_qdec_init(void);
void driver_qdec_init(void);
void io_handle_qdec_msg(T_IO_MSG *io_qdec_msg);


#ifdef __cplusplus
}
#endif

#endif

