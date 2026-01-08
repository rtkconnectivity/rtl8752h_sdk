/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _APP_TASK_H_
#define _APP_TASK_H_

#include "app_msg.h"
#include "stdbool.h"

extern void *app_task_handle;
extern void *evt_queue_handle;
extern void *io_queue_handle;

void app_task_init(void);
bool app_send_msg_to_apptask(T_IO_MSG *p_msg);

#endif

