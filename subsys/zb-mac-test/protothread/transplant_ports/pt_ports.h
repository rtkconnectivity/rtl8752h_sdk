/**************************************************************************//**
 * @file      pt_ports.h
 * @brief     Header file for protothread porting
 * @author    felix
 * @version   V1.00
 * @date      2025-01-17
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

#ifndef _PT_PORTS_H_
#define _PT_PORTS_H_

#include <stdint.h>
typedef unsigned long clock_time_t;
#define PROCESS_CONF_NO_PROCESS_NAMES 0
#define RINGBUF_INTERRUPT_SAFE_TYPE  uint8_t
#define CLOCK_SECONDS 1000
extern void protothread_clock_init(void);
extern clock_time_t clock_time(void);
extern uint64_t clock_time_us(void);
#endif
