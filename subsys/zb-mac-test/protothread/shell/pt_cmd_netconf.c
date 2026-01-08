/**************************************************************************//**
 * @file      pt_cmd_netconf.c
 * @brief     Source file for netconf command implementation
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
#include "mac_driver.h"
#include "services/auto_test.h"

/******************************************************************************/
int cmd_panid(int argc, char *argv[])
{
    uint16_t panid = 0;
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
    case 1: // set
        panid = _strtoul((const char *)(argv[0]), (char **)NULL, 16);
#if TEST_MPAN_EN
        mpan_SetPANId(panid, pan_idx);
        mpan_EnableCtl(pan_idx, 1);
#else
        mac_panid_set(panid);
#endif /*TEST_MPAN_EN*/
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
    case 0: // show
#if TEST_MPAN_EN
        for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
        {
            dbg_printf("panid[%d] 0x%04x\r\n", i, mpan_GetPANId(i));
        }
#else
        dbg_printf("panid 0x%04x\r\n", mac_panid_get());
#endif /*TEST_MPAN_EN*/
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
int cmd_channel(int argc, char *argv[])
{
    uint8_t ch = 0;
#if (TEST_DUAL_CHNL_EN == 1)
    uint8_t pan_idx = 0;
#endif
    switch (argc)
    {
#if (TEST_DUAL_CHNL_EN == 1)
    case 2: // set with pan index
        pan_idx = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (pan_idx >= TEST_MAX_PAN_NUM)
        {
            goto error;
        }
#endif
    case 1:
        ch = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
#if (TEST_DUAL_CHNL_EN == 1)
        mpan_SetChannel(ch, pan_idx);
#else
        mac_channel_set(ch);
#endif
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
    case 0:
#if (TEST_DUAL_CHNL_EN == 1)
        for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
        {
            dbg_printf("channel[%d] %u\r\n", i, mpan_GetChannel(i));
        }
        dbg_printf("channel %u\r\n", mac_channel_get());
#else
        dbg_printf("channel %u\r\n", mac_channel_get());
#endif
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
#if (TEST_MPAN_EN == 0)
int cmd_freq(int argc, char *argv[])
{
    uint16_t freq;
    switch (argc)
    {
    case 1:
        freq = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_freq_set(freq);
    case 0:
        dbg_printf("freq = %u MHz\r\n", mac_freq_get());
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
#else
int cmd_freq(int argc, char *argv[])
{
    return TRUE;
}
#endif /*TEST_MPAN_EN*/
/******************************************************************************/
int cmd_shortaddr(int argc, char *argv[])
{
    uint16_t saddr = 0;
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
        saddr = _strtoul((const char *)(argv[0]), (char **)NULL, 16);
#if TEST_MPAN_EN
        mpan_SetShortAddress(saddr, pan_idx);
#else
        mac_short_addr_set(saddr);
#endif /*TEST_MPAN_EN*/
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
    case 0:
#if TEST_MPAN_EN
        for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
        {
            dbg_printf("shortaddr[%d] 0x%04x\r\n", i, mpan_GetShortAddress(i));
        }
#else
        dbg_printf("shortaddr 0x%04x\r\n", mac_short_addr_get());
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
int cmd_extaddr(int argc, char *argv[])
{
    uint64_t laddr = 0;
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
        laddr = _strtoull((const char *)(argv[0]), (char **)NULL, 16);

#if TEST_MPAN_EN
        mpan_SetLongAddress((uint8_t *)&laddr, pan_idx);
#else
        mac_long_addr_set((uint8_t *)&laddr);
#endif /*TEST_MPAN_EN*/
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
    case 0:
#if TEST_MPAN_EN
        for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
        {
            memcpy(&laddr, mpan_GetLongAddress(i), sizeof(laddr));
            dbg_printf("extaddr[%d] 0x%016llx\r\n", i, laddr);
        }
#else
        memcpy(&laddr, mac_long_addr_get(), sizeof(laddr));
        dbg_printf("extaddr 0x%016llx\r\n", laddr);
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
int cmd_mackey(int argc, char *argv[])
{
    int i;
    char *tmp;

    switch (argc)
    {
    case 1:
        if (strlen(argv[0]) != (sizeof(g_mac_key) << 1))
        {
            goto error;
        }

        tmp = argv[0];
        for (i = 0; i < sizeof(g_mac_key); i++)
        {
            g_mac_key[i] = (parse_digit(tmp[i << 1]) << 4) + parse_digit(tmp[(i << 1) + 1]);
        }
        if (auto_test_is_enable())
        {
            at_send(0, NULL);
            break;
        }
    case 0:
        dbg_printf("mackey ");
        for (i = 0; i < 16; i++)
        {
            dbg_printf("%02x", g_mac_key[i]);
        }
        dbg_printf("\r\n");
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
void shell_register_cmd_netconf(void)
{
    shell_register_pt((shell_program_t)cmd_panid, "panid",
                      BRIEF("get/set panid")
                      SYNOPSIS("panid [<panid>] [<pan_index>]")
                      DESCRIPTION("    panid: 0x0000 ~ 0xffff")
                      DESCRIPTION("pan_index: 0 ~ 3"));
    shell_register_pt((shell_program_t)cmd_channel, "channel",
                      BRIEF("get/set channel")
                      SYNOPSIS("channel [<channel>] [<pan_index>]")
                      DESCRIPTION("  channel: 11 ~ 26")
                      DESCRIPTION("pan_index: 0 ~ 3"));
#if (TEST_MPAN_EN == 0)
    shell_register_pt((shell_program_t)cmd_freq, "freq",
                      BRIEF("get/set rf frequency")
                      SYNOPSIS("freq [<value>]")
                      DESCRIPTION("value: 2402 ~ 2480 frequency (MHz)")
                      EXAMPLE("freq 2402"));
#endif
    shell_register_pt((shell_program_t)cmd_shortaddr, "shortaddr",
                      BRIEF("get/set short addr")
                      SYNOPSIS("shortaddr [<saddr>] [<pan_index>]")
                      DESCRIPTION("    saddr: 0x0000 ~ 0xffff")
                      DESCRIPTION("pan_index: 0 ~ 3"));
    shell_register_pt((shell_program_t)cmd_extaddr, "extaddr",
                      BRIEF("get/set extended addr")
                      SYNOPSIS("extaddr [<extaddr>] [<pan_index>]")
                      DESCRIPTION("  extaddr: 0x0000000000000000 ~ 0xffffffffffffffff")
                      DESCRIPTION("pan_index: 0 ~ 3"));
    shell_register_pt((shell_program_t)cmd_mackey, "mackey",
                      BRIEF("get/set mac test key")
                      SYNOPSIS("mackey [<key>]")
                      DESCRIPTION("key: 16 bytes HEX data")
                      EXAMPLE("mackey 11223344556677889900aabbccddeeff"));
}
