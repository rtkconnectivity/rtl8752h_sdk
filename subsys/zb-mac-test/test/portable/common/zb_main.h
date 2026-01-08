/**************************************************************************//**
* @file      main.h
* @brief    Header file for BLE peripheral project, mainly used for initialize modules
* @author   jane
* @version  V1.0
* @date     2017-06-12
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

#ifndef _ZB_MAIN_H_
#define _ZB_MAIN_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "dbg_printf.h"

#define RX_BUF_SIZE    128
extern uint8_t rxbuf[RX_BUF_SIZE];
extern struct ringbuf rxbuf_ring;

extern void *zb_sem;
extern void *zb_task_handle;

extern void shell_cmd_init(void);

int stdio_getc_wrap(void *adapter, char *data);
extern void shell_register_test_cmd(void);
void shell_register_priv_cmd(void) __attribute__((weak));
void shell_register_user_cmd(void) __attribute__((weak));
extern void stdio_putc_wrap(void *adapter, const char data);
void zb_test_task(void *p_param);
void zb_task_init(void);
extern void mac_Initialize_Patch(void);

#ifdef __cplusplus
}
#endif

#endif /* _ZB_MAIN_H_ */
