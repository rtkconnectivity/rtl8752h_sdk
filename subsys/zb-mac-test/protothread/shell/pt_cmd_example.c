/**************************************************************************//**
 * @file      pt_cmd_example.c
 * @brief     Source file for example command implementation
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

#include "protothread.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"

#define EXAMPLE_PROCESS 0

#if EXAMPLE_PROCESS
static struct etimer example_etimer;

static void example_exit()
{
    etimer_stop(&example_etimer);
    dbg_printf("%s exit\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
}

PROCESS(example_process, "example");
PROCESS_THREAD(example_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    etimer_set(&example_etimer, 1000);
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            example_exit();
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_TIMER && etimer_expired(&example_etimer))
        {
            dbg_printf("now %u\r\n", clock_time());
            etimer_reset(&example_etimer);
        }
    }
    PROCESS_END();
}
#endif

static int cmd_example(int argc, char *argv[])
{
    uint32_t value = 0;

    switch (argc)
    {
    case 1:
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        dbg_printf("example value %u\r\n", value);
        break;
    case 0:
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

#if EXAMPLE_PROCESS
    if (process_is_running(&example_process))
    {
        dbg_printf("%s is running\r\n", PROCESS_NAME_STRING(&example_process));
        return FALSE;
    }
    process_start(&example_process, NULL);
#endif
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_cmd_example(void)
{
    shell_register_pt((shell_program_t)cmd_example, "example",
                      BRIEF("example program")
                      SYNOPSIS("example"));
}
