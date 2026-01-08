/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef _TRACE_SYSTEM_LOADING_H_
#define _TRACE_SYSTEM_LOADING_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


void trace_system_loading_init(void);

void get_system_loading(void);

void reset_system_loading(void);


#ifdef __cplusplus
}
#endif

#endif /* _TRACE_SYSTEM_LOADING_H_ */
