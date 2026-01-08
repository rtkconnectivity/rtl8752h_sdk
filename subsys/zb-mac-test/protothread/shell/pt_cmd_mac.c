/**************************************************************************//**
 * @file      pt_cmd_mac.c
 * @brief     Source file for mac command implementation
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
#if (TEST_BT_PM_EN == 1)
#include "power_manager_interface.h"
#endif
/******************************************************************************/
int cmd_mac_init(int argc, char *argv[])
{
    zb_mac_drv_init();
    etimer_init();
    if (auto_test_is_enable() == 0)
    {
        dbg_printf("MAC Init Done. Now %u\r\n", mac_btus_get());
    }
    else
    {
        at_send(0, NULL);
    }
    return TRUE;
}
/******************************************************************************/
int cmd_mac_enable(int argc, char *argv[])
{
    if (argc == 1)
    {
        if (argv[0][0] == '1')
        {
            mac_enable();
        }
        else if (argv[0][0] == '0')
        {
            mac_disable();
        }
        else
        {
            goto prf;
        }
        dbg_printf("Done\r\n");
        return TRUE;
    }
prf:
    dbg_printf("mac_IsEnabled %u\r\n", mac_enabled_check());
    return TRUE;
}
/******************************************************************************/
int cmd_mac_disable(int argc, char *argv[])
{
    mac_RadioOff();
    // busy wait 100 us
    uint64_t start = clock_time_us();
    while (clock_time_us() - start < 100);
    mac_disable();
    dbg_printf("Done\r\n");
    return TRUE;
}
/******************************************************************************/
int cmd_radio(int argc, char *argv[])
{
    if (argc == 2)
    {
        bool on = _strtoul((const char *)(argv[1]), (char **)NULL, 10);

        if (strcmp(argv[0], "rf") == 0)
        {
            if (on)
            {
                mac_radio_on();
            }
            else
            {
                mac_radio_off();
            }
        }
#if (TEST_BT_PM_EN == 1)
        else if (strcmp(argv[0], "bt") == 0)
        {
            if (on)
            {
                PMUnitStatus state = power_manager_interface_get_unit_status(PM_SLAVE_BTMAC, PM_UNIT_BTMAC);
                dbg_printf("BT_PM state %u\r\n", state);
                power_manager_interface_check_unit_active(PM_SLAVE_BTMAC, PM_UNIT_BTMAC);
            }
        }
#endif
    }
#if (TEST_BT_PM_EN == 1)
    PMUnitStatus state = power_manager_interface_get_unit_status(PM_SLAVE_BTMAC, PM_UNIT_BTMAC);
    dbg_printf("BT_PM state %u (rd only)\r\n", state);
#endif
    dbg_printf("radio state %u (rd only)\r\n", mac_radio_state_get());
    return TRUE;
}
/******************************************************************************/
#define MAX_CH_NUM 16
static struct ctimer edscan_ctimer;
static int8_t min_peak_ed_level;
static int8_t min_avg_ed_level;

struct ch_stats_s
{
    int16_t ewma_peak_level;
    int16_t ewma_avg_level;
};

static struct edscan_conf
{
    uint8_t ch[MAX_CH_NUM];
    struct ch_stats_s ch_stats[MAX_CH_NUM];
    uint32_t ch_cnt : 8;
    uint32_t round  : 24;
    uint32_t duration;
    uint32_t interval;
} conf;

PROCESS(edscan_process, "edscan");
static void edscan_post_continue_event(void *ptr)
{
    process_post(&edscan_process, PROCESS_EVENT_CONTINUE, ptr);
}

PROCESS_THREAD(edscan_process, ev, data)
{
    uint32_t idx = 0, round = 0;
    uint16_t freq;
    int8_t peak_ed_level = 0, avg_ed_level = 0;
    uint32_t most_clean_idx = 0;
    int32_t most_clean_level = 0;

    PROCESS_BEGIN();
    dbg_printf("[%u] %s start\r\n", clock_time(), PROCESS_NAME_STRING(PROCESS_CURRENT()));
    dbg_printf("duration %u round %u interval %u\r\n", conf.duration, conf.round, conf.interval);
    min_peak_ed_level = 0;
    min_avg_ed_level = 0;
    do
    {
        //dbg_printf("round %u\r\n", (uint32_t)data & 0xffffff);
        do
        {
            idx = (uint32_t)data >> 24;
#if (TEST_DUAL_CHNL_EN == 1)
            uint8_t dual_ch_enable = mpan_DualChannelIsEnabled();
            if (dual_ch_enable)
            {
                freq = mpan_GetChannel(0);
                mpan_PauseChannelSwitching(0);
            }
            else
#endif
            {
                freq = mac_freq_get(); // backup orig freq
            }
            mac_cs_enter();
            mac_channel_set(conf.ch[idx]);
            mac_cs_exit();
            // Prevent channel changes from other tasks, use block wait for ed scan
            if (mac_ed_scan_poll(conf.duration, &peak_ed_level, &avg_ed_level) == MAC_STS_SUCCESS)
            {
                if (conf.ch_stats[idx].ewma_peak_level == 0 && conf.ch_stats[idx].ewma_avg_level == 0)
                {
                    conf.ch_stats[idx].ewma_peak_level = peak_ed_level << 7;
                    conf.ch_stats[idx].ewma_avg_level = avg_ed_level << 7;
                }
                else
                {
                    conf.ch_stats[idx].ewma_peak_level = ((conf.ch_stats[idx].ewma_peak_level * 7 +
                                                           (peak_ed_level << 7)) >> 3);
                    conf.ch_stats[idx].ewma_avg_level = ((conf.ch_stats[idx].ewma_avg_level * 7 +
                                                          (avg_ed_level << 7)) >> 3);
                }
                if (peak_ed_level < min_peak_ed_level)
                {
                    min_peak_ed_level = peak_ed_level;
                }
                if (avg_ed_level < min_avg_ed_level)
                {
                    min_avg_ed_level = avg_ed_level;
                }
                dbg_printf("[%u] ch %u peak %d avg %d ewma_peak %d ewma_avg %d\r\n", clock_time(), conf.ch[idx],
                           peak_ed_level, avg_ed_level, conf.ch_stats[idx].ewma_peak_level >> 7,
                           conf.ch_stats[idx].ewma_avg_level >> 7);
            }
            else
            {
                dbg_printf("[%u] ch %u edscan_poll fail\r\n", clock_time(), conf.ch[idx]);
            }
#if (TEST_DUAL_CHNL_EN == 1)
            if (dual_ch_enable)
            {
                mac_cs_enter();
                mpan_SetChannel(freq, 0);
                mac_cs_exit();
                mpan_ResumeChannelSwitching();
            }
            else
#endif
            {
                mac_cs_enter();
                mac_freq_set(freq); // restore orig freq
                mac_cs_exit();
            }
            idx++; // move to next ch
            if (idx >= conf.ch_cnt)
            {
                break;
            }
            else
            {
                idx = ((idx << 24) | ((uint32_t)data & 0xffffff));
                process_post(&edscan_process, PROCESS_EVENT_CONTINUE, (void *)idx);
            }
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE || ev == PROCESS_EVENT_EXIT);
            if (ev == PROCESS_EVENT_EXIT)
            {
                //PROCESS_EXIT();
                goto err_out;
            }
        }
        while (1);

        round = (uint32_t)data & 0xffffff;
        round++; // move to next round
        if (conf.round && round >= conf.round)
        {
            break;
        }
        else if (conf.interval)
        {
            ctimer_set(&edscan_ctimer, conf.interval, edscan_post_continue_event, (void *)round);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE || ev == PROCESS_EVENT_EXIT);
            if (ev == PROCESS_EVENT_EXIT)
            {
                ctimer_stop(&edscan_ctimer);
                //PROCESS_EXIT();
                goto err_out;
            }
        }
        else
        {
            process_post(&edscan_process, PROCESS_EVENT_CONTINUE, (void *)round);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE || ev == PROCESS_EVENT_EXIT);
            if (ev == PROCESS_EVENT_EXIT)
            {
                //PROCESS_EXIT();
                goto err_out;
            }
        }
    }
    while (1);

err_out:
    for (idx = 0; idx < conf.ch_cnt; idx++)
    {
        int32_t level = conf.ch_stats[idx].ewma_peak_level + conf.ch_stats[idx].ewma_avg_level;
        if (level < most_clean_level)
        {
            most_clean_level = level;
            most_clean_idx = idx;
        }
    }

    if (auto_test_is_enable())
    {
        if (conf.ch_cnt > 0)
        {
            uint8_t best_channel = conf.ch[most_clean_idx];
            at_send(sizeof(best_channel), &best_channel);
        }
    }
    else
    {
        dbg_printf("\r\n--- ED Scan Report ---\r\n");
        dbg_printf("Overall min peak: %d dBm, min avg: %d dBm\r\n", min_peak_ed_level, min_avg_ed_level);
        for (idx = 0; idx < conf.ch_cnt; idx++)
        {
            dbg_printf("%c CH %-2u | Avg: %4d dBm | Peak: %4d dBm\r\n",
                       (idx == most_clean_idx) ? 'o' : ' ',
                       conf.ch[idx],
                       conf.ch_stats[idx].ewma_avg_level >> 7,
                       conf.ch_stats[idx].ewma_peak_level >> 7);
        }
        dbg_printf("------------------------\r\n");
        if (conf.ch_cnt > 0)
        {
            dbg_printf("Recommendation: Cleanest channel is %u (Sum: %d dBm)\r\n", conf.ch[most_clean_idx],
                       most_clean_level >> 7);
        }
    }

    PROCESS_END();
}

static int cmd_edscan(uint32_t argc, uint8_t  *argv[])
{
    if (process_is_running(&edscan_process))
    {
        if (argc == 1)
        {
            if (strcmp((const char *)argv[0], "reset") == 0)
            {
                for (uint32_t idx = 0; idx < conf.ch_cnt; idx++)
                {
                    conf.ch_stats[idx].ewma_peak_level = 0;
                    conf.ch_stats[idx].ewma_avg_level = 0;
                }
                dbg_printf("Done\r\n");
                goto done;
            }
        }
        dbg_printf("%s is running\r\n", PROCESS_NAME_STRING(&edscan_process));
        return FALSE;
    }

    if (argc < 3)
    {
        dbg_printf("Error: Invalid arguments.\r\n");
        dbg_printf("Usage: edscan <duration> <round> <interval> [ch1] [ch2] ...\r\n");
        dbg_printf("       edscan reset\r\n");
        return FALSE;
    }

    memset(&conf, 0, sizeof(struct edscan_conf));
    conf.duration = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
    conf.round = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
    conf.interval = _strtoul((const char *)(argv[2]), (char **)NULL, 10);

    if (argc == 3)   // scan on all channels
    {
        for (uint32_t i = 0; i < 16; i++)
        {
            conf.ch[i] = i + 11;
            conf.ch_cnt++;
        }
    }
    else
    {
        for (uint32_t i = 3; i < argc; i++)
        {
            if (conf.ch_cnt >= MAX_CH_NUM)
            {
                dbg_printf("Warning: Too many channels. Max is %d. Ignoring extras.\r\n", MAX_CH_NUM);
                break;
            }
            uint8_t channel = _strtoul((const char *)(argv[i]), (char **)NULL, 10);
            if (channel < 11 || channel > 26)
            {
                dbg_printf("Warning: Invalid channel number '%s'. Must be between 11 and 26. Skipping.\r\n",
                           argv[i]);
                continue;
            }
            conf.ch[conf.ch_cnt] = channel;
            conf.ch_cnt++;
        }
    }

    if (conf.ch_cnt > 0)
    {
        process_start(&edscan_process, NULL);
    }
    else
    {
        dbg_printf("Error: No channels to scan.\r\n");
        return FALSE;
    }
done:
    return TRUE;
}

void shell_register_cmd_mac(void)
{
    // do some global variable init
    edscan_process.state = 0;
    shell_register_pt((shell_program_t)cmd_mac_init, "mac_init",
                      BRIEF("Initial mac")
                      DESCRIPTION("Must be called once at startup before any test commands"));
    shell_register_pt((shell_program_t)cmd_mac_disable, "mac_disable",
                      BRIEF("Disable mac")
                      DESCRIPTION("After disabling, you need to do mac_init command again to continue executing other test commands"));
    shell_register_pt((shell_program_t)cmd_mac_enable, "mac_enable",
                      BRIEF("get mac state or enable it")
                      SYNOPSIS("mac_enable [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
    shell_register_pt((shell_program_t)cmd_radio, "radio",
                      BRIEF("get/set radio state")
                      SYNOPSIS("radio <bt/rf> [<on/off>]")
                      DESCRIPTION("on/off: 1/0"));
    shell_register_pt((shell_program_t)cmd_edscan, "edscan",
                      BRIEF("start a scheduled ED scan procedure")
                      SYNOPSIS("edscan <duration> <round> <interval> [<ch0> <ch1> ... <chN>]")
                      SYNOPSIS("edscan reset")
                      DESCRIPTION("Arguments:")
                      DESCRIPTION("duration: 1 ~ 4,294,967,295 us")
                      DESCRIPTION("round: 0 ~ 16,777,215")
                      DESCRIPTION("       0 - infinite loop")
                      DESCRIPTION("interval: 0 ~ 4,294,967,295 ms")
                      DESCRIPTION("          0 - no delay")
                      DESCRIPTION("ch: 11 ~ 26")
                      DESCRIPTION("Command:")
                      DESCRIPTION("reset: Clear statistics while ED scan is running.")
                      EXAMPLE("ed_scan 500 10 100 11 12 13 - Scan channels 11,12,13 for 10 rounds with 100ms interval.")
                      EXAMPLE("ed_scan 500 10 100          - Scan on all channels"));
}
