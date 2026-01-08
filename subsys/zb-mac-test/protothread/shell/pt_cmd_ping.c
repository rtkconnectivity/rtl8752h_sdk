/**************************************************************************//**
 * @file      pt_cmd_ping.c
 * @brief     Source file for ping command implementation
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

#include <osif.h>
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "protothread.h"
#include "net/pt_mac.h"
#include "shell/pt_cmd.h"
#include "mac_driver_interface.h"
#include "mac_test_common.h"
#include "mac_802154_frame_parser.h"

#define ECHO_REQUEST 0
#define ECHO_REPLY 1

static bool dump_rx = 0;

/* The Ping headers. */
typedef struct
{
    uint8_t type;
    uint8_t idx; // last byte of session pointer
    uint16_t seq;
} __attribute__((packed)) ping_hdr_t;

/* The Ping Payload. */
typedef struct
{
    int8_t rssi;
    uint8_t lqi;
    uint16_t rsvd;
} __attribute__((packed)) ping_payload_t;

/***ECHO_REQUEST********************************************************/
typedef struct
{
    uint32_t last_recv_seq : 17; // use for rx filter
    uint32_t resv : 15;
    uint32_t total_send;
    uint32_t total_recv;
    uint32_t start_time;
    uint32_t rtt_min;
    uint32_t rtt_max;
    uint32_t send_time;
    uint64_t rtt_avg;
} statistics;

typedef struct
{
    uint32_t cnt;
    uint32_t interval;
    uint32_t timeout;
    uint64_t daddr;
    uint8_t data_len;
    uint8_t pan_idx : 2;
    uint8_t verbose : 1;
    uint8_t rsvd : 5;
} ping_conf_t;

typedef struct ping_session
{
    struct ping_session_t *next;
    struct etimer timeout_timer; // must keep it at the top
    ping_conf_t conf;
    uint32_t seq; // current working seq
    uint32_t start_timestamp;
    statistics stats;
} ping_session_t;

#define MAX_PING_SESSION 2
MEMB(ping_session_memb, ping_session_t, MAX_PING_SESSION);
LIST(ping_session_list);

PROCESS(ping_process, "ping");
static process_event_t ping_new_event = 0;

/*static void session_print_internal(ping_session_t *session)
{
    if (session) {
        dbg_printf("conf.pan_idx %u\r\n", session->conf.pan_idx);
        dbg_printf("conf.daddr 0x%llx\r\n", session->conf.daddr);
        dbg_printf("conf.interval %u\r\n", session->conf.interval);
        dbg_printf("conf.timeout %u\r\n", session->conf.timeout);
        dbg_printf("conf.cnt %u\r\n", session->conf.cnt);
        dbg_printf("conf.data_len %u\r\n", session->conf.data_len);
        dbg_printf("conf.verbose %u\r\n", session->conf.verbose);
        dbg_printf("expired(timeout_timer) %u\r\n",etimer_expired(&(session->timeout_timer)));
    }
}

static void session_print(ping_session_t *session)
{
    if (session) {
        session_print_internal(session);
    } else {
        for(session = list_head(ping_session_list); session != NULL; session = list_item_next(session))
        {
            session_print_internal(session);
        }
    }
}*/

#define get_integer(numerator, denominator) ((numerator) / (denominator))
#define get_decimal(numerator, denominator) (((numerator) % (denominator)) * 100 / (denominator))

static void session_free(ping_session_t *session)
{
    if (session)
    {
        list_remove(ping_session_list, session);
        etimer_stop(&session->timeout_timer);

        clock_time_t now = clock_time();
        uint32_t err_count = session->stats.total_send - session->stats.total_recv;
        dbg_printf("--- [%p] 0x%llx ping statistics ---\r\n", session, session->conf.daddr);
        dbg_printf("%u packets transmitted, %u received, %u.%u%% packet loss, time %u ms\r\n",
                   session->stats.total_send, session->stats.total_recv,
                   get_integer(err_count * 100, session->stats.total_send),
                   get_decimal(err_count * 100, session->stats.total_send),
                   (now - session->stats.start_time));
        dbg_printf("rtt min/avg/max = %u/%llu.%llu/%u ms\r\n", session->stats.rtt_min,
                   get_integer(session->stats.rtt_avg, session->stats.total_recv),
                   get_decimal(session->stats.rtt_avg, session->stats.total_recv),
                   session->stats.rtt_max);
        memb_free(&ping_session_memb, session);
    }
}

static ping_session_t *session_alloc(void)
{
    ping_session_t *session = memb_alloc(&ping_session_memb);
    if (session)
    {
        memset(session, 0, sizeof(ping_session_t));
        session->stats.last_recv_seq = 65536;
        session->stats.rtt_min = 0xffffffff;
        list_add(ping_session_list, session);
    }
    return session;
}

static void ping_init()
{
    list_init(ping_session_list);
    memb_init(&ping_session_memb);
    if (ping_new_event == 0)
    {
        ping_new_event = process_alloc_event();
    }
}

static void ping_exit()
{
    ping_session_t *session = list_pop(ping_session_list);
    while (session)
    {
        session_free(session);
        session = list_pop(ping_session_list);
    }
}

static int request_send(ping_session_t *session, bool retry)
{
    uint16_t panid = mpan_GetPANId(session->conf.pan_idx);
    uint16_t src_addr_short;
    uint16_t dst_addr_short;
    uint8_t *src_addr;
    uint8_t *dst_addr;
    uint32_t now = clock_time();
    int ret = FALSE;

    if (retry == 0)   // send new one
    {
        session->start_timestamp = now;
        session->stats.total_send++;
        etimer_set(&session->timeout_timer, session->conf.timeout);
    }
    fc_t fc = {0};
    fc.type = FRAME_TYPE_COMMAND;
    fc.sec_en = 0;
    fc.ack_req = ((session->conf.daddr == 0xffff) ? 0 : 1);
    fc.ver = FRAME_VER_2006;

    if (session->conf.daddr > 0xffff)   // extend address
    {
        src_addr = mpan_GetLongAddress(session->conf.pan_idx);
        fc.src_addr_mode = ADDR_MODE_EXTEND;
        dst_addr = (uint8_t *)&session->conf.daddr;
        fc.dst_addr_mode = ADDR_MODE_EXTEND;
    }
    else     // short address
    {
        src_addr_short = mpan_GetShortAddress(session->conf.pan_idx);
        src_addr = (uint8_t *)&src_addr_short;
        fc.src_addr_mode = ADDR_MODE_SHORT;
        dst_addr_short = session->conf.daddr;
        dst_addr = (uint8_t *)&dst_addr_short;
        fc.dst_addr_mode = ADDR_MODE_SHORT;
    }

    g_tx_buf.len = generate_ieee_frame(fc.type, g_tx_buf.buf, 125, fc, session->stats.total_send,
                                       panid, src_addr, panid, dst_addr,
                                       NV_FIELD, NV_FIELD, MAC_CMD_PING, NV_FIELD);

    ping_hdr_t hdr =
    {
        .type = ECHO_REQUEST,
        .idx = (uint32_t)session,
        .seq = session->stats.total_send,
    };
    CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], &hdr, sizeof(ping_hdr_t), g_tx_buf.len);
    GEN_SEQ_DATA_MV_PTR(g_tx_buf.buf, 0, session->conf.data_len, g_tx_buf.len);

    pktgen_conf_t conf =
    {
        .daddr = session->conf.daddr,
        .cnt = 1,
        .interval = timer_remaining(&session->timeout_timer.timer), // ms
        .random = 0,
        .pan_idx = session->conf.pan_idx,
        .data_len = 0,
        .silent = 3,
        .defer_time = 0,
        .sec_level = 0,
        .key_id_mode = 0,
        .frm_type = 1,
        .frm_ver = 0,
        .cmd_id = 0,
        .user_raw = g_tx_buf.buf,
        .raw_len = g_tx_buf.len,
        .user_cb = NULL,
    };
    //dbg_printf("[%p] send %u idx %u remain %u\r\n", session, hdr.seq, hdr.idx, conf.interval);

    ret = pktgen(&conf);
    if (ret == FALSE)
    {
        process_post(&ping_process, PROCESS_EVENT_CONTINUE, (void *)session);
    }
    else
    {
        session->stats.send_time = now;
        //if (session->conf.daddr == 0xffff)
        //  dbg_printf("[%p] send %u bytes to 0x%llx: icmp_req=%u\r\n", session, session->conf.data_len,
        //      session->conf.daddr, session->stats.total_send);
    }
    return ret;
}

static void ping_timeout(ping_session_t *session)
{
    uint32_t now = clock_time();

    if (session->conf.interval == 0 ||
        (now - session->start_timestamp < session->conf.interval))   // wait reply timeout
    {
        if (session->conf.daddr != 0xffff)
        {
            dbg_printf("[%p] %u bytes from 0x%llx: icmp_req=%u timeout retry=%u\r\n", session,
                       session->conf.data_len,
                       session->conf.daddr, session->stats.total_send, mac_txn_retry_cnt_get());
        }

        if (session->conf.interval)
        {
            etimer_set(&session->timeout_timer, session->start_timestamp + session->conf.interval - now);
        }
        else
        {
            process_post(&ping_process, PROCESS_EVENT_CONTINUE, (void *)session);
        }
    }
    else     // send next
    {
        process_post(&ping_process, PROCESS_EVENT_CONTINUE, (void *)session);
    }
}

PROCESS_THREAD(ping_process, ev, data)
{
    ping_session_t *session = (ping_session_t *)data;

    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    ping_init();
    while (1)
    {
        PROCESS_WAIT_EVENT();
        //dbg_printf("ping ev %x\r\n", ev);
        if (ev == PROCESS_EVENT_EXIT)
        {
            ping_exit();
            PROCESS_EXIT();
        }
        else if (ev == ping_new_event)
        {
            dbg_printf("[%p] PING 0x%llx size = %u count = %u timeout = %u(ms) interval = %u(ms)\r\n",
                       session, session->conf.daddr, session->conf.data_len, session->conf.cnt,
                       session->conf.timeout, session->conf.interval);
            session->stats.start_time = clock_time();
            request_send(session, 0);
        }
        else if (ev == PROCESS_EVENT_TIMER && etimer_expired((struct etimer *)data))
        {
            // data is the address of etimer, etimer is the second element of the session
            session = (ping_session_t *)(((uint8_t *)data) - sizeof(void *));
            ping_timeout(session);
        }
        else if (ev == PROCESS_EVENT_CONTINUE)
        {
            //dbg_printf("etimer_expired %u\r\n", etimer_expired(&(session->timeout_timer)));
            if (!etimer_expired(&(session->timeout_timer)))
            {
                if (timer_remaining(&session->timeout_timer.timer) > 3)
                {
                    request_send(session, 1);
                }
            }
            else     // expired
            {
                if (session->conf.cnt && session->stats.total_send >= session->conf.cnt)
                {
                    session_free(session);
                }
                else
                {
                    request_send(session, 0);
                }
            }
        }
    }
    PROCESS_END();
}

static int cmd_ping(int argc, char *argv[])
{
    int8_t key = -1, expr;
    uint32_t value = 0;

    ping_conf_t conf =
    {
        .cnt = 3,
        .interval = 0, // ms
        .timeout = 1000, // ms
        .pan_idx = 0,
        .data_len = 10,
        .verbose = 0,
    };

    if (argc <= 0)
    {
        goto error;
    }
    else
    {
        if (!strcmp(argv[0], "dumprx"))
        {
            dump_rx = 1;
            return TRUE;
        }
        else if (!strcmp(argv[0], "nodumprx"))
        {
            dump_rx = 0;
            return TRUE;
        }

        conf.daddr = _strtoull((const char *)(argv[0]), (char **)NULL, 16);

        for (int i = 1; i < argc; i++)
        {
            if (key == 's' || key == 'c' || key == 'i' || key == 'p' ||
                key == 't')
            {
                value = _strtoul((const char *)(argv[i]), (char **)NULL, 10);
                expr = key;
            }
            else
            {
                expr = argv[i][0];
                key = -1;
            }
            switch (expr)
            {
            case 's':
                if (key == 's')
                {
                    if (value > 111)
                    {
                        goto error;
                    }
                    conf.data_len = value;
                    key = -1;
                }
                else
                {
                    key = 's';
                }
                break;
            case 'c':
                if (key == 'c')
                {
                    conf.cnt = value;
                    key = -1;
                }
                else
                {
                    key = 'c';
                }
                break;
            case 'i':
                if (key == 'i')
                {
                    conf.interval = value;
                    key = -1;
                }
                else
                {
                    key = 'i';
                }
                break;
            case 'p':
                if (key == 'p')
                {
                    if (value > 3)
                    {
                        goto error;
                    }
                    conf.pan_idx = value;
                    key = -1;
                }
                else
                {
                    key = 'p';
                }
                break;
            case 't':
                if (key == 't')
                {
                    conf.timeout = value;
                    if (conf.timeout == 0 || conf.timeout < 5)
                    {
                        goto error;
                    }
                    key = -1;
                }
                else
                {
                    key = 't';
                }
                break;
            case 'v':
                conf.verbose = 1;
                break;
            default:
                i = argc;
                key = '_';
                break;
            }
        }
    }

    if (key != -1 || (conf.interval && conf.interval <= conf.timeout))
    {
        goto error;
    }

    if (!process_is_running(&ping_process))
    {
        process_start(&ping_process, NULL);
    }

    ping_session_t *session = session_alloc();
    if (session == NULL)
    {
        dbg_printf("Error: Session Full\r\n");
        return FALSE;
    }
    mac_memcpy(&session->conf, &conf, sizeof(ping_conf_t));
    process_post_synch(&ping_process, ping_new_event, session);
    //dbg_printf("Done\r\n");
    return TRUE;
error:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}

void shell_register_cmd_ping(void)
{
    //process_start(&ping_process, NULL);
    ping_process.state = 0;
    ping_new_event = 0;
    shell_register_pt((shell_program_t)cmd_ping, "ping",
                      BRIEF("ping")
                      SYNOPSIS("ping <shortaddr/extaddr>")
                      DESCRIPTION("	shortaddr: 0x0000 ~ 0xffff")
                      DESCRIPTION("	extaddr: 0x0000000000010000 ~ 0xffffffffffffffff")
                      DESCRIPTION("c <loop_cnt>")
                      DESCRIPTION("	0 ~ 4294967295")
                      DESCRIPTION("	0 - infinite loop")
                      DESCRIPTION("	1 (default)")
                      DESCRIPTION("i <interval>")
                      DESCRIPTION("	0 ~ 4294967295 ms")
                      DESCRIPTION("	0 (default)")
                      DESCRIPTION("	If an interval is set, it must be greater than the timeout value.")
                      DESCRIPTION("p <pan_index>")
                      DESCRIPTION("	0 ~ 3")
                      DESCRIPTION("	0 (default)")
                      DESCRIPTION("s <size>")
                      DESCRIPTION("	0 ~ 111")
                      DESCRIPTION("	10 (default)")
                      DESCRIPTION("t <timeout>")
                      DESCRIPTION("	5 ~ 4294967295 ms")
                      DESCRIPTION("	1000 (default)")
                      DESCRIPTION("v	verbose")
                      EXAMPLE("ping 0x1 c 10")
                      DESCRIPTION("	send 10 request packets to short address 0x1")
                      EXAMPLE("ping 0x10000 e 40 v")
                      DESCRIPTION("	send 802.15.4-2015 secure data packet to extend address 0x10000")
                     );
}

/***ECHO_REPLY********************************************************/
typedef struct reply_buf
{
    struct reply_buf *next;
    buf_t buf;
    txl_ctrl_info_t info;
} reply_buf_t;

#define MAX_REPLY_BUF 8

PROCESS(ping_reply_process, "ping_reply");
MEMB(ping_reply_memb, reply_buf_t, MAX_REPLY_BUF);
LIST(ping_reply_list);

static int create_reply(buf_t *tx_buf, uint8_t pan_idx, bool addr_ext,
                        uint8_t *dst_addr, ping_hdr_t *hdr, uint8_t payload_len, int8_t rssi, uint8_t lqi)
{
    uint16_t panid = mpan_GetPANId(pan_idx);
    uint16_t src_addr_short;
    uint8_t *src_addr;
    fc_t fc = {0};
    fc.type = FRAME_TYPE_COMMAND;
    fc.sec_en = 0;
    fc.ack_req = 1;
    fc.ver = FRAME_VER_2006;

    if (addr_ext)   // extend address
    {
        src_addr = mpan_GetLongAddress(pan_idx);
        fc.src_addr_mode = ADDR_MODE_EXTEND;
        fc.dst_addr_mode = ADDR_MODE_EXTEND;
    }
    else     // short address
    {
        src_addr_short = mpan_GetShortAddress(pan_idx);
        src_addr = (uint8_t *)&src_addr_short;
        fc.src_addr_mode = ADDR_MODE_SHORT;
        fc.dst_addr_mode = ADDR_MODE_SHORT;
    }

    tx_buf->len = generate_ieee_frame(fc.type, tx_buf->buf, 125, fc, 0,
                                      panid, src_addr, panid, dst_addr,
                                      NV_FIELD, NV_FIELD, MAC_CMD_PING, NV_FIELD);
    hdr->type = ECHO_REPLY;
    CPY_MV_PTR(&tx_buf->buf[tx_buf->len], hdr, sizeof(ping_hdr_t), tx_buf->len);

    if (payload_len >= sizeof(ping_payload_t))
    {
        ping_payload_t payload = { .rssi = rssi, .lqi = lqi, .rsvd = 0 };
        CPY_MV_PTR(&tx_buf->buf[tx_buf->len], &payload, sizeof(ping_payload_t), tx_buf->len);
        payload_len -= sizeof(ping_payload_t);
    }
    GEN_SEQ_DATA_MV_PTR(tx_buf->buf, 0, payload_len, tx_buf->len);

    return TRUE;
}

static void ping_input(uint8_t *rx_data, uint8_t pan_idx)
{
    uint8_t *buf = MAC_RX_PKT(rx_data);
    uint8_t buf_len = MAC_RX_PKT_LEN(rx_data);
    uint8_t lqi = MAC_RX_PKT_LQI(rx_data);
    int8_t rssi = MAC_RX_PKT_RSSI(rx_data);
    bool is_src_addr_extended, is_dst_addr_extended;
    uint16_t src_addr_short = 0;
    uint16_t dst_addr_short = 0;
    uint64_t src_addr_ext = 0;
    uint64_t dst_addr_ext = 0;
    const uint8_t *src_addr = mac_802154_frame_parser_src_addr_get(rx_data, &is_src_addr_extended);
    const uint8_t *dst_addr = mac_802154_frame_parser_dst_addr_get(rx_data, &is_dst_addr_extended);
    uint16_t panid;
    const uint8_t *_panid = mac_802154_frame_parser_dst_panid_get(rx_data);
    mac_memcpy(&panid, _panid, 2);
    if (is_src_addr_extended == TRUE)
    {
        mac_memcpy(&src_addr_ext, src_addr, 8);
    }
    else
    {
        mac_memcpy(&src_addr_short, src_addr, 2);
    }
    if (is_dst_addr_extended == TRUE)
    {
        mac_memcpy(&dst_addr_ext, dst_addr, 8);
    }
    else
    {
        mac_memcpy(&dst_addr_short, dst_addr, 2);
    }

    uint8_t offset = mac_802154_frame_parser_addressing_end_offset_get(rx_data);
    offset++; // shift one byte command id
    ping_hdr_t *ping_hdr = (ping_hdr_t *)&rx_data[offset];
    uint16_t seq;
    mac_memcpy(&seq, &ping_hdr->seq, sizeof(seq));
    uint8_t payload_len = (uint32_t)buf + buf_len - (uint32_t)ping_hdr - sizeof(ping_hdr_t);

    if (dump_rx)
        dbg_printf("Received %s from 0x%llx to 0x%llx icmp_req=%u data_len=%u lqi=%u rssi=%d pan_idx=%u\r\n",
                   (ping_hdr->type == ECHO_REQUEST) ? "Echo Request" : "Echo Reply",
                   (is_src_addr_extended == TRUE) ? src_addr_ext : (uint64_t)src_addr_short,
                   (is_dst_addr_extended == TRUE) ? dst_addr_ext : (uint64_t)dst_addr_short,
                   seq, payload_len, lqi, rssi, pan_idx);
    // Input check
    uint16_t my_panid = mpan_GetPANId(pan_idx);
    uint16_t my_addr_short = mpan_GetShortAddress(pan_idx);
    uint8_t *my_addr_ext = mpan_GetLongAddress(pan_idx);

    if (panid != my_panid)
    {
        dbg_printf("ping drop: panid mismatch. panid[%u]=0x%04x != 0x%04x\r\n", pan_idx, my_panid, panid);
        return;
    }
    if (is_dst_addr_extended == TRUE)
    {
        uint64_t tmp_ext;
        mac_memcpy(&tmp_ext, my_addr_ext, 8);

        if (memcmp(my_addr_ext, &dst_addr_ext, 8))
        {
            dbg_printf("ping drop: extaddr mismatch. extaddr[%u]=0x%016llx != 0x%016llx\r\n",
                       pan_idx, tmp_ext, dst_addr_ext);
            return;
        }
    }
    else
    {
        if (dst_addr_short != 0xffff && my_addr_short != dst_addr_short)
        {
            dbg_printf("ping drop: shortaddr mismatch. shortaddr[%u]=0x%04x != 0x%04x\r\n",
                       pan_idx, my_addr_short, dst_addr_short);
            return;
        }
    }
    if (ping_hdr->type == ECHO_REQUEST)
    {
        // create ping reply
        reply_buf_t *reply = memb_alloc(&ping_reply_memb);
        if (reply)
        {
            memset(reply, 0, sizeof(reply_buf_t));
#if TEST_MPAN_EN
            reply->info.mpan_enabled = 1;
            reply->info.mpan_idx = pan_idx;
#else
            reply->info.mpan_enabled = 0;
            reply->info.mpan_idx = pan_idx;
#endif
            create_reply(&reply->buf, pan_idx, is_src_addr_extended,
                         (uint8_t *)src_addr, ping_hdr, payload_len, rssi, lqi);
            list_add(ping_reply_list, reply);
            process_poll(&ping_reply_process);
        }
        else
        {
            dbg_printf("ping drop: memb_alloc fail\r\n");
        }
        return;
    }
    else if (ping_hdr->type == ECHO_REPLY)
    {
        ping_session_t *session;
        uint32_t now = clock_time();

        for (session = list_head(ping_session_list); session != NULL; session = list_item_next(session))
        {
            if (((uint32_t)session & 0xff) == ping_hdr->idx)
            {
                break;
            }
        }

        if (session)
        {
            // rx filter: drop if seq is the same as last received packet
            if (seq == session->stats.last_recv_seq)
            {
                if (session->conf.verbose)
                {
                    dbg_printf("[%p] %u bytes from 0x%llx: icmp_req=%u duplicated drop\r\n", session, payload_len,
                               (is_src_addr_extended == TRUE) ? src_addr_ext : (uint64_t)src_addr_short, seq);
                }
                return;
            }

            session->stats.last_recv_seq = seq;
            if (seq != (uint16_t)session->stats.total_send)
            {
                if (session->conf.verbose)
                {
                    dbg_printf("[%p] %u bytes from 0x%llx: icmp_req=%u is not correct\r\n", session, payload_len,
                               (is_src_addr_extended == TRUE) ? src_addr_ext : (uint64_t)src_addr_short, seq);
                }
                return;
            }
            session->stats.total_recv++;
            uint32_t rtt = now - session->stats.send_time;
            if (rtt < session->stats.rtt_min)
            {
                session->stats.rtt_min = rtt;
            }
            if (rtt > session->stats.rtt_max)
            {
                session->stats.rtt_max = rtt;
                //dbg_printf("rtt_max = %u\r\n", session->stats.rtt_max);
            }
            // Calculate average rtt
            session->stats.rtt_avg += rtt;

            ping_payload_t *payload = NULL;
            if (payload_len >= sizeof(ping_payload_t))
            {
                payload = (ping_payload_t *)((uint8_t *)ping_hdr + sizeof(ping_hdr_t));
            }

            dbg_printf("[%p] %u bytes from 0x%llx: icmp_req=%u time=%u ms rssi=%d lqi=%u r_rssi=%d r_lqi=%u\r\n",
                       session, payload_len,
                       (is_src_addr_extended == TRUE) ? src_addr_ext : (uint64_t)src_addr_short, session->stats.total_send,
                       rtt, rssi, lqi, (payload) ? payload->rssi : 0, (payload) ? payload->lqi : 0);
            if (session->conf.verbose)
            {
                dbg_printf("    retry=%u\r\n", mac_txn_retry_cnt_get());
            }
            if (session->conf.daddr != 0xffff)
            {
                etimer_stop(&session->timeout_timer);
                if (session->conf.interval && (now - session->start_timestamp) < session->conf.interval)
                {
                    PROCESS_CONTEXT_BEGIN(&ping_process);
                    etimer_set(&session->timeout_timer, session->start_timestamp + session->conf.interval - now);
                    PROCESS_CONTEXT_END(&ping_process);
                }
                else
                {
                    process_post(&ping_process, PROCESS_EVENT_CONTINUE, (void *)session);
                }
            }
        }
        else
        {
            dbg_printf("ping drop: session mismatch %X\r\n", ping_hdr->idx);
        }
    }
}

MAC_INPUT_HANDLER(ping_handler0, 0, MAC_CMD_PING, ping_input);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
MAC_INPUT_HANDLER(ping_handler1, 1, MAC_CMD_PING, ping_input);
#endif
#if (TEST_MAX_PAN_NUM > 2)
MAC_INPUT_HANDLER(ping_handler2, 2, MAC_CMD_PING, ping_input);
#endif
#if (TEST_MAX_PAN_NUM > 3)
MAC_INPUT_HANDLER(ping_handler3, 3, MAC_CMD_PING, ping_input);
#endif
#endif
static void ping_reply_exit()
{
    reply_buf_t *reply = list_pop(ping_reply_list);
    while (reply)
    {
        memb_free(&ping_reply_memb, reply);
        reply = list_pop(ping_reply_list);
    }

    NETSTACK_MAC.register_handler(&ping_handler0, 0);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
    NETSTACK_MAC.register_handler(&ping_handler1, 0);
#endif
#if (TEST_MAX_PAN_NUM > 2)
    NETSTACK_MAC.register_handler(&ping_handler2, 0);
#endif
#if (TEST_MAX_PAN_NUM > 3)
    NETSTACK_MAC.register_handler(&ping_handler3, 0);
#endif
#endif
    dbg_printf("%s exit\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
}

PROCESS_THREAD(ping_reply_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    list_init(ping_reply_list);
    memb_init(&ping_reply_memb);
    NETSTACK_MAC.register_handler(&ping_handler0, 1);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
    NETSTACK_MAC.register_handler(&ping_handler1, 1);
#endif
#if (TEST_MAX_PAN_NUM > 2)
    NETSTACK_MAC.register_handler(&ping_handler2, 1);
#endif
#if (TEST_MAX_PAN_NUM > 3)
    NETSTACK_MAC.register_handler(&ping_handler3, 1);
#endif
#endif

    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            ping_reply_exit();
            PROCESS_EXIT();
        }
        else if (ev == PROCESS_EVENT_POLL)
        {
            reply_buf_t *reply = list_head(ping_reply_list);
            if (reply)
            {
                BUF_RESET(g_tx_buf);
                CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], reply->buf.buf, reply->buf.len, g_tx_buf.len);

                int ret = NETSTACK_MAC.send(NULL, NULL, &reply->info);
                if (ret == PT_MAC_TX_OK)
                {
                    reply = list_pop(ping_reply_list);
                    memb_free(&ping_reply_memb, reply);
                    reply = list_head(ping_reply_list);
                }
            }
            if (reply)
            {
                process_poll(&ping_reply_process);
            }
        }
    }
    PROCESS_END();
}
