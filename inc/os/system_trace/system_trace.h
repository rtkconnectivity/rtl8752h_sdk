/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _SYSTEM_TRACE_H_
#define _SYSTEM_TRACE_H_

#include "app_section.h"
#include "trace_heap.h"
#include "trace_stack.h"
#include "trace_timer.h"
#include "trace_queue.h"
#include "trace_dlps.h"
#include "trace_task_hang.h"
#include "trace_hardfault.h"
#include "trace_wdg_timeout.h"
#include "trace_system_loading.h"

#ifdef __cplusplus
extern "C" {
#endif

extern HEAP_INFO heap_info[TRACE_HEAP_TYPE_NUM];
extern STACK_INFO stack_info;
extern TIMER_INFO timer_info;
extern QUEUE_INFO queue_info[MAX_QUEUE_OBJECTS_NUM];

extern void system_trace_init(void);

#ifdef __cplusplus
}
#endif

#endif /*_SYSTEM_TRACE_H_*/
