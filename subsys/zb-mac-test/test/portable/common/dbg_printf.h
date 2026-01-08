/**************************************************************************//**
 * @file     dbg_printf.h
 * @brief    Header file for debug printf implementation used to output formatted log messages.
 * @author   chengruei.wei
 * @version  V1.00
 * @date     2024-09-19
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef __DBG_PRINTF_COMMON_H__
#define __DBG_PRINTF_COMMON_H__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "platform_port.h"

#define DBG_OUT_IF_NONE 0
#define DBG_OUT_IF_UART 1
#define DBG_OUT_IF_USB  2

extern uint8_t dbg_output_interface;

// wrapper (used as buffer) for output function type
typedef struct
{
    void (*fct)(char character, void *arg);
    void *arg;
} out_fct_wrap_type;

void dbg_init(void);
int dbg_snprintf(char *buffer, size_t count, const char *format, ...);
int dbg_printf(const char *fmt, ...);
int dbg_sprintf(char *buffer, const char *fmt, ...);


#endif /* __DBG_PRINTF_COMMON_H__ */
