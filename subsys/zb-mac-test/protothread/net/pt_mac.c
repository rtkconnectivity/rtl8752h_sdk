/**************************************************************************//**
 * @file      pt_mac.c
 * @brief     Source file for protothread mac function implementation
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
#include "mac_driver_interface.h"
#include "mac_stats.h"
#include "pt_mac.h"
#include "lib/ringbufindex.h"
#include "lib/list.h"
#include "mac_802154_frame_parser.h"

static int mac_state;
bool ptmac_dump_rx;
bool ptmac_dump_tx;

static struct
{
    pt_mac_callback_t callback;
    void *ptr;
    uint32_t trig_timestamp;
    uint32_t tx_timestamp;
    txl_ctrl_info_t ctrl_info;
    uint8_t sw_retry_cnt;
} report;

void shell_register_cmd_ptmac(void);

PROCESS(mac_tx_process, "mac_tx");
PROCESS_THREAD(mac_tx_process, ev, data)
{
    int status = PT_MAC_TX_ERR_FATAL;
    uint8_t tx_cnt = 0;

    PROCESS_BEGIN();
    report.ctrl_info.prt_tx_mask = 0;
    report.ctrl_info.manual_tx_lock = 1;
    if (ptmac_dump_tx)
    {
        report.ctrl_info.dump_pkt = 1;
        report.ctrl_info.dump_enc_pkt = 1;
    }
    else
    {
        report.ctrl_info.dump_pkt = 0;
        report.ctrl_info.dump_enc_pkt = 0;
    }
sw_retry:
    //dbg_mem_dump((const uint8_t *)&report.ctrl_info, sizeof(txl_ctrl_info_t));
#if TEST_MPAN_EN
    mpan_mac_lock(report.ctrl_info.mpan_idx);
#else
    mpan_mac_lock(0);
#endif
    if (txl_exec(&report.trig_timestamp, 0, (uint32_t)&report.ctrl_info) != 0)
    {
        g_tx_done = TX_TERMED + 1;
        process_post(&mac_tx_process, PROCESS_EVENT_POLL, NULL);
    }
    report.tx_timestamp = report.trig_timestamp;
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
    mpan_mac_unlock();
    switch (g_tx_done)
    {
    case TX_SUCCESS:
        report.tx_timestamp = mac_txn_timestamp_get();
        status = PT_MAC_TX_OK;
        tx_cnt = 1 + mac_txn_retry_cnt_get();
        break;
    case TX_BUSY:
        status = PT_MAC_TX_COLLISION;
        break;
    case TX_NOACK:
        status = PT_MAC_TX_NOACK;
        tx_cnt = 1 + mac_txn_retry_cnt_get();
        break;
    case TX_AT_FAIL:
        status = PT_MAC_TX_NOTONTIME;
        break;
    case TX_TERMED:
        {
            if (report.ctrl_info.tx_mode == TX_MODE_NORMAL &&
                report.sw_retry_cnt < mac_txn_retry_get())
            {
                report.sw_retry_cnt++;
                goto sw_retry;
            }
            tx_cnt = 1 + report.sw_retry_cnt;
            status = PT_MAC_TX_ERR;
            break;
        }
    default:
        status = PT_MAC_TX_ERR_FATAL;
        break;
    }
    if (report.callback)
    {
        report.callback(report.ptr, status, tx_cnt, report.trig_timestamp, report.tx_timestamp);
        report.callback = NULL;
    }
    PROCESS_END();
}

/**********************MAC RX***************************/
struct ringbufindex pt_mac_rx_ring;
rx_item_t pt_mac_rx_buf[PT_MAC_RX_BUF_MAX];

LIST(pt_mac_input_handler_list);

static mac_input_handler_t *
mac_input_handler_lookup(uint8_t pan_idx, uint8_t type)
{
    mac_input_handler_t *handler;
    mac_input_handler_t *input_handler = NULL;

    for (handler = list_head(pt_mac_input_handler_list);
         handler != NULL;
         handler = list_item_next(handler))
    {
        if (handler->pan_idx == pan_idx)
        {
            if (handler->type == MAC_CMD_ANY)
            {
                input_handler = handler;
            }
            else if (handler->type == type)
            {
                input_handler = handler;
                break;
            }
        }
    }
    return input_handler;
}

static void
dump_rx_packet(uint8_t *rx_data, uint8_t pan_idx)
{
    uint8_t *buf = MAC_RX_PKT(rx_data);
    uint8_t buf_len = MAC_RX_PKT_LEN(rx_data);
    uint8_t lqi = MAC_RX_PKT_LQI(rx_data);
    int8_t rssi = MAC_RX_PKT_RSSI(rx_data);

    dbg_printf("MAC_RX: ");
    for (uint32_t i = 0; i < buf_len; i++)
    {
        dbg_printf("%02x ", buf[i]);
    }
    dbg_printf("LEN %u LQI %u RSSI %d PAN_IDX %u\r\n", buf_len, lqi, rssi, pan_idx);
}

PROCESS(mac_rx_process, "mac_rx");
PROCESS_THREAD(mac_rx_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    ringbufindex_init(&pt_mac_rx_ring, PT_MAC_RX_BUF_MAX); // radio to mac
    while (1)
    {
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL);
        int rx_index = ringbufindex_peek_get(&pt_mac_rx_ring);
        while (rx_index != -1)
        {
            rx_item_t *pkt = &pt_mac_rx_buf[rx_index];
            mac_stats_pktsig_update(MAC_RX_PKT_RSSI(pkt->raw), MAC_RX_PKT_LQI(pkt->raw));
            uint8_t type = MAC_CMD_ANY;

            if (ptmac_dump_rx)
            {
                dump_rx_packet(pkt->raw, pkt->pan_idx);
            }

            if (((fc_t *)&pkt->raw[1])->type == FRAME_TYPE_COMMAND)
            {
                uint8_t offset = mac_802154_frame_parser_addressing_end_offset_get(pkt->raw);
                type = pkt->raw[offset];
            }

            mac_input_handler_t *input = mac_input_handler_lookup(pkt->pan_idx, type);
            if (input && input->handler)
            {
                input->handler(pkt->raw, pkt->pan_idx);
            }
            /* Remove input from ringbuf */
            ringbufindex_get(&pt_mac_rx_ring);
            rx_index = ringbufindex_peek_get(&pt_mac_rx_ring);
        }
    }
    PROCESS_END();
}

static int
send_packet(pt_mac_callback_t sent, void *ptr, void *ctrl_info)
{
    if (mac_state != PT_MAC_ON || ctrl_info == NULL)
    {
        //dbg_printf("err: mac_state = %d\r\n", mac_state);
        return PT_MAC_TX_ERR;
    }
    if (process_is_running(&mac_tx_process))
    {
        //dbg_printf("%s is running\r\n", PROCESS_NAME_STRING(&cmd_kill_process));
        return PT_MAC_TX_ERR;
    }
    report.callback = sent;
    report.ptr = ptr;
    mac_memcpy(&report.ctrl_info, ctrl_info, sizeof(txl_ctrl_info_t));
    report.sw_retry_cnt = 0;
    process_start(&mac_tx_process, NULL);
    return PT_MAC_TX_OK;
}

static void
packet_sent(void)
{
    if (process_is_running(&mac_tx_process))
    {
        process_poll(&mac_tx_process);
    }
}

static void
packet_input(void)
{
    // called from rxdone_handler ISR
    if (process_is_running(&mac_rx_process) && mac_state == PT_MAC_ON)
    {
        process_poll(&mac_rx_process);
    }
}

static void
register_input_handler(mac_input_handler_t *handler, bool add)
{
    if (add)
    {
        list_add(pt_mac_input_handler_list, handler);
    }
    else
    {
        list_remove(pt_mac_input_handler_list, handler);
    }
}

static int
on(void)
{
    dbg_printf("Turn on %s\r\n", ieee802154_driver.name);
    mac_state = PT_MAC_ON;
    packet_input();
    return 0;
}

static int
off(void)
{
    dbg_printf("Turn off %s\r\n", ieee802154_driver.name);
    mac_state = PT_MAC_OFF;
    return 0;
}

static int
max_payload(void)
{
    return 127;
}

static void
init(void)
{
    mac_state = PT_MAC_OFF;
    ptmac_dump_rx = 0;
    ptmac_dump_tx = 0;
    memset(&report, 0, sizeof(report));
    list_init(pt_mac_input_handler_list);
    mac_tx_process.state = 0;
    mac_rx_process.state = 0;
    ieee802154_driver.on();
    process_start(&mac_rx_process, NULL);
    shell_register_cmd_ptmac();
}

const struct mac_driver ieee802154_driver =
{
    "IEEE802.15.4 Driver",
    init,
    send_packet,
    packet_sent, // notified by MAC TX ISR
    packet_input, // notified by MAC RX ISR
    register_input_handler, // notify upper layer
    on,
    off,
    max_payload,
};

static int cmd_ptmac(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        if (!strcmp(argv[0], "on"))
        {
            ieee802154_driver.on();
        }
        else if (!strcmp(argv[0], "off"))
        {
            ieee802154_driver.off();
        }
        else if (!strcmp(argv[0], "dumprx"))
        {
            ptmac_dump_rx = 1;
        }
        else if (!strcmp(argv[0], "nodumprx"))
        {
            ptmac_dump_rx = 0;
        }
        else if (!strcmp(argv[0], "dumptx"))
        {
            ptmac_dump_tx = 1;
        }
        else if (!strcmp(argv[0], "nodumptx"))
        {
            ptmac_dump_tx = 0;
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_cmd_ptmac(void)
{
    shell_register_pt((shell_program_t)cmd_ptmac, "ptmac",
                      BRIEF("protothread mac command")
                      SYNOPSIS("ptmac <on/off/dumprx/nodumprx/dumptx/nodumptx>"));
}
