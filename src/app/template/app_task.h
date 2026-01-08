/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include <stdint.h>
#include <stdbool.h>
#include "board.h"


#define  USE_OSIF                  1

#if (USE_OSIF == 1)

extern void *app_task_handle;
extern void *evt_queue_handle;
extern void *io_queue_handle;
#else
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
extern QueueHandle_t evt_queue_handle;
extern QueueHandle_t io_queue_handle;
extern TaskHandle_t app_task_handle;
#endif

/**  @brief Event type definitions.*/
typedef enum
{
    EVENT_GAP_MSG = 0x01,  /**< message from gap layer for stack */
    EVENT_IO_TO_APP = 0x02, /**< message from IO to user application */
} T_EVENT_TYPE;

bool app_send_msg_to_apptask(uint32_t *p_send);

void task_init(void);

void sw_timer_init(void);


