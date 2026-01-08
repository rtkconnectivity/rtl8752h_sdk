/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _TRACE_HEAP_H_
#define _TRACE_HEAP_H_

#include "trace_common.h"
#include "trace_port_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _BLOCK_SIZE_LIST
{
    uint32_t number;
    uint32_t size[TRACE_HEAP_MAX_NUMBER_IN_BLOCK_LIST];
} BLOCK_SIZE_LIST;

typedef struct _HEAP_INFO
{
    uint32_t total_size;
    uint32_t curr_remain_size;
    uint32_t minimum_ever_free_size;
    //BLOCK_SIZE_LIST used_size_list;
    BLOCK_SIZE_LIST free_size_list;
} HEAP_INFO;

extern void trace_heap(void);

#ifdef __cplusplus
}
#endif

#endif /*_TRACE_HEAP_H_*/
