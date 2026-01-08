/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*********************************************************************************************************
* @file     xiaomi_gpio_test.h
* @brief
* @details
* @author
* @date
* @version  v1.0
*********************************************************************************************************
*/

#ifndef __GPIO_MP_TEST_H
#define __GPIO_MP_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_gpio.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "trace.h"


/* Globals ------------------------------------------------------------------*/
extern uint8_t GPIO_Test_Result[12];

uint8_t *mp_test_gpio_forward(void);
uint8_t *mp_test_gpio_backward(void);

#ifdef __cplusplus
}
#endif

#endif
