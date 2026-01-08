/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _TRACE_QUEUE_H_
#define _TRACE_QUEUE_H_

#include "trace_common.h"
#include "trace_port_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _QUEUE_INFO
{
    uint16_t capacity_num;
    uint16_t curr_num_in_queue;
    uint16_t maximum_ever_num_in_queue;
    uint16_t reserved;
} QUEUE_INFO;

extern bool trace_queue_init(uint32_t queue_num, ...);
extern void trace_queue(void);

#ifdef __cplusplus
}
#endif

#endif /*_TRACE_QUEUE_H_*/

/******************* (C) COPYRIGHT 2015 Realtek Semiconductor Corporation *****END OF FILE****/
