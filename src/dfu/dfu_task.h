/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _DFU_TASK_H_
#define _DFU_TASK_H_
#include <stdint.h>
#include <stdbool.h>
#include "app_msg.h"


void dfu_task_init(void);
bool app_send_msg_to_dfutask(T_IO_MSG *p_msg);

#endif

