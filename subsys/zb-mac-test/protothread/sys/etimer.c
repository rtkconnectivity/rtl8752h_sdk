/**************************************************************************//**
 * @file      etimer.c
 * @brief     Source file for protothread core
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

/*
 * Copyright (c) 2004, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/**
 * \addtogroup etimer
 * @{
 */

/**
 * \file
 * Event timer library implementation.
 * \author
 * Adam Dunkels <adam@sics.se>
 */
#include "protothread.h"
#include "platform_port.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"

static struct etimer *timerlist;
static clock_time_t next_expiration;

static uint32_t etimer_early_offset;

/*
static bool etimer_dbg_en = 0;
static uint32_t etimer_dbg_set;
static uint32_t etimer_dbg_timeout;
static uint32_t etimer_dbg_process;
*/

PROCESS(etimer_process, "etimer");
/*---------------------------------------------------------------------------*/
static mac_timer_handle_t *os_etimer = NULL;

static void os_etimer_cb(void *arg)
{
    //etimer_dbg_timeout = mac_btus_get();
    if (etimer_pending())
    {
        etimer_request_poll();
    }
}

static clock_time_t
remaining_time(struct timer *t, clock_time_t now)
{
    clock_time_t diff = (now - t->start);
    if (t->interval <= diff)
    {
        return 0;
    }
    else
    {
        return t->interval - diff;
    }
}

static void
update_time(void)
{
    clock_time_t min_tdist, tdist;
    struct etimer *t;
    uint64_t now_us = clock_time_us();
    uint64_t now_ms = now_us / 1000;
    clock_time_t now = (uint32_t)now_ms;

    if (timerlist == NULL)
    {
        next_expiration = 0;
        //dbg_printf("[%llu] mac_sw_timer_stop\r\n", now_us);
        mac_sw_timer_stop(os_etimer);
    }
    else
    {
        t = timerlist;
        /* Must calculate distance to next time into account due to wraps */
        min_tdist = remaining_time(&t->timer, now);
        for (t = t->next; min_tdist != 0 && t != NULL; t = t->next)
        {
            tdist = remaining_time(&t->timer, now);
            if (tdist < min_tdist)
            {
                min_tdist = tdist;
            }
        }
        next_expiration = now + min_tdist;
        if (min_tdist == 0)
        {
            etimer_request_poll();
        }
        else
        {
            uint32_t target_btus = mac_btus_get();
            target_btus += ((min_tdist * 1000) - etimer_early_offset);
            mac_sw_timer_start(os_etimer, target_btus, os_etimer_cb, NULL);
        }
    }
}
/*---------------------------------------------------------------------------*/
static struct etimer test_etimer;

static void etimer_test_exit()
{
    etimer_stop(&test_etimer);
    dbg_printf("%s exit\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
}

PROCESS(etimer_test_process, "etimer_test");
PROCESS_THREAD(etimer_test_process, ev, data)
{
    static uint32_t last_ms_tick = 0;
    static uint32_t last_us_tick = 0;

    PROCESS_BEGIN();
    dbg_printf("%s start. interval %u ms\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()), (uint32_t)data);
    etimer_set(&test_etimer, (uint32_t)data);
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            etimer_test_exit();
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_TIMER && etimer_expired(&test_etimer))
        {
            etimer_reset(&test_etimer);
            uint64_t now_us = clock_time_us();
            uint64_t now_ms = now_us / 1000;
            uint32_t tmp_ms_tick = (uint32_t)now_ms;
            uint32_t tmp_us_tick = (uint32_t)now_us;
            dbg_printf("MS[%u+%d] US[%u+%d] BTUS[%u] MAC[%llu]\r\n",
                       tmp_ms_tick, (int)(tmp_ms_tick - last_ms_tick - test_etimer.timer.interval),
                       tmp_us_tick, (int)(tmp_us_tick - last_us_tick - test_etimer.timer.interval * 1000),
                       (uint32_t)(now_us % MAX_BT_CLOCK_COUNTER), now_us);
            last_ms_tick = tmp_ms_tick;
            last_us_tick = tmp_us_tick;
        }
    }
    PROCESS_END();
}

static int cmd_etimer(int argc, char *argv[])
{
    if (argc > 0 && process_is_running(&etimer_test_process))
    {
        dbg_printf("%s is running\r\n", PROCESS_NAME_STRING(&etimer_test_process));
        return FALSE;
    }

    uint32_t value = 0;

    switch (argc)
    {
    case 2:
        etimer_early_offset = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        dbg_printf("Set etimer_early_offset = %u\r\n", etimer_early_offset);
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        process_start(&etimer_test_process, (void *)value);
        break;
    case 1:
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        //etimer_dbg_en = (value ? 1 : 0);
        break;
    case 0: // no param
        {
            struct etimer *t;
            dbg_printf("clock_time %u next_expiration %u etimer_early_offset %u\r\n", clock_time(),
                       next_expiration, etimer_early_offset);
            for (t = timerlist; t != NULL; t = t->next)
            {
                dbg_printf("tmr %p start %u interval %u process %s\r\n", t, t->timer.start,
                           t->timer.interval, PROCESS_NAME_STRING(t->p));
            }
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_cmd_etimer(void)
{
    etimer_test_process.state = 0;
    shell_register_pt((shell_program_t)cmd_etimer, "etimer",
                      BRIEF("etimer debug program")
                      SYNOPSIS("etimer <ms_timer_interval>"));
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(etimer_process, ev, data)
{
    struct etimer *t, *u;

    PROCESS_BEGIN();
    while (1)
    {
        PROCESS_YIELD();

        if (ev == PROCESS_EVENT_EXITED)
        {
            struct process *p = data;

            while (timerlist != NULL && timerlist->p == p)
            {
                timerlist = timerlist->next;
            }

            if (timerlist != NULL)
            {
                t = timerlist;
                while (t->next != NULL)
                {
                    if (t->next->p == p)
                    {
                        t->next = t->next->next;
                    }
                    else
                    {
                        t = t->next;
                    }
                }
            }
            continue;
        }
        else if (ev != PROCESS_EVENT_POLL)
        {
            continue;
        }
        //dbg_printf("etimer ev %x\r\n", ev);
        /*
        etimer_dbg_process = mac_btus_get();
        if (etimer_dbg_en) {
            dbg_printf("    set %u\r\n", etimer_dbg_set);
            dbg_printf("timeout %u\r\n", etimer_dbg_timeout);
            dbg_printf("process %u\r\n", etimer_dbg_process);
            dbg_printf("   diff %u\r\n", etimer_dbg_process - etimer_dbg_set);
        }*/
again:

        u = NULL;

        for (t = timerlist; t != NULL; t = t->next)
        {
            if (timer_expired(&t->timer))
            {
                //dbg_printf("[%u] %p etimer_expired. start %u interval %u post event to %s\r\n",
                //    clock_time(), t, t->timer.start, t->timer.interval, PROCESS_NAME_STRING(t->p));
                if (process_post(t->p, PROCESS_EVENT_TIMER, t) == PROCESS_ERR_OK)
                {
                    /* Reset the process ID of the event timer, to signal that the
                       etimer has expired. This is later checked in the
                       etimer_expired() function. */
                    t->p = PROCESS_NONE;
                    if (u != NULL)
                    {
                        u->next = t->next;
                    }
                    else
                    {
                        timerlist = t->next;
                    }
                    t->next = NULL;
                    update_time();
                    goto again;
                }
                else
                {
                    etimer_request_poll();
                }
            }
            u = t;
        }

        if (etimer_pending())
        {
            mac_cs_enter();
            uint32_t tmr_is_start = os_etimer->timer_ctrl.is_start;
            mac_cs_exit();
            if (tmr_is_start == 0)
            {
                //dbg_printf("etimer recover\r\n");
                update_time();
                goto again;
            }
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
etimer_request_poll(void)
{
    //dbg_printf("[%u] etimer_request_poll\r\n", clock_time());
    process_poll(&etimer_process);
}
/*---------------------------------------------------------------------------*/
static void
add_timer(struct etimer *timer)
{
    struct etimer *t;

    if (timer->p != PROCESS_NONE)
    {
        for (t = timerlist; t != NULL; t = t->next)
        {
            if (t == timer)
            {
                /* Timer already on list, bail out. */
                timer->p = PROCESS_CURRENT();
                update_time();
                return;
            }
        }
    }

    /* Timer not on list. */
    timer->p = PROCESS_CURRENT();
    timer->next = timerlist;
    timerlist = timer;
    update_time();
}
/*---------------------------------------------------------------------------*/
void
etimer_set(struct etimer *et, clock_time_t interval)
{
    //dbg_printf("etimer_set %p %u ms\r\n", et, interval);
    timer_set(&et->timer, interval);
    add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
etimer_reset_with_new_interval(struct etimer *et, clock_time_t interval)
{
    timer_reset(&et->timer);
    et->timer.interval = interval;
    add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
etimer_reset(struct etimer *et)
{
    timer_reset(&et->timer);
    add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
etimer_restart(struct etimer *et)
{
    timer_restart(&et->timer);
    add_timer(et);
}
/*---------------------------------------------------------------------------*/
void
etimer_adjust(struct etimer *et, int timediff)
{
    et->timer.start += timediff;
    update_time();
}
/*---------------------------------------------------------------------------*/
int
etimer_expired(struct etimer *et)
{
    return et->p == PROCESS_NONE;
}
/*---------------------------------------------------------------------------*/
clock_time_t
etimer_expiration_time(struct etimer *et)
{
    return et->timer.start + et->timer.interval;
}
/*---------------------------------------------------------------------------*/
clock_time_t
etimer_start_time(struct etimer *et)
{
    return et->timer.start;
}
/*---------------------------------------------------------------------------*/
int
etimer_pending(void)
{
    return timerlist != NULL;
}
/*---------------------------------------------------------------------------*/
clock_time_t
etimer_next_expiration_time(void)
{
    return etimer_pending() ? next_expiration : 0;
}
/*---------------------------------------------------------------------------*/
void
etimer_stop(struct etimer *et)
{
    struct etimer *t;

    /* First check if et is the first event timer on the list. */
    if (et == timerlist)
    {
        timerlist = timerlist->next;
        update_time();
    }
    else
    {
        /* Else walk through the list and try to find the item before the
           et timer. */
        for (t = timerlist; t != NULL && t->next != et; t = t->next)
        {
        }

        if (t != NULL)
        {
            /* We've found the item before the event timer that we are about
               to remove. We point the items next pointer to the event after
               the removed item. */
            t->next = et->next;

            update_time();
        }
    }

    /* Remove the next pointer from the item to be removed. */
    et->next = NULL;
    /* Set the timer as expired */
    et->p = PROCESS_NONE;
}
/*---------------------------------------------------------------------------*/
void
etimer_init(void)
{
    timerlist = NULL;
    etimer_early_offset = TASK_SCHEDULE_OVERHEAD;
    os_etimer = mac_sw_timer_alloc();
    if (os_etimer == NULL)
    {
        dbg_printf("warn: etimer init fail. ensure the MAC is initialized.\r\n");
        return;
    }
    shell_register_cmd_etimer();
    process_exit(&etimer_process);
    process_start(&etimer_process, NULL);
}
/** @} */
