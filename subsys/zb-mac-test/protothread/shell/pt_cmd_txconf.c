/**************************************************************************//**
 * @file      pt_cmd_txconf.c
 * @brief     Source file for txconf command implementation
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

/******************************************************************************/
int cmd_txpower(int argc, char *argv[])
{
    int tx_dbm;
    switch (argc)
    {
    case 1:
        tx_dbm = _strtol((const char *)(argv[0]), (char **)NULL, 10);
        if (tx_dbm > 127 || tx_dbm < -128)
        {
            dbg_printf("Error: InvalidArgs\r\n");
            return FALSE;
        }
        mac_tx_power_set((int8_t)tx_dbm);
    case 0:
        dbg_printf("txpower %d\r\n", mac_tx_power_get());
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_txretry(int argc, char *argv[])
{
    uint8_t retry;
    switch (argc)
    {
    case 1:
        retry = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_txn_retry_set(retry);
    case 0:
        dbg_printf("txretry %d\r\n", mac_txn_retry_get());
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_ccacnt(int argc, char *argv[])
{
    static uint32_t before_cca_cnt = 0;
    uint32_t cca_cnt = mac_cca_cnt_get();
    cca_cnt = cca_cnt - before_cca_cnt;
    before_cca_cnt = mac_cca_cnt_get();
    if (auto_test_is_enable() == 0)
    {
        dbg_printf("CCA count %u (read only)\r\n", cca_cnt);
    }
    else
    {
        at_send(sizeof(cca_cnt), &cca_cnt);
    }
    return TRUE;
}
/******************************************************************************/
int cmd_ccamode(int argc, char *argv[])
{
    uint8_t mode;
    uint8_t th = 0;

    switch (argc)
    {
    case 2:
        th = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
    case 1:
        mode = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (mode > MAC_CCA_CS_ED_AND)
        {
            dbg_printf("Error: InvalidArgs\r\n");
            return FALSE;
        }
        mac_cca_mode_set(mode);
        if (argc == 2 && (mode == MAC_CCA_ED || mode == MAC_CCA_CS_ED || mode == MAC_CCA_CS_ED_AND))
        {
            mac_cca_ed_threshold_set(th);
        }
    case 0:
        mode = mac_cca_mode_get();
        dbg_printf("ccamode %u\r\n", mode);
        if (mode == MAC_CCA_ED || mode == MAC_CCA_CS_ED || mode == MAC_CCA_CS_ED_AND)
        {
            dbg_printf("ED threshold %d\r\n", mac_cca_ed_threshold_get());
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    at_send(0, NULL);
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_csma(int argc, char *argv[])
{
    int8_t key = -1, expr;
    int enable = -1, minbe = -1, maxbe = -1, maxbo = -1;
    int value = 0;

    for (int i = 0; i < argc; i++)
    {
        if (key == 'e' || key == 'i' || key == 'a' || key == 'b')
        {
            value = _strtol((const char *)(argv[i]), (char **)NULL, 10);
            expr = key;
        }
        else
        {
            expr = argv[i][0];
            key = -1;
        }
        switch (expr)
        {
        case 'e':
            if (key == 'e')
            {
                enable = (value == 1) ? 1 : 0;
                key = -1;
            }
            else
            {
                key = 'e';
            }
            break;
        case 'i':
            if (key == 'i')
            {
                minbe = value;
                key = -1;
            }
            else
            {
                key = 'i';
            }
            break;
        case 'a':
            if (key == 'a')
            {
                maxbe = value;
                key = -1;
            }
            else
            {
                key = 'a';
            }
            break;
        case 'b':
            if (key == 'b')
            {
                maxbo = value;
                key = -1;
            }
            else
            {
                key = 'b';
            }
            break;
        default:
            i = argc;
            key = '_';
            break;
        }
    }

    if (key != -1)
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

    if (enable != -1)
    {
        mac_txn_csma_set(enable);
    }
    if (minbe != -1)
    {
        mac_csma_minbe_set(minbe);
    }
    if (maxbe != -1)
    {
        mac_csma_maxbe_set(maxbe);
    }
    if (maxbo != -1)
    {
        mac_csma_max_backoffs_set(maxbo);
    }

    if (auto_test_is_enable() == 0)
    {
        dbg_printf("csma.enable %u\r\n", mac_txn_csma_get());
        dbg_printf("csma.minBe %u\r\n", mac_csma_minbe_get());
        dbg_printf("csma.maxBe %u\r\n", mac_csma_maxbe_get());
        dbg_printf("csma.maxBackoff %u\r\n", mac_csma_max_backoffs_get());
    }
    else
    {
        at_send(0, NULL);
    }
    return TRUE;
}
/******************************************************************************/
int cmd_txgain(int argc, char *argv[])
{
    uint8_t tx_gain = 0;
    switch (argc)
    {
    case 1:
        tx_gain = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_tx_gain_set(tx_gain);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("tx_gain %u\r\n", mac_tx_gain_get());
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
int cmd_notxcrc(int argc, char *argv[])
{
    uint8_t enable = 0;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_txn_nocrc_set(enable ? 1 : 0);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("notxcrc %u\r\n", mac_txn_nocrc_get());
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
int cmd_enh_ack_early(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        g_enh_ack_early = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("enh_ack_early %u\r\n", g_enh_ack_early ? 1 : 0);
        }
        else
        {
            at_send(sizeof(g_enh_ack_early), &g_enh_ack_early);
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
int cmd_tx_interval(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        g_tx_interval_ms = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
    case 0:
        dbg_printf("tx_interval %u ms\r\n", g_tx_interval_ms);
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    //dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_preampt(int argc, char *argv[])
{
#if (TEST_PHY_ARBI_PRIO_EN == 1)
    uint32_t value = 0;
    switch (argc)
    {
    case 1:
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (value > 1)
        {
            goto err_out;
        }
        mac_PHYArbiSetPreamptBT(value);
    case 0:
        extern uint8_t mac_PHYArbiGetPreamptBT(void);
        dbg_printf("preampt %u\r\n", mac_PHYArbiGetPreamptBT());
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
int cmd_phyprio(int argc, char *argv[])
{
#if (TEST_PHY_ARBI_PRIO_EN == 1)
    uint32_t value = 0;
    switch (argc)
    {
    case 1:
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (value > 2)
        {
            goto err_out;
        }
        mac_PHYArbiSetPriority(value);
    case 0:
        dbg_printf("phyprio %u\r\n", mac_PHYArbiGetPriority());
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
uint32_t g_phy_arbitration_method = 0xFFFF;
int cmd_gntmode(int argc, char *argv[])
{
    uint32_t value = 0;
    switch (argc)
    {
    case 1:
        value = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (value > MAC_PHY_ARBI_IMMEDIATE)
        {
            goto err_out;
        }
        g_phy_arbitration_method = value;
    case 0:
        dbg_printf("gntmode %u\r\n", g_mac_attribute.phy_arbitration_en);
        break;
    default:
        goto err_out;
    }
    return TRUE;
err_out:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}
/******************************************************************************/
int cmd_noackrsp(int argc, char *argv[])
{
    uint8_t enable = 0;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        mac_noackrsp_set(enable ? 1 : 0);
    case 0:
        if (auto_test_is_enable() == 0)
        {
            dbg_printf("noackrsp %u\r\n", mac_noackrsp_get());
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
void shell_register_cmd_txconf(void)
{
    g_phy_arbitration_method = 0xFFFF;
    shell_register_pt((shell_program_t)cmd_txpower, "txpower",
                      BRIEF("get/set tx power")
                      SYNOPSIS("txpower [<power_level>]")
                      DESCRIPTION("power_level: dBm * 2")
                      DESCRIPTION("rtl8752h: 7.5 dBm (max)")
                      DESCRIPTION("rtl87x2g:  14 dBm (max)  power_level -8 ~ 28"));
    shell_register_pt((shell_program_t)cmd_txretry, "txretry",
                      BRIEF("get/set maximum number of retries (macMaxFrameRetries)")
                      SYNOPSIS("txretry [<value>]")
                      DESCRIPTION("value: 0 - 7"));
    shell_register_pt((shell_program_t)cmd_ccamode, "ccamode",
                      BRIEF("get/set CCA mode")
                      SYNOPSIS("ccamode [<mode> [<threshold>]]")
                      DESCRIPTION("mode: 0 ~ 4")
                      DESCRIPTION("      0 disable CCA")
                      DESCRIPTION("      1 Energy Detection mode")
                      DESCRIPTION("      2 Carrier Sense mode")
                      DESCRIPTION("      3 CS or ED combination mode")
                      DESCRIPTION("      4 CS and ED combination mode")
                      DESCRIPTION("threshold: 0 ~ 255")
                      DESCRIPTION("           Only applicable to modes 1, 3, and 4"));
    shell_register_pt((shell_program_t)cmd_ccacnt, "ccacnt",
                      BRIEF("get cca count")
                      SYNOPSIS("ccacnt")
                      DESCRIPTION("value: The number of CCA attempts for a single data transmission."));
    shell_register_pt((shell_program_t)cmd_csma, "csma",
                      BRIEF("get/set CSMA parameters")
                      SYNOPSIS("csma [e <enable/disable>] [i <minBe>] [a <maxBe>] [b <maxBackoff>]")
                      DESCRIPTION("enable/disable: 1/0")
                      DESCRIPTION("minBe: 0 ~ 15")
                      DESCRIPTION("maxBe: 0 ~ 15")
                      DESCRIPTION("maxBackoff: 0 ~ 15")
                      EXAMPLE("csma e 1 i 3 a 5 b 4"));
    shell_register_pt((shell_program_t)cmd_txgain, "txgain",
                      BRIEF("get/set TX gain index")
                      SYNOPSIS("txgain [<index>]")
                      DESCRIPTION("index: 0 ~ 127")
                      DESCRIPTION("rtl87x2g: 0 (-4 dBm), 28 (14 dBm)"));
    shell_register_pt((shell_program_t)cmd_notxcrc, "notxcrc",
                      BRIEF("Send frame without FCS")
                      SYNOPSIS("notxcrc [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_enh_ack_early, "enhackearly",
                      BRIEF("Send enhanced ACK at RX early interrupt")
                      SYNOPSIS("enhackearly [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_tx_interval, "txinterval",
                      BRIEF("The interval between continuous send test")
                      SYNOPSIS("txinterval [<value>]")
                      DESCRIPTION("value: 0 ~  4294967295 ms")
                      DESCRIPTION("       0 - no additional delay"));
    shell_register_pt((shell_program_t)cmd_preampt, "preampt",
                      BRIEF("whether the 15.4 can preampt the BT PHY grant")
                      SYNOPSIS("preampt [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_phyprio, "phyprio",
                      BRIEF("get/set priority of the 15.4 MAC for the PHY arbitration")
                      SYNOPSIS("phyprio [<priority>]")
                      DESCRIPTION("phyprio: 0 - low")
                      DESCRIPTION("         1 - medium")
                      DESCRIPTION("         2 - high"));
    shell_register_pt((shell_program_t)cmd_gntmode, "gntmode",
                      BRIEF("get/set grant mode of the 15.4 MAC for the PHY arbitration")
                      SYNOPSIS("gntmode [<mode>]")
                      DESCRIPTION("mode: 0 - zb only")
                      DESCRIPTION("      1 - anchor point")
                      DESCRIPTION("      2 - immediate grant"));
    shell_register_pt((shell_program_t)cmd_noackrsp, "noackrsp",
                      BRIEF("Disable automatic ACK response")
                      SYNOPSIS("noackrsp [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
}
