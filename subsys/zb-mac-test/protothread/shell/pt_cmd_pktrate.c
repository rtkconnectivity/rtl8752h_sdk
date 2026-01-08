/**************************************************************************//**
 * @file      pt_cmd_pktrate.c
 * @brief     Source file for pktrate command implementation
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
#include "mac_driver_interface.h"
#include "mac_test_common.h"
#include "mac_stats.h"

static struct etimer pktrate_etimer;

static struct pktrate_info_s
{
    uint32_t start_us;
    uint32_t rx_bytes;
} pktrate_info;

static void pktrate_settlement()
{
    struct pktrate_info_s curr_info;

    curr_info.start_us = clock_time_us();
    curr_info.rx_bytes = 0;
    for (uint8_t pan_idx = 0; pan_idx < TEST_MAX_PAN_NUM; pan_idx++)
    {
        curr_info.rx_bytes += mac_stats_rx_bytes_sum_get(pan_idx);
    }

    if (curr_info.rx_bytes >= pktrate_info.rx_bytes && curr_info.start_us >= pktrate_info.start_us)
    {
        uint64_t total_bits = ((uint64_t)(curr_info.rx_bytes - pktrate_info.rx_bytes)) << 3;
        uint64_t interval_us = curr_info.start_us - pktrate_info.start_us;
        uint64_t bps = total_bits * 1000000 / interval_us;
        dbg_printf("RX = %llu bps\r", bps);
    }

    memcpy(&pktrate_info, &curr_info, sizeof(struct pktrate_info_s));
}

PROCESS(pktrate_process, "pktrate");
PROCESS_THREAD(pktrate_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    etimer_set(&pktrate_etimer, (uint32_t)data);
    pktrate_info.start_us = clock_time_us();
    pktrate_info.rx_bytes = 0;
    for (uint8_t pan_idx = 0; pan_idx < TEST_MAX_PAN_NUM; pan_idx++)
    {
        pktrate_info.rx_bytes += mac_stats_rx_bytes_sum_get(pan_idx);
    }

    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            dbg_printf("%s exit\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
            etimer_stop(&pktrate_etimer);
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_TIMER && etimer_expired(&pktrate_etimer))
        {
            etimer_reset(&pktrate_etimer);
            pktrate_settlement();
        }
    }
    PROCESS_END();
}

static int cmd_pktrate(int argc, char *argv[])
{
    if (process_is_running(&pktrate_process))
    {
        dbg_printf("%s is running\r\n", PROCESS_NAME_STRING(&pktrate_process));
        return FALSE;
    }

    uint32_t interval;

    switch (argc)
    {
    case 1:
        interval = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    process_start(&pktrate_process, (void *)interval);
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_cmd_pktrate(void)
{
    pktrate_process.state = 0;
    shell_register_pt((shell_program_t)cmd_pktrate, "pktrate",
                      BRIEF("show rx packet rate")
                      SYNOPSIS("pktrate <interval>")
                      DESCRIPTION("interval: 0 ~ 4294967295 ms"));
}
