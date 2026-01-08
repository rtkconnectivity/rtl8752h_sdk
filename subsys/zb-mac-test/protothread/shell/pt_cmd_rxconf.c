/**************************************************************************//**
 * @file      pt_cmd_rxconf.c
 * @brief     Source file for rxconf command implementation
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

int cmd_promiscuous(int argc, char *argv[])
{
    uint8_t enable = 0;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_promiscuous_set(enable ? 1 : 0);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("promiscuous %u\r\n", mac_promiscuous_get());
        }
        else
        {
            at_send(0, NULL);
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_recverrpkt(int argc, char *argv[])
{
    uint8_t enable = 0;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_rx_err_pkt_set(enable ? 1 : 0);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("recverrpkt %u\r\n", mac_rx_err_pkt_get());
        }
        else
        {
            at_send(0, NULL);
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_scanmode(uint32_t argc, uint8_t  *argv[])
{
    uint32_t mode;
    switch (argc)
    {
    case 1:
        mode = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_scan_mode_set(mode);
    case 0:
        dbg_printf("scanmode %u\r\n", mac_scan_mode_get());
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_rxframetype(int argc, char *argv[])
{
    /*
     * bit 0: beacon
     * bit 1: data
     * bit 2: ack
     * bit 3: command
     * bit 4: reserved
     * bit 5: multipurpose
    */
    uint8_t rxftype = 0x0b; // 00001011
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
        rxftype = _strtoul((const char *)(argv[0]), (char **)NULL, 16);
        rxftype &= 0x2f;
#if TEST_MPAN_EN
        mpan_SetAcceptFrm(FRAME_VER_2006, rxftype, pan_idx);
#else
        mac_rx_frm_filter_set(FRAME_VER_2006, rxftype);
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
            dbg_printf("rxframetype[%d] 0x%x\r\n", i, mpan_GetAcceptFrm(FRAME_VER_2006, i));
        }
#else
        dbg_printf("rxframetype 0x%x\r\n", mac_rx_frm_filter_get(FRAME_VER_2006));
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
int cmd_test_enh_ack_late(int argc, char *argv[])
{
    uint8_t enable;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        g_test_enh_ack_late = (enable ? 1 : 0);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("g_test_enh_ack_late %u\r\n", g_test_enh_ack_late);
        }
        else
        {
            at_send(0, NULL);
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

#if Auto_test
    unsigned char bytes[] = {0x04, 0x0e, 0x06, 0x02, 0x00, 0xfc, 0x01, 0x01, g_test_enh_ack_late};
    UART_SendData(UART2, bytes, sizeof(bytes));
#endif
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
void shell_register_cmd_rxconf(void)
{
    shell_register_pt((shell_program_t)cmd_promiscuous, "promiscuous",
                      BRIEF("Accept all packets with CRC OK")
                      SYNOPSIS("promiscuous [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_recverrpkt, "recverrpkt",
                      BRIEF("Accept all kinds of pkt(including CRC error)")
                      SYNOPSIS("recverrpkt [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_scanmode, "scanmode",
                      BRIEF("enable RX Filter scan mode")
                      SYNOPSIS("scanmode [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0")
                      DESCRIPTION("When scan mode is enabled, only Beacon frame with FCS correct will be accepted by RX filter"));
    shell_register_pt((shell_program_t)cmd_rxframetype, "rxframetype",
                      BRIEF("RX Frame Type Filter for IEEE802.15.4 version 2006")
                      SYNOPSIS("rxframetype [<type>] [<pan_index>]")
                      DESCRIPTION("     type: 0x0 ~ 0x3f")
                      DESCRIPTION("           bit 0 - beacon")
                      DESCRIPTION("           bit 1 - data")
                      DESCRIPTION("           bit 2 - ack")
                      DESCRIPTION("           bit 3 - command")
                      DESCRIPTION("           bit 4 - reserved")
                      DESCRIPTION("           bit 5 - multipurpose")
                      DESCRIPTION("pan_index: 0 ~ 3"));
    shell_register_pt((shell_program_t)cmd_test_enh_ack_late, "enh_ack_late",
                      BRIEF("get/set enh_ack_late test flag")
                      SYNOPSIS("enh_ack_late [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0")
                      DESCRIPTION("                1 - wait the time window opened for trigger Enh-Ack TX")
                      DESCRIPTION("                0 - force to trigger Enh-Ack TX"));
}
