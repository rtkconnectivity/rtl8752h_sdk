/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _TRACE_CONFIG_H_
#define _TRACE_CONFIG_H_

#define TRACE_HEAP_EN                           1
#define TRACE_STACK_EN                          1
#define TRACE_TIMER_EN                          1
#define TRACE_QUEUE_EN                          1
#define TRACE_SYSTEM_LOADING                    1
#define DEBUG_TASK_HANG_EN                      1
#define DEBUG_DLPS_ERROR_EN                     0
#define TRACE_HARDFAULT                         0
#define TRACE_WDG_TIMEOUT                       0
#define TRACE_TASK_HANG_USE_WDG_ISR             0
/******************* CONFIGURATION *******************/
#define TRACE_PERIOD_TIME                       ( 10 * 1000 )

/******************* QUEUE CONFIGURATION *******************/
#define QUEUE_OBJECTS_NUM                       ( 3 )
//#define TRACE_QUEUE_PERIOD_TIME                 ( 10 * 1000 )

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /*_TRACE_CONFIG_H_*/
