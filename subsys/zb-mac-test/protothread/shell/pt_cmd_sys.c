/**************************************************************************//**
 * @file      pt_cmd_sys.c
 * @brief     Source file for sys command implementation
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

#include "platform_port.h"
#include "protothread.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"
#include "services/auto_test.h"

/******************************************************************************/
static int cmd_ps(int argc, char *argv[])
{
    struct process *q;
    for (q = process_list; q != NULL; q = q->next)
    {
        dbg_printf("[%s]\r\n", q->name);
    }
    dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
static int cmd_kill(int argc, char *argv[])
{
    struct process *q;
    for (q = process_list; q != NULL; q = q->next)
    {
        if (q == PROCESS_CURRENT())
        {
            continue;
        }
        if (strcmp(q->name, argv[0]) == 0)
        {
            process_exit(q);
            dbg_printf("Done\r\n");
            return TRUE;
        }
    }
    dbg_printf("process %s not found\r\n", argv[0]);
    return TRUE;
}
/******************************************************************************/
int cmd_reset(int argc, char *argv[])
{
    WDG_SystemReset();
    at_send(0, NULL);
    return TRUE;
}
/******************************************************************************/
int cmd_time(int argc, char *argv[])
{
    uint64_t now_us = clock_time_us();
    uint64_t now_ms = now_us / 1000;
    clock_time_t now = (clock_time_t)now_ms;
    uint32_t btus = (uint32_t)(now_us % MAX_BT_CLOCK_COUNTER);
    dbg_printf("OS[%u]BTUS[%u]MAC[%llu]bt_clk_offset[%llu]\r\n", now, btus, now_us, bt_clk_offset);
    return TRUE;
}
/******************************************************************************/
int g_dbg_port = -1;
int g_rf_dbg_port = -1;
int cmd_dbgport(int argc, char *argv[])
{
#if (TEST_FPGA_DBG_PORT_EN == 1)
    uint32_t dbg_port = 0, rf_dbg_port = 0;
    switch (argc)
    {
    case 2:
        dbg_port = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        rf_dbg_port = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (dbg_port > 0xff || rf_dbg_port > 0xff)
        {
            goto err_out;
        }
        g_dbg_port = dbg_port;
        g_rf_dbg_port = rf_dbg_port;
    case 0:
        dbg_printf("dbgport %d %d\r\n", g_dbg_port, g_rf_dbg_port);
        break;
    default:
        goto err_out;
    }
    return TRUE;
err_out:
    dbg_printf("Error: InvalidArgs\r\n");
#endif
    return FALSE;
}
/******************************************************************************/
#if TEST_GPIO_DBG_EN
int cmd_gpio(int argc, char *argv[])
{
    uint32_t gpio_pin = 0;
    bool on_off = 0;

    switch (argc)
    {
    case 2:
        gpio_pin = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
    case 1:
        on_off = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (on_off)
        {
            debug_gpio_high(gpio_pin ? GPIO_OUTPUT_PIN_1 : GPIO_OUTPUT_PIN_0);
        }
        else
        {
            debug_gpio_low(gpio_pin ? GPIO_OUTPUT_PIN_1 : GPIO_OUTPUT_PIN_0);
        }
    case 0:
        dbg_printf("gpio_pin %u on_off %u\r\n", gpio_pin,
                   debug_gpio_get(gpio_pin ? GPIO_OUTPUT_PIN_1 : GPIO_OUTPUT_PIN_0));
        if (argc == 0)
        {
            dbg_printf("gpio_pin 1 on_off %u\r\n", debug_gpio_get(GPIO_OUTPUT_PIN_1));
        }
        break;
    default:
        goto err_out;
    }
    return TRUE;
err_out:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
#endif
/******************************************************************************/

void shell_register_cmd_sys(void)
{
    g_dbg_port = -1;
    g_rf_dbg_port = -1;

    shell_register_pt((shell_program_t)cmd_ps, "ps",
                      BRIEF("show process"));
    shell_register_pt((shell_program_t)cmd_kill, "kill",
                      BRIEF("kill process"));
    shell_register_pt((shell_program_t)cmd_reset, "reset",
                      BRIEF("Reset system"));
    shell_register_pt((shell_program_t)cmd_time, "time",
                      BRIEF("Show system time"));
    shell_register_pt((shell_program_t)cmd_dbgport, "dbgport",
                      BRIEF("get/set FPGA debug port")
                      SYNOPSIS("dbgport <dbg_port> <rf_dbg_port>"));
#if TEST_GPIO_DBG_EN
    shell_register_pt((shell_program_t)cmd_gpio, "gpio",
                      BRIEF("gpio test command")
                      SYNOPSIS("gpio [<on/off>] [<gpio_pin>]")
                      DESCRIPTION("      on/off: 0/1 (OFF/ON)")
                      DESCRIPTION("    gpio_pin: 0/1")
                      EXAMPLE("gpio 1 0"));
#endif
}
