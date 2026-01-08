/**************************************************************************//**
 * @file      pt_ports.c
 * @brief     Source file for protothread porting
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
#include "mac_driver_interface.h"
#include "dbg_printf.h"

void protothread_clock_init(void)
{

}

uint64_t clock_time_us(void)
{
    static uint64_t last_us = 0;
    uint8_t retry = 0;
    uint64_t now_us = 0;

    while (retry++ < 3)
    {
        now_us = mac_timestamp_get();
        if (now_us < last_us)
        {
            dbg_printf("clock_time_us warning. now %llu last %llus\r\n", now_us, last_us);
            if (retry == 3)
            {
                dbg_printf("force update last_us\r\n");
            }
        }
        else
        {
            break;
        }
    }

    last_us = now_us;
    return now_us;
}

clock_time_t clock_time(void)
{
    uint64_t now_us = clock_time_us();
    //uint64_t now_ms = now_us / 1000;
    return (clock_time_t)(now_us / 1000);
}
