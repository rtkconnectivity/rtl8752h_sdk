/**************************************************************************//**
 * @file      pt_cmd_usb.c
 * @brief     Source file for usb cdc acm test command implementation
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
#include "mac_test_common.h"
#include "services/usb_cdc_acm.h"

static uint8_t usb_rx_buf[512];
static uint16_t usb_rx_len;

static void usb_exit()
{
    dbg_printf("%s exit\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
}

PROCESS(usb_process, "usb_process");
PROCESS_THREAD(usb_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            usb_exit();
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_POLL)
        {
            dbg_printf("[USB RX] %u\r\n", usb_rx_len);
            dbg_mem_dump(usb_rx_buf, usb_rx_len);
            usb_cdc_acm_send("done\r\n", 6);
        }
    }
    PROCESS_END();
}

static void app_usb_rx_cb(const uint8_t *data, uint16_t len, void *ctx)
{
    (void)ctx;

    if (len > 512)
    {
        len = 512;
    }
    memcpy(usb_rx_buf, data, len);
    usb_rx_len = len;
    process_poll(&usb_process);
}

static int cmd_usb(int argc, char *argv[])
{
    usb_cdc_acm_config_t cfg =
    {
        .rx_cb = app_usb_rx_cb,

        .manufacturer = "Realtek",
        .product      = "USB CDC-ACM",
        .serial       = "0001",

        .rx_cb_ctx = NULL,
    };

    if (!usb_cdc_acm_init(&cfg))
    {
        dbg_printf("usb_cdc_acm_init fail\r\n");
        return FALSE;
    }

    usb_cdc_acm_start();

    process_start(&usb_process, NULL);
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_cmd_usb(void)
{
    shell_register_pt((shell_program_t)cmd_usb, "usb",
                      BRIEF("usb cdc acm loopback test")
                      SYNOPSIS("usb"));
}
