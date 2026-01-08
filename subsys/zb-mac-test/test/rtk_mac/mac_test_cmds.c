/**************************************************************************//**
* @file     mac_test_cmds.c
* @brief    Source file for IEEE 802.15.4 MAC test command
* @author   felix
* @version  V1.00
* @date     2025-01-17
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
/*============================================================================*
*                              Header Files
*============================================================================*/
#include <osif.h>
#include "shell.h"
#include "dbg_printf.h"
#include "strproc.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"
#if (TEST_ZB_PM_EN == 1)
#include "power_manager_slave.h"
#include "power_manager_interface.h"
#endif
#include "protothread.h"
#include "shell/pt_cmd.h"
#if TEST_ATCMD_ZIGBTEST
#include "at_zigbtest_mac_wrapper.h"
#include "../../../at_zigbtest/at_zigbtest_module.h"
#endif

#if (TEST_ADV_SWTIMER == 0)
static void swtimer_finish_cb(void *arg)
{
    mac_timer_handle_t *swtimer = (mac_timer_handle_t *)arg;
    dbg_printf("sw timer free: %p\r\n", swtimer);
    mac_sw_timer_free(swtimer);
}
#endif

static void swtimer_timeout_cb(void *arg)
{
    uint32_t now = mac_btus_get();
    dbg_printf("timer %p fire %u\r\n", arg, now);
#if (TEST_ADV_SWTIMER == 0)
    work_queue_msg_send(swtimer_finish_cb, arg);
#endif
}

static int cmd_swtimer(int argc, char *argv[])
{
    uint32_t timeout;
    mac_timer_handle_t *swtimer = NULL;
#if (TEST_ADV_SWTIMER == 1)
    if (strcmp(argv[0], "alloc") == 0)
    {
        swtimer = mac_sw_timer_alloc();
        if (swtimer == NULL)
        {
            dbg_printf("Error: sw timer alloc fail\r\n");
        }
        else
        {
            dbg_printf("sw timer: %p\r\n", swtimer);
        }
    }
    else if (strcmp(argv[0], "start") == 0)
    {
        swtimer = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
        timeout = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
        uint32_t now = mac_btus_get();
        mac_sw_timer_start(swtimer, now + timeout, swtimer_timeout_cb, (void *)swtimer);
        dbg_printf("swtimer %p start: now %u timeout %u us\r\n", swtimer, now, timeout);
    }
    else if (strcmp(argv[0], "stop") == 0)
    {
        swtimer = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
        dbg_printf("sw timer stop: %p\r\n", swtimer);
        mac_sw_timer_stop(swtimer);
    }
    else if (strcmp(argv[0], "free") == 0)
    {
        swtimer = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
        dbg_printf("sw timer free: %p\r\n", swtimer);
        mac_sw_timer_free(swtimer);
    }
#else
    if (argc == 1)
    {
        swtimer = mac_sw_timer_alloc();
        if (swtimer == NULL)
        {
            dbg_printf("Error: sw timer alloc fail\r\n");
            return FALSE;
        }
        else
        {
            dbg_printf("sw timer alloc: %p\r\n", swtimer);
        }
        timeout = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        uint32_t now = mac_btus_get();
        mac_sw_timer_start(swtimer, now + timeout, swtimer_timeout_cb, (void *)swtimer);
        dbg_printf("swtimer start: now %u timeout %u us\r\n", now, timeout);
    }
    else
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
#endif
    dbg_printf("Done\r\n");
    return TRUE;
}

static int cmd_bttimer(int argc, char *argv[])
{
    uint32_t timeout;

    if (argc == 1)
    {
        timeout = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        uint32_t now = mac_btus_get();
        dbg_printf("bttimer start: now %u timeout %u us\r\n", now, timeout);
        mac_btus_intr_set(MAC_BT_TIMER0, mac_btus_get() + timeout);
        osif_sem_take(zb_sem, 0xffffffff);
        dbg_printf("fire %u us\r\n", mac_btus_get());
        dbg_printf("Done\r\n");
    }
    else
    {
        dbg_printf("Error: InvalidArgs\r\n");
    }
    return TRUE;
}

#if (TEST_ZB_PM_EN == 1)
static void cmd_pm_exit(void)
{
    dbg_printf("exit_pm = %u pm_wakeup_diff(min/avg/max) = %d/%d/%d us\r\n", mac_btus_get(),
               g_zbpm_wakeup_diff_min >> 7, g_zbpm_wakeup_diff_avg >> 7, g_zbpm_wakeup_diff_max >> 7);
}

static void cmd_pm_set(void *arg)
{
    uint32_t timeout = (uint32_t)arg;
    dbg_printf("enter_pm %u duration %u us\r\n", mac_btus_get(), timeout);
    zbmac_power_manager_set(timeout, 0);
}

static int cmd_pm(int argc, char *argv[])
{
    uint32_t timeout = 1000000;

    if (argc >= 1)
    {
        if (strcmp(argv[0], "init") == 0)
        {
            zbmac_power_manager_init(cmd_pm_exit);
            dbg_printf("Done\r\n");
        }
        else if (strcmp(argv[0], "set") == 0)
        {
            if (argc > 1)
            {
                timeout = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
            }
            work_queue_msg_send(cmd_pm_set, (void *)timeout);
        }
        else if (strcmp(argv[0], "wakeup") == 0)
        {
            if (PM_UNIT_INACTIVE == power_manager_interface_get_unit_status(PM_SLAVE_ZIGBEE, PM_UNIT_ZIGBEE))
            {
                power_manager_master_initiate_wakeup(PM_SLAVE_ZIGBEE, (PMSystemLevel)(PM_UNIT_ZIGBEE >> 2),
                                                     PM_UNIT_ZIGBEE);
                while (PM_UNIT_INACTIVE == power_manager_interface_get_unit_status(PM_SLAVE_ZIGBEE,
                                                                                   PM_UNIT_ZIGBEE));
                osif_sem_give(zb_sem);
            }
        }
    }
    else
    {
        dbg_printf("Error: InvalidArgs\r\n");
    }
    return TRUE;
}

static uint32_t g_demo_sleep_duration;
static void demo_cb(void *arg)
{
    static uint32_t seq = 0;
    fc_t fc = {0};
    uint32_t start = mac_btus_get();
    uint16_t panid = mac_panid_get();
    uint16_t saddr = mac_short_addr_get();
    uint16_t daddr = saddr;

    fc.type = FRAME_TYPE_DATA;
    fc.sec_en = 0;
    fc.pending = 0;
    fc.ack_req = 1;
    fc.panid_compress = 1;
    fc.dst_addr_mode = ADDR_MODE_SHORT;
    fc.ver = FRAME_VER_2006;
    fc.src_addr_mode = ADDR_MODE_SHORT;
    g_tx_buf.len = generate_ieee_frame(FRAME_TYPE_DATA, g_tx_buf.buf, 125, fc, 0,
                                       panid, (uint8_t *)&saddr, panid, (uint8_t *)&daddr,
                                       NV_FIELD, NV_FIELD, NV_FIELD, NV_FIELD);
    GEN_SEQ_DATA_MV_PTR(g_tx_buf.buf, 0, 10, g_tx_buf.len);
    mac_txn_payload_set(0, g_tx_buf.len, g_tx_buf.buf);
    RESET_TXDOWN();
    dbg_printf("send ");
    mac_txn_trig(fc.ack_req, fc.sec_en);
    WAIT_FOR_TXDOWN_UNTIL(mac_btus_get() > (start + 1000000));
    print_tx_result(seq++, 0xff);
    uint32_t end = mac_btus_get();
    dbg_printf("\t proc_t %u\r\n", end - start);
    zbmac_power_manager_set(g_demo_sleep_duration, 0);
}

static void demo_pm_exit(void)
{
    work_queue_msg_send(demo_cb, NULL);
    dbg_printf("exit_pm = %u pm_wakeup_diff(min/avg/max) = %d/%d/%d us\r\n", mac_btus_get(),
               g_zbpm_wakeup_diff_min >> 7, g_zbpm_wakeup_diff_avg >> 7, g_zbpm_wakeup_diff_max >> 7);
}

static int cmd_demo(uint32_t argc, uint8_t  *argv[])
{
    if (argc != 1)
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    g_demo_sleep_duration = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
    zbmac_power_manager_init(demo_pm_exit);
    demo_cb(NULL);
    return TRUE;
}
#endif

static int cmd_config_dump(int argc, char *argv[])
{
#if (TEST_PROTOTHREAD_EN == 1)
    cmd_time(0, NULL);
    dbg_printf("[Basic PAN Config]\r\n");
    cmd_panid(0, NULL);
    cmd_channel(0, NULL);
    cmd_freq(0, NULL);
    cmd_shortaddr(0, NULL);
    cmd_extaddr(0, NULL);
    cmd_mackey(0, NULL);
#if TEST_BT_ADV_EN
    cmd_ble_adv(0, NULL);
#endif /* TEST_BT_ADV_EN */
    cmd_radio(0, NULL);
#if (TEST_BT_PM_EN == 1)
    dbg_printf("TEST_BT_PM_EN 1 (rd only)\r\n");
#else
    dbg_printf("TEST_BT_PM_EN 0 (rd only)\r\n");
#endif

    dbg_printf("\r\n[TX Config]\r\n");
    cmd_ccamode(0, NULL);
    cmd_csma(0, NULL);
    cmd_txretry(0, NULL);
    cmd_txpower(0, NULL);
    cmd_notxcrc(0, NULL);
    cmd_enh_ack_early(0, NULL);
    cmd_tx_interval(0, NULL);
    cmd_preampt(0, NULL);
    cmd_phyprio(0, NULL);
    cmd_gntmode(0, NULL);

    dbg_printf("\r\n[RX Config]\r\n");
    cmd_promiscuous(0, NULL);
    cmd_recverrpkt(0, NULL);
    cmd_scanmode(0, NULL);
    cmd_rxframetype(0, NULL);
    cmd_srcmatchmode(0, NULL);
    cmd_srcmatchfilter(0, NULL);
    cmd_test_enh_ack_late(0, NULL);
#else
    uint64_t now_us = mac_timestamp_get();
    dbg_printf("BTUS[%u]MAC[%llu]\r\n", (uint32_t)(now_us % MAX_BT_CLOCK_COUNTER), now_us);

    dbg_printf("[Basic PAN Config]\r\n");
    dbg_printf("panid 0x%04x\r\n", mac_panid_get());
    dbg_printf("channel %u\r\n", mac_channel_get());
    dbg_printf("shortaddr 0x%04x\r\n", mac_short_addr_get());
    uint64_t laddr = 0;
    memcpy(&laddr, mac_long_addr_get(), sizeof(laddr));
    dbg_printf("extaddr 0x%016llx\r\n", laddr);
    dbg_printf("mackey ");
    for (int i = 0; i < 16; i++)
    {
        dbg_printf("%02x", g_mac_key[i]);
    }
    dbg_printf("radio state %u (rd only)\r\n", mac_radio_state_get());
#if (TEST_BT_PM_EN == 1)
    dbg_printf("TEST_BT_PM_EN 1 (rd only)\r\n");
#else
    dbg_printf("TEST_BT_PM_EN 0 (rd only)\r\n");
#endif

    dbg_printf("\r\n[TX Config]\r\n");
    uint8_t mode = mac_cca_mode_get();
    dbg_printf("ccamode %u\r\n", mode);
    if (mode == MAC_CCA_ED || mode == MAC_CCA_CS_ED || mode == MAC_CCA_CS_ED_AND)
    {
        dbg_printf("ED threshold %d\r\n", mac_cca_ed_threshold_get());
    }
    dbg_printf("csma.enable %u\r\n", mac_txn_csma_get());
    dbg_printf("csma.minBe %u\r\n", mac_csma_minbe_get());
    dbg_printf("csma.maxBe %u\r\n", mac_csma_maxbe_get());
    dbg_printf("csma.maxBackoff %u\r\n", mac_csma_max_backoffs_get());
    dbg_printf("txpower %d\r\n", mac_tx_power_get());
    dbg_printf("tx_gain %u\r\n", mac_tx_gain_get());
    dbg_printf("txretry %d\r\n", mac_txn_retry_get());
    dbg_printf("notxcrc %u\r\n", mac_txn_nocrc_get());
    dbg_printf("enh_ack_early %u\r\n", g_enh_ack_early ? 1 : 0);
    dbg_printf("tx_interval %u ms\r\n", g_tx_interval_ms);

    dbg_printf("\r\n[RX Config]\r\n");
    dbg_printf("promiscuous %u\r\n", mac_promiscuous_get());
    dbg_printf("recverrpkt %u\r\n", mac_rx_err_pkt_get());
    dbg_printf("scanmode %u\r\n", mac_scan_mode_get());
    dbg_printf("rxframetype %s\r\n",
               (mac_rx_frm_filter_get(FRAME_VER_2006) & 0x4) ? "Include ACK" : "Default");
    dbg_printf("srcmatchmode %u\r\n", mac_addr_match_mode_get());
    dbg_printf("g_test_enh_ack_late %u\r\n", g_test_enh_ack_late);
    dbg_printf("\r\n");
#endif
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_test_cmd(void)
{
    /* brief, synopsis, description, example */
    /* System Command */
    shell_register((shell_program_t)cmd_swtimer, "swtimer",
                   BRIEF("MAC sw timer test")
                   SYNOPSIS("swtimer [<timeout>]")
                   DESCRIPTION("timeout: 25 ~ 4294967295 (us)")
                   EXAMPLE("swtimer 2000000 - timeout after 2s"));
    shell_register((shell_program_t)cmd_bttimer, "bttimer",
                   BRIEF("BT timer test")
                   SYNOPSIS("bttimer <timeout>")
                   DESCRIPTION("timeout: 50 ~ 4294967295 (us)")
                   EXAMPLE("bttimer 2000000 - timeout after 2s"));
#if (TEST_ZB_PM_EN == 1)
    shell_register((shell_program_t)cmd_pm, "pm",
                   BRIEF("mac power management test")
                   SYNOPSIS("pm <init/set> [<interval>]")
                   DESCRIPTION("init: mac power management initialization")
                   DESCRIPTION("set: set wake up interval")
                   DESCRIPTION("interval: 2000 ~ 4294967295 (us)")
                   EXAMPLE("pm set 2000000 - wake up after 2s"));
#endif
    /* Config command */
    shell_register((shell_program_t)cmd_config_dump, "config",
                   BRIEF("Show configuration parameters"));

    /* APP Commands */
#if (TEST_ZB_PM_EN == 1)
    shell_register((shell_program_t)cmd_demo, "demo",
                   BRIEF("demo test command")
                   SYNOPSIS("demo <time>")
                   DESCRIPTION("time: 2000 ~ 4294967295 (us) sleep duration")
                   DESCRIPTION("Periodically do <sleep - wake up - send> in this demo"));
#endif

#if TEST_ATCMD_ZIGBTEST
    shell_register_cmd_at_zigbtest();
#endif
}
