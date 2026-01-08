/**************************************************************************//**
 * @file      protothread.c
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

//#include <trace.h>
#include <osif.h>
#include "platform_port.h"
#include "mac_test_config.h"
#include "protothread.h"
#include "net/pt_mac.h"
#include "shell/pt_cmd.h"
#include "services/auto_test.h"

void *pt_task_handle;
PROCESS_NAME(shell_process);

static void protothread_shell_cmd_init()
{
    shell_register_cmd_sys();
    shell_register_cmd_mac();
    shell_register_cmd_ble();
    shell_register_cmd_netconf();
    shell_register_cmd_netstat();
    shell_register_cmd_txconf();
    shell_register_cmd_rxconf();
    shell_register_cmd_srcmatch();
    shell_register_cmd_pktgen();
    shell_register_cmd_pktrate();
    shell_register_cmd_example();
    shell_register_cmd_ping();
    shell_register_cmd_sec();
    shell_register_cmd_sniffer();
}

static void protothread_task(void *p_param)
{
    uint32_t notify = 0;
    process_init();
    etimer_init();
    ctimer_init();
    process_start(&shell_process, NULL);
    /* network stack init */
    NETSTACK_MAC.init();
    process_start(&ping_reply_process, NULL);
    auto_test_init();
    protothread_shell_cmd_init();
    while (1)
    {
        process_num_events_t r;
        do
        {
            r = process_run();
            //watchdog_periodic();
            //DBG_DIRECT("process_run %u\r\n", r);
        }
        while (r > 0);
        //DBG_DIRECT("pt idle %u", notify);
        osif_task_notify_take(1, 0xffffffff, &notify);
        //DBG_DIRECT("pt run %u", notify);
    }
}

void protothread_init(void)
{
    osif_task_create(&pt_task_handle, "pt_task", protothread_task, NULL, PT_TASK_STACK_SIZE,
                     PT_TASK_PRIORITY);
}
