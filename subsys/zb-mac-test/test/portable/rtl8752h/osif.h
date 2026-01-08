/**************************************************************************//**
 * @file     osif.h
 * @brief    Implement the string to unsigned long long integer covnersion function.
 * @author   felix.yu
 * @version  V1.00
 * @date     2025-03-12
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

#ifndef _OSIF_H_
#define _OSIF_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <mem_types.h>
#include "os_sched.h"
//#include "os_pm.h"


#ifdef __cplusplus
extern "C" {
#endif

#define Support_AliOS                             0
#define Support_FreeRTOS                          1
/* task & isr context check interface */
//static inline bool osif_task_context_check(void);
void osif_systick_handler(void);
/* OS schedule interfaces */
void osif_delay(uint32_t ms);
uint64_t osif_sys_time_get(void);
uint64_t osif_sys_tick_get(void);
uint64_t osif_sys_tick_increase(uint32_t tick_increment);
bool osif_sched_start(void);
bool osif_sched_stop(void);
bool osif_sched_suspend(void);
bool osif_sched_resume(void);
bool osif_sched_is_start(void);
SCHEDULER_STATE osif_sched_state_get(void);
void osif_vector_table_update(void);
void osif_init(void);

/* OS task interfaces */
bool osif_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
                      void *p_param, uint16_t stack_size, uint16_t priority);
bool osif_task_delete(void *p_handle);
bool osif_task_suspend(void *p_handle);
bool osif_task_resume(void *p_handle);
bool osif_task_yield(void);
bool osif_task_handle_get(void **pp_handle);
bool osif_task_priority_get(void *p_handle, uint16_t *p_priority);
bool osif_task_priority_set(void *p_handle, uint16_t priority);
void osif_task_status_dump(void);
void osif_task_dlps_return_idle_task(void);
bool osif_task_notify_take(long xClearCountOnExit, uint32_t xTicksToWait,
                           uint32_t *p_notify);
bool osif_task_notify_give(void *p_handle);
bool osif_task_signal_create(void *p_handlel, uint32_t count);

/* OS synchronization interfaces */
uint32_t osif_lock(void);
void osif_unlock(uint32_t flags);
bool osif_sem_create(void **pp_handle, const char *p_name, uint32_t init_count, uint32_t max_count);
bool osif_sem_delete(void *p_handle);
bool osif_sem_take(void *p_handle, uint32_t wait_ms);
bool osif_sem_give(void *p_handle);
bool osif_mutex_create(void **pp_handle);
bool osif_mutex_delete(void *p_handle);
bool osif_mutex_take(void *p_handle, uint32_t wait_ms);
bool osif_mutex_give(void *p_handle);

/* OS message queue interfaces */
bool osif_msg_queue_create(void **pp_handle, uint32_t msg_num, uint32_t msg_size);
bool osif_msg_queue_delete(void *p_handle);
bool osif_msg_queue_peek(void *p_handle, uint32_t *p_msg_num);
bool osif_msg_send(void *p_handle, void *p_msg, uint32_t wait_ms);
bool osif_msg_recv(void *p_handle, void *p_msg, uint32_t wait_ms);
bool osif_msg_peek(void *p_handle, void *p_msg, uint32_t wait_ms);

/* OS memory management interfaces */
void *osif_mem_alloc(RAM_TYPE ram_type, size_t size);
void *osif_mem_aligned_alloc(RAM_TYPE ram_type, size_t size, uint8_t alignment);
void osif_mem_free(void *p_block);
void osif_mem_aligned_free(void *p_block);
size_t osif_mem_peek(RAM_TYPE ram_type);
void osif_mem_check_heap_usage(void);

/* OS software timer interfaces */
bool osif_timer_id_get(void **pp_handle, uint32_t *p_timer_id);
bool osif_timer_create(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
                       uint32_t interval_ms, bool reload, void (*p_timer_callback)());
bool osif_timer_start(void **pp_handle);
bool osif_timer_restart(void **pp_handle, uint32_t interval_ms);
bool osif_timer_stop(void **pp_handle);
bool osif_timer_delete(void **pp_handle);
bool osif_timer_dump(void);
bool osif_timer_state_get(void **pp_handle, uint32_t *p_timer_state);
void osif_timer_init(void);
bool osif_timer_number_get(void **pp_handle, uint32_t *p_timer_num);
bool osif_timer_get_auto_reload(void **pp_handle, long *p_autoreload);
bool osif_timer_is_timer_active(void **pp_handle);

typedef void (*PendedFunctionOS_t)(void *para1, uint32_t para2);

bool osif_timer_pend_function_call(PendedFunctionOS_t xFunctionToPend, void *para1, uint32_t para2);

bool osif_sched_restore(void);
/*os kernel systick handler interfaces */
uint32_t osif_sys_tick_rate_get(void);
uint32_t osif_sys_tick_clk_get(void);

/* Get software timer pool next expire value interface */
uint32_t osif_pm_next_timeout_value_get(void);

/*dlps restore os kernel scheduler processing interfaces */
void osif_pm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _OSIF_H_ */
