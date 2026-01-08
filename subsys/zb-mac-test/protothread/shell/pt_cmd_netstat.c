/**************************************************************************//**
 * @file      pt_cmd_netstat.c
 * @brief     Source file for netstat command implementation
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
#include "services/auto_test.h"
#include "mac_stats.h"

int cmd_ifconfig(int argc, char *argv[])
{
    char c = 'a';

#if (TEST_MPAN_EN == 1)
    int8_t pan_index = -1;

    if (argc > 1)
    {
        pan_index = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (pan_index >= TEST_MAX_PAN_NUM)
        {
            goto error;
        }
    }
#endif /*TEST_MPAN_EN*/

    if (argc > 0)
    {
        c = argv[0][0];
    }
    switch (c)
    {
    case 'c':
        mac_stats_init();
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("Done\r\n");
        }
        else
        {
            at_send(0, NULL);
        }
        break;
    case 'a':
    case 'i':
        mac_stats_irq_print();
        if (c == 'i')
        {
            break;
        }
    case 'p':
        if (auto_test_is_enable() == 0)
        {
            mac_stats_pktsig_print();
        }
        if (c == 'p')
        {
            break;
        }
    case 'r':
        for (uint8_t pan_idx = 0; pan_idx < TEST_MAX_PAN_NUM; pan_idx++)
        {
#if (TEST_MPAN_EN == 1)
            if (pan_index != -1 && pan_idx != pan_index) { continue; }
#endif
            if (auto_test_is_enable() == 0)
            {
                mac_stats_rx_print(pan_idx);
            }
            else
            {
                uint32_t rx_total = mac_stats_rx_total_sum_get(pan_idx);
                at_send(sizeof(rx_total), &rx_total);
            }
        }
        if (c == 'r')
        {
            break;
        }
    case 't':
        for (uint8_t pan_idx = 0; pan_idx < TEST_MAX_PAN_NUM; pan_idx++)
        {
#if (TEST_MPAN_EN == 1)
            if (pan_index != -1 && pan_idx != pan_index) { continue; }
#endif
            if (auto_test_is_enable() == 0)
            {
                mac_stats_tx_print(pan_idx);
            }
            else
            {
                uint32_t tx_succ_total = mac_stats_tx_succ_total_get(pan_idx, 0);
                uint32_t tx_fail_total = mac_stats_tx_fail_total_get(pan_idx, 0);
                at_send_first(sizeof(tx_succ_total), &tx_succ_total);
                at_send_last(sizeof(tx_fail_total), &tx_fail_total);
            }
        }
        break;
    default:
        goto error;
    }
    return TRUE;
error:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
/******************************************************************************/
int cmd_retrycnt(int argc, char *argv[])
{
    uint8_t retry_num = mac_txn_retry_cnt_get();
    if (auto_test_is_enable() == 0)
    {
        dbg_printf("tx retry count %u (read only)\r\n", retry_num);
    }
    else
    {
        at_send(sizeof(retry_num), &retry_num);
    }
    return TRUE;
}
/******************************************************************************/
int cmd_pendack(int argc, char *argv[])
{
    uint8_t pendack = mac_imm_ack_fp_check();
    if (auto_test_is_enable() == 0)
    {
        dbg_printf("ACK pending %u (read only)\r\n", pendack);
    }
    else
    {
        at_send(sizeof(pendack), &pendack);
    }
    return TRUE;
}
/******************************************************************************/
void shell_register_cmd_netstat(void)
{
    shell_register_pt((shell_program_t)cmd_ifconfig, "ifconfig",
                      BRIEF("show/clear interface statistic")
                      SYNOPSIS("ifconfig [<value>] [<pan_index>]")
                      DESCRIPTION("value: c/r/t/i/p/a")
                      DESCRIPTION("       c - clear statistic")
                      DESCRIPTION("       r - show RX statistic")
                      DESCRIPTION("       t - show TX statistic")
                      DESCRIPTION("       i - show IRQ statistic")
                      DESCRIPTION("       p - show packet signal quality statistic")
                      DESCRIPTION("       a - show All statistic")
                      DESCRIPTION("pan_index: 0 ~ 3")
                      EXAMPLE("ifconfig - show ALL statistic"));
    shell_register_pt((shell_program_t)cmd_retrycnt, "retrycnt",
                      BRIEF("Get tx retry counter"));
    shell_register_pt((shell_program_t)cmd_pendack, "pendack",
                      BRIEF("Get RX Imm-Ack pending bit"));
}
