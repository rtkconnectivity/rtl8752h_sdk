/**************************************************************************//**
 * @file     zb_mp_entry.h
 * @brief    Zigbee MP test functions declaration and macros.
* @author    JustinWu
 * @version  V1.00
 * @date     2024-06-17
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


#ifndef _ZB_MP_ENTRY_H_
#define _ZB_MP_ENTRY_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"

typedef uint8_t (*zigbee_mp_handle_cmd_t)(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                          uint8_t *send_cmd_comp_event_flag);
typedef uint8_t (*zigbee_mp_handle_event_t)(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                            uint8_t *event_parameter);

extern zigbee_mp_handle_cmd_t zigbee_mp_cmd_handler;
extern zigbee_mp_handle_event_t zigbee_mp_event_handler;

extern void zigbee_mp_handler_hook(zigbee_mp_handle_cmd_t cmd_handler,
                                   zigbee_mp_handle_event_t event_handler);

#ifdef __cplusplus
}
#endif

#endif    // end of #ifndef _ZB_MP_ENTRY_H_
