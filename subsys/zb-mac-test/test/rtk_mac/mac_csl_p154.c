/**************************************************************************//**
 * @file     mac_csl_p154.c
 * @brief    Source file for IEEE - 802.15.4 proprietary CSL implementation.
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
#include "mac_driver_interface.h"
#include "mac_802154_frame_parser.h"
#include "mac_test_common.h"
#include "mac_csl.h"
#include "ftl.h"
#if (TEST_ZB_PM_EN == 1)
#include "power_manager_unit_zbmac.h"
static zbpm_callback_t app_zbpm_enter = NULL;
#endif
#if TEST_GPIO_DBG_EN
#include "rtl876x_gpio.h"
#endif
#include "net/pt_mac.h"

static uint8_t g_csl_tx_seq = 0;

static void p154_csma_arq_enable(void)
{
    dbg_printf("enable csma and arq\r\n");
    mac_cca_mode_set(MAC_CCA_CS);
    mac_txn_csma_set(true);
    mac_txn_retry_set(3);
}

static void p154_csma_arq_disable(void)
{
    dbg_printf("disable csma and arq\r\n");
    g_csl_tx_seq = 1;
    mac_cca_mode_set(MAC_CCA_NONE);
    mac_txn_csma_set(false);
    mac_txn_retry_set(0);
    if (g_csl_role == CSL_ROLE_COORD)
    {
        g_csl_tx_check_offest = ((g_csl_listen_window - 400) >> 1) - 760;
    }
}

static void p154_config_init(void)
{
    if (g_csl_role == CSL_ROLE_ENDPOINT)
    {
        //g_csl_ppm = -3;
        g_csl_prepare_to_rx = 400;
        g_csl_ack_require_time = 900;
    }
    else
    {
        //g_csl_tx_check_offest = -740;
    }
}

static int p154_send(uint8_t *data, uint8_t data_len, uint32_t *tx_timestamp)
{
    fc_t fc = {0};
    uint16_t panid = mac_panid_get();
    uint16_t saddr = mac_short_addr_get();

    fc.type = FRAME_TYPE_COMMAND;
    fc.sec_en = 0;
    fc.pending = 0;
    fc.ack_req = 1;
    fc.panid_compress = 1;
    fc.dst_addr_mode = ADDR_MODE_SHORT;
    fc.ver = FRAME_VER_2006;
    fc.src_addr_mode = ADDR_MODE_SHORT;
    g_tx_buf.len = generate_ieee_frame(FRAME_TYPE_COMMAND, g_tx_buf.buf, 125, fc, g_csl_tx_seq,
                                       panid, (uint8_t *)&saddr, panid, (uint8_t *)&g_csl_peer_addr,
                                       NV_FIELD, NV_FIELD, MAC_CMD_CSL, NV_FIELD);
    CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], data, data_len, g_tx_buf.len);
    mac_txn_payload_set(0, g_tx_buf.len, g_tx_buf.buf);
    RESET_TXDOWN();
#if TEST_GPIO_DBG_EN
    debug_gpio_high(GPIO_OUTPUT_PIN_1);
#endif
    mac_txn_trig(fc.ack_req, fc.sec_en);
    trigger_gpio_at_anchor(0);
    WAIT_FOR_TXDOWN();
#if TEST_GPIO_DBG_EN
    debug_gpio_low(GPIO_OUTPUT_PIN_1);
#endif
    dbg_printf("[%u][%u]send ", mac_btus_get(), (uint32_t)mac_timestamp_get());
    print_tx_result(g_csl_tx_seq, 0xff);
    g_csl_tx_seq++;
    if (g_tx_done == TX_SUCCESS)
    {
        // get tx time as csl anchor point
        *tx_timestamp = (uint32_t)(bt_clk_offset + mac_txn_timestamp_get());
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static void p154_input(uint8_t *rx_data, uint8_t pan_idx)
{
    uint8_t *buf = MAC_RX_PKT(rx_data);
    uint8_t buf_len = MAC_RX_PKT_LEN(rx_data);
    uint32_t rx_timestamp = MAC_RX_PKT_TIMESTAMP(rx_data);
    uint8_t hdr_len = mac_802154_frame_parser_addressing_end_offset_get(rx_data) + 1;
    bool is_src_addr_extended = false;
    const uint8_t *p_saddr = mac_802154_frame_parser_src_addr_get(rx_data, &is_src_addr_extended);
    uint16_t saddr = (p_saddr[1] << 8) | p_saddr[0];

    uint8_t *optptr = rx_data + hdr_len;
    uint8_t *end = buf + buf_len;
    //dbg_mem_dump(optptr, end - optptr);
    csl_input(optptr, end - optptr, rx_timestamp, saddr);
}

MAC_INPUT_HANDLER(csl_handler, 0, MAC_CMD_CSL, p154_input);

static void *p154_timer_init(void)
{
    return (void *)mac_sw_timer_alloc();
}

static void p154_timer_start(void *timer, uint32_t timeout, void *callback, void *arg)
{
    uint32_t target = mac_btus_get() + timeout;
    mac_sw_timer_start((pmac_timer_handle_t)timer, target, callback, arg);
}

static void p154_timer_stop(void *timer)
{
    mac_sw_timer_stop((pmac_timer_handle_t)timer);
}

static void p154_pm_init(void *pm_enter_cb, void *pm_exit_cb)
{
#if (TEST_ZB_PM_EN == 1)
    app_zbpm_enter = (zbpm_callback_t)pm_enter_cb;
    zbmac_power_manager_init((zbpm_callback_t)pm_exit_cb);
#endif
}

#if (TEST_ZB_PM_EN == 1)
void default_zbpm_enter(void)
{
    if (app_zbpm_enter)
    {
        app_zbpm_enter();
    }
}
#endif

static int p154_pm_set(uint32_t time)
{
#if (TEST_ZB_PM_EN == 1)
    return zbmac_power_manager_set(time, 0);
#else
    return FALSE;
#endif
}

static void p154_pm_init_wakeup(void)
{
    PMUnitStatus unit_status = power_manager_interface_get_unit_status(PM_SLAVE_ZIGBEE, PM_UNIT_ZIGBEE);

    if (unit_status == PM_UNIT_INACTIVE)
    {
        power_manager_master_initiate_wakeup(PM_SLAVE_ZIGBEE, (PMSystemLevel)(PM_UNIT_ZIGBEE >> 2),
                                             PM_UNIT_ZIGBEE);
        osif_sem_give(zb_sem);
    }
}

static uint32_t p154_get_curr_timestamp(void)
{
    return (uint32_t)mac_timestamp_get();
}

void p154_init(void)
{
    uint16_t addr;
    mac_panid_set(0x89);
    // load pre-cofigured short address
    if (load_cfg_shortaddr(&addr) == FTL_READ_SUCCESS)
    {
        dbg_printf("load_cfg_shortaddr successfully. 0x%02x\r\n", addr);
        mac_short_addr_set(addr);
    }
    mac_channel_set(14);
    //mac_cca_mode_set(MAC_CCA_CS);
    mac_cca_ed_threshold_set(60);
#if (TEST_PROTOTHREAD_EN == 1)
    // register receive packet handler
    NETSTACK_MAC.register_handler(&csl_handler, 1);
#else
    dbg_printf("error: protothread not enbale\r\n");
#endif
    // register protocol function table
    PROTOCOL.id = PROTOCOL_154;
    PROTOCOL.get_curr_timestamp = p154_get_curr_timestamp;
    PROTOCOL.timer_init = p154_timer_init;
    PROTOCOL.timer_stop = p154_timer_stop;
    PROTOCOL.timer_start = p154_timer_start;
    PROTOCOL.send = p154_send;
    PROTOCOL.start_periodic_tx_sched_cb = p154_csma_arq_disable;
    PROTOCOL.stop_periodic_tx_sched_cb = p154_csma_arq_enable;
    PROTOCOL.pm_init = p154_pm_init;
    PROTOCOL.pm_init_wakeup = p154_pm_init_wakeup;
    PROTOCOL.pm_set = p154_pm_set;
    PROTOCOL.config_init = p154_config_init;
    dbg_printf("Protocol IEEE 802.15.4 initialization done\r\n");
    dbg_printf("Use panid 0x%x, channel %u, address 0x%x\r\n", mac_panid_get(),
               mac_channel_get(), mac_short_addr_get());
}
