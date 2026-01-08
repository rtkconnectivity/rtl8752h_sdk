/**************************************************************************//**
 * @file      protothread.h
 * @brief     Header file for protothread core
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

#ifndef _PROTOTHREAD_H_
#define _PROTOTHREAD_H_

#include "mac_test_config.h"
#include "sys/pt.h"
#include "sys/process.h"
#include "transplant_ports/pt_ports.h"
#include "sys/timer.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "lib/list.h"
#include "lib/ringbuf.h"
#include "lib/memb.h"

#if (TEST_PROTOTHREAD_EN == 1)
extern void *pt_task_handle;
void protothread_init(void);
#else
#define protothread_init()
#endif
#endif /* PROTOTHREAD_H_ */
