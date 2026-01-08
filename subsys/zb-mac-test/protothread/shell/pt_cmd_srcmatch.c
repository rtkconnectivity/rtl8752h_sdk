/**************************************************************************//**
 * @file      pt_cmd_srcmatch.c
 * @brief     Source file for srcmatch command implementation
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

int cmd_srcmatchmode(int argc, char *argv[])
{
    uint8_t mode = 0;
#if TEST_MPAN_EN
    uint8_t pan_idx = 0;
#endif /*TEST_MPAN_EN*/
    switch (argc)
    {
#if TEST_MPAN_EN
    case 2: // set with pan index
        pan_idx = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (pan_idx >= TEST_MAX_PAN_NUM)
        {
            goto error;
        }
#endif /*TEST_MPAN_EN*/
    case 1:
        mode = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
#if TEST_MPAN_EN
        mpan_SetAddrMatchMode(((mode & 0x1) ? 1 : 0), pan_idx);
        mpan_SetEnhFrmPending(((mode & 0x2) ? 1 : 0), pan_idx);
        mpan_SetDataReqFrmPending(((mode & 0x4) ? 1 : 0), pan_idx);
#else
        mac_addr_match_mode_set(mode);
#endif /*TEST_MPAN_EN*/
    case 0:
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
#if TEST_MPAN_EN
        for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
        {
            dbg_printf("srcmatchmode[%d] %u\r\n", i, mpan_GetAddrMatchMode(i));
        }
#else
        dbg_printf("srcmatchmode %u\r\n", mac_addr_match_mode_get());
#endif /*TEST_MPAN_EN*/
        break;
    default:
        goto error;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
error:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
/******************************************************************************/
int cmd_srcmatchfilter(int argc, char *argv[])
{
    uint16_t panid, saddr;
    uint64_t laddr;
#if TEST_MPAN_EN
    uint8_t pan_idx = 0;
    extern uint8_t mpan_AddSrcShortAddrMatchPanIdx(uint8_t index);
    extern uint8_t mpan_AddSrcExtAddrMatchPanIdx(uint8_t index);
#endif /*TEST_MPAN_EN*/

    extern uint8_t mac_addr_match_short_enable_get(uint8_t index);
    extern uint8_t mac_addr_match_long_enable_get(uint8_t index);
    if (argc == 0)
    {
        // show table
        for (uint8_t i = 0; i < EXT_SRC_ADDR_MATCH_ENTRY_NUM; i++)
        {
            dbg_printf("SRC_ADDR_MATCH_ENTRY[%02u] ", i);
            dbg_printf("%08x ", mac_addr_match_entry_get(i, FALSE));
            dbg_printf("%08x ", mac_addr_match_entry_get(i, TRUE));
#if TEST_MPAN_EN
            dbg_printf("s_en/pidx:%u/%u s_en/pidx:%u/%u e_en/pidx:%u/%u",
                       mac_addr_match_short_enable_get(i * 2), mpan_AddSrcShortAddrMatchPanIdx(i * 2),
                       mac_addr_match_short_enable_get(i * 2 + 1), mpan_AddSrcShortAddrMatchPanIdx(i * 2 + 1),
                       mac_addr_match_long_enable_get(i), mpan_AddSrcExtAddrMatchPanIdx(i));
#else
            dbg_printf("s_en:%u s_en:%u e_en:%u", mac_addr_match_short_enable_get(i * 2),
                       mac_addr_match_short_enable_get(i * 2 + 1),
                       mac_addr_match_long_enable_get(i));
#endif
            dbg_printf("\r\n");
        }
        return TRUE;
    }
    else
    {
        char c_op = argv[0][0];
        bool op_add = FALSE;
        if (c_op == 'a')
        {
            op_add = TRUE;
        }
        else if (c_op == 'd')
        {
            op_add = FALSE;
        }
        else
        {
            goto error;
        }
        char c_addr_mode = argv[0][1];
        bool addr_mode_short = FALSE;
        if (c_addr_mode == 's')
        {
            addr_mode_short = TRUE;
        }
        else if (c_addr_mode == 'e')
        {
            addr_mode_short = FALSE;
        }
        else if (op_add == 0 && c_addr_mode == 'a')     // delete all
        {
#if TEST_MPAN_EN
            if (argc > 1)
            {
                pan_idx = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
            }
            mpan_DelAllSrcAddrMatch(pan_idx);
#else
            mac_addr_match_short_flush();
            mac_addr_match_long_flush();
#endif
            at_send(0, NULL);
            goto done;
        }
        else
        {
            goto error;
        }

        if (addr_mode_short == TRUE)
        {
            // srcmatchfilter <as/ds> <shortaddr> <panid> [<pan_index>]
            if (argc < 3)
            {
                goto error;
            }
#if TEST_MPAN_EN
            if (argc > 3)
            {
                pan_idx = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
            }
#endif
            saddr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
            panid = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
            if (op_add == TRUE)
            {
#if TEST_MPAN_EN
                mpan_AddSrcShortAddrMatch(saddr, panid, pan_idx);
#else
                mac_addr_match_short_add(saddr, panid);
#endif
                at_send(0, NULL);
            }
            else
            {
#if TEST_MPAN_EN
                mpan_DelSrcShortAddrMatch(saddr, panid, pan_idx);
#else
                mac_addr_match_short_del(saddr, panid);
#endif
            }
        }
        else
        {
            // srcmatchfilter <ae/de> <extaddr> [<pan_index>]
            if (argc < 2)
            {
                goto error;
            }
#if TEST_MPAN_EN
            if (argc > 2)
            {
                pan_idx = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
            }
#endif
            laddr = _strtoull((const char *)(argv[1]), (char **)NULL, 16);
            if (op_add == TRUE)
            {
#if TEST_MPAN_EN
                mpan_AddSrcExtAddrMatch((uint8_t *)&laddr, pan_idx);
#else
                mac_addr_match_long_add((uint8_t *)&laddr);
#endif
                at_send(0, NULL);
            }
            else
            {
#if TEST_MPAN_EN
                mpan_DelSrcExtAddrMatch((uint8_t *)&laddr, pan_idx);
#else
                mac_addr_match_long_del((uint8_t *)&laddr);
#endif
            }
        }
    }

done:
    dbg_printf("Done\r\n");
    return TRUE;
error:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
/******************************************************************************/
void shell_register_cmd_srcmatch(void)
{
    shell_register_pt((shell_program_t)cmd_srcmatchmode, "srcmatchmode",
                      BRIEF("enable the enhanced frame pending mechanism of auto frame pending bit in Imm-Ack")
                      SYNOPSIS("srcmatchmode [<mode>] [<pan_index>]")
                      DESCRIPTION("mode: 0 ~ 7")
                      DESCRIPTION("      Bit 0: enhanced frame pending according to source address match filter")
                      DESCRIPTION("             1 - response to Data frames or Command frames")
                      DESCRIPTION("             0 - only response to a Data Request command frame")
                      DESCRIPTION("      Bit 1: default pending bit in Imm-Ack for non data request frame")
                      DESCRIPTION("      Bit 2: default pending bit in Imm-Ack for data request frame")
                      DESCRIPTION("pan_index: 0 ~ 3"));
    shell_register_pt((shell_program_t)cmd_srcmatchfilter, "srcmatchfilter",
                      BRIEF("get/set source address match filter")
                      SYNOPSIS("srcmatchfilter <as/ds> <shortaddr> <panid> [<pan_index>]")
                      SYNOPSIS("srcmatchfilter <ae/de> <extaddr> [<pan_index>]")
                      SYNOPSIS("srcmatchfilter <da> [<pan_index>]")
                      DESCRIPTION("    as/ds: add/delete short address")
                      DESCRIPTION("    ae/de: add/delete extended address")
                      DESCRIPTION("       da: delete all")
                      DESCRIPTION("  extaddr: 0x0000000000000000 ~ 0xffffffffffffffff")
                      DESCRIPTION("shortaddr: 0x0000 ~ 0xffff")
                      DESCRIPTION("    panid: 0x0000 ~ 0xffff")
                      DESCRIPTION("pan_index: 0 ~ 3")
                      SYNOPSIS("srcmatchfilter")
                      DESCRIPTION("     show filter table")
                      EXAMPLE("srcmatchfilter ae 0xacde480000000001")
                      EXAMPLE("srcmatchfilter as 0x1122 0x1aaa"));
}
