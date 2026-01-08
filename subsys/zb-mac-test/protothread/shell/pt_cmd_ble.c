/**************************************************************************//**
 * @file      pt_cmd_ble.c
 * @brief     Source file for ble command implementation
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

#if TEST_BT_ADV_EN
#include "gap_msg.h"
#include "gap_adv.h"
extern void ble_peripheral_init(void);
int cmd_ble_adv(uint32_t argc, uint8_t  *argv[])
{
    static bool g_ble_peripheral_init = FALSE;
    T_GAP_DEV_STATE state;
    bool enable;
    uint16_t adv_int = 0;
    switch (argc)
    {
    case 2:
        adv_int = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (adv_int < 20 || adv_int > 10240)
        {
            goto err_out;
        }
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (enable)
        {
            if (g_ble_peripheral_init == FALSE)
            {
                ble_peripheral_init();
                g_ble_peripheral_init = TRUE;
                if (adv_int)
                {
                    dbg_printf("ble_adv interval %u ms\r\n", adv_int);
                    adv_int = (uint32_t)adv_int * 1000 / 625;
                    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int), &adv_int);
                    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int), &adv_int);
                }
                else
                {
                    dbg_printf("ble_adv interval 200 ms\r\n");
                }
            }
            else
            {
                if (adv_int)
                {
                    dbg_printf("ble_adv interval %u ms\r\n", adv_int);
                    adv_int = (uint32_t)adv_int * 1000 / 625;
                    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MIN, sizeof(adv_int), &adv_int);
                    le_adv_set_param(GAP_PARAM_ADV_INTERVAL_MAX, sizeof(adv_int), &adv_int);
                }
                le_adv_start();
            }

        }
        else
        {
            le_adv_stop();
        }
    case 0:
        le_get_gap_param(GAP_PARAM_DEV_STATE, &state);
        dbg_printf("ble_adv %u\r\n", (state.gap_adv_state == GAP_ADV_STATE_START ||
                                      state.gap_adv_state == GAP_ADV_STATE_ADVERTISING) ? 1 : 0);
        /*
        #define GAP_ADV_STATE_IDLE           0   //!< Idle, no advertising
        #define GAP_ADV_STATE_START          1   //!< Start Advertising. A temporary state, haven't received the result.
        #define GAP_ADV_STATE_ADVERTISING    2   //!< Advertising
        #define GAP_ADV_STATE_STOP           3   //!< Stop Advertising. A temporary state, haven't received the result.
        */
        break;
    default:
        goto err_out;
    }
    return TRUE;
err_out:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
#else
int cmd_ble_adv(uint32_t argc, uint8_t  *argv[])
{
    dbg_printf("Error: BLE not enabled\r\n");
    return FALSE;
}
#endif /* TEST_BT_ADV_EN */

void shell_register_cmd_ble(void)
{
#if TEST_BT_ADV_EN
    /* BLE */
    shell_register_pt((shell_program_t)cmd_ble_adv, "ble_adv",
                      BRIEF("get/set BLE Advertising")
                      SYNOPSIS("ble_adv [<enable/disable>] [<interval>]")
                      DESCRIPTION("enable/disable: 1/0")
                      DESCRIPTION("interval: 20ms - 10240ms, 0.625ms/step"));
#endif /* TEST_BT_ADV_EN */
}
