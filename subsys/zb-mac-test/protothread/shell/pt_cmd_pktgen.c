/**************************************************************************//**
 * @file      pt_cmd_pktgen.c
 * @brief     Source file for pktgen command implementation
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

#include <stdlib.h>
#include "protothread.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "net/pt_mac.h"
#include <osif.h>
#include "mac_driver_interface.h"
#include "mac_test_common.h"
#include "mac_802154_frame_parser.h"
#include "shell/pt_cmd.h"
#include "services/auto_test.h"

#define MAX_PKTGEN_SESSION 4
#define GIVE_UP_EVENT (PT_MAC_TX_MAX_REASON + 1)

typedef struct pktgen_session
{
    struct pktgen_session *next;
    struct etimer interval_timer; // must keep it at the top
    pktgen_conf_t conf;
    uint32_t seq; // current working seq
    uint32_t mac_seq; // seq requested to mac
    uint32_t start_timestamp;
    uint32_t near_start; // use for random send mode
    uint8_t pending_retry : 1; // need retry or not
    uint8_t retry_ev : 1; // retry event in queue
    uint8_t pending_free : 1; // free session later
    uint8_t wait_mac : 1; // waiting mac reply
    uint8_t reserved : 4;
} pktgen_session_t;

MEMB(pktgen_session_memb, pktgen_session_t, MAX_PKTGEN_SESSION);
LIST(pktgen_session_list);
PROCESS(pktgen_process, "pktgen");
static process_event_t pktgen_new_event = 0;

static void session_print_internal(pktgen_session_t *session)
{
    if (session)
    {
        dbg_printf("conf.pan_idx %u\r\n", session->conf.pan_idx);
        dbg_printf("conf.daddr 0x%llx\r\n", session->conf.daddr);
        dbg_printf("conf.interval %u\r\n", session->conf.interval);
        dbg_printf("conf.random %u\r\n", session->conf.random);
        dbg_printf("conf.cnt %u\r\n", session->conf.cnt);
        dbg_printf("conf.data_len %u\r\n", session->conf.data_len);
        dbg_printf("conf.defer_time %u\r\n", session->conf.defer_time);
        dbg_printf("conf.silent %u\r\n", session->conf.silent);
        dbg_printf("conf.sec_level %u\r\n", session->conf.sec_level);
        dbg_printf("conf.key_id_mode %u\r\n", session->conf.key_id_mode);
        dbg_printf("conf.frm_type %u\r\n", session->conf.frm_type);
        dbg_printf("conf.frm_ver %u\r\n", session->conf.frm_ver);
        dbg_printf("conf.cmd_id %u\r\n", session->conf.cmd_id);
        dbg_printf("conf.user_raw %p len %u\r\n", session->conf.user_raw, session->conf.raw_len);
        dbg_printf("expired(interval_timer) %u\r\n", etimer_expired(&(session->interval_timer)));

        dbg_printf("seq %u\r\n", session->seq);
        dbg_printf("mac_seq %u\r\n", session->mac_seq);
        dbg_printf("pending_retry %u\r\n", session->pending_retry);
        dbg_printf("retry_ev %u\r\n", session->retry_ev);
        dbg_printf("pending_free %u\r\n", session->pending_free);
        dbg_printf("wait_mac %u\r\n", session->wait_mac);
    }
}

static void session_print(pktgen_session_t *session)
{
    if (session)
    {
        session_print_internal(session);
    }
    else
    {
        for (session = list_head(pktgen_session_list); session != NULL; session = list_item_next(session))
        {
            session_print_internal(session);
        }
    }
}

static void session_free(pktgen_session_t *session)
{
    if (session)
    {
        if (session->conf.silent <= 2)
        {
            dbg_printf("[%u] close pktgen session [%p]\r\n", clock_time(), session);
        }
        if (session->conf.silent == 0)
        {
            session_print_internal(session);
        }
        list_remove(pktgen_session_list, session);
        etimer_stop(&session->interval_timer);
        if (session->conf.user_raw)
        {
            osif_mem_free(session->conf.user_raw);
            session->conf.user_raw = NULL;
        }
        memb_free(&pktgen_session_memb, session);
    }
}

static pktgen_session_t *session_alloc(void)
{
    pktgen_session_t *session = memb_alloc(&pktgen_session_memb);
    if (session)
    {
        memset(session, 0, sizeof(pktgen_session_t));
        list_add(pktgen_session_list, session);
    }
    return session;
}

static void pktgen_init()
{
    list_init(pktgen_session_list);
    memb_init(&pktgen_session_memb);
    if (pktgen_new_event == 0)
    {
        pktgen_new_event = process_alloc_event();
    }
}

static void pktgen_exit()
{
    pktgen_session_t *session = list_pop(pktgen_session_list);
    while (session)
    {
        session_free(session);
        session = list_pop(pktgen_session_list);
    }
}

static void packet_sent(void *ptr, int status, int transmissions, uint32_t trig_ts, uint32_t tx_ts)
{
    pktgen_session_t *session = (pktgen_session_t *)ptr;
    uint32_t tmp_seq;
    uint32_t timestamp = 0;

    struct
    {
        uint64_t trig_ts : 32;
        uint64_t tx_ts : 32;
    } report = { .trig_ts = trig_ts, .tx_ts = tx_ts};

    if (status == GIVE_UP_EVENT)
    {
        tmp_seq = session->seq;
    }
    else
    {
        tmp_seq = session->mac_seq;
        if (session->wait_mac)
        {
            session->wait_mac = 0;
            timestamp = clock_time() - session->start_timestamp;
        }
        else
        {
            if (session->conf.silent <= 2)
            {
                dbg_printf("fatal error [%p]\r\n", session);
            }
        }
    }

    if (session->conf.user_cb)
    {
        session->conf.user_cb(ptr, status, transmissions, report.trig_ts, report.tx_ts);
    }
    else
    {
        if (session->conf.silent <= 1)
        {
            dbg_printf("[%p] send %u, status %d, transmissions %d, time %u", session,
                       tmp_seq, status, transmissions, timestamp);
            if (status == PT_MAC_TX_OK && session->conf.defer_time)
            {
                dbg_printf(", txat diff %u\r\n", report.tx_ts - report.trig_ts);
            }
            else
            {
                dbg_printf("\r\n");
            }
        }
        at_send_first(sizeof(report), &report);
        at_send_more(sizeof(status), &status);
        at_send_last(sizeof(transmissions), &transmissions);
    }

    if (session->conf.interval == 0 || (session->conf.cnt && session->seq == session->conf.cnt - 1))
    {
        process_post(&pktgen_process, PROCESS_EVENT_MSG, (void *)session);
    }
}

static int pktgen_send(pktgen_session_t *session)
{
    uint16_t spanid = mpan_GetPANId(session->conf.pan_idx);
    uint16_t dpanid = spanid;
    uint16_t src_addr_short;
    uint16_t dst_addr_short;
    uint8_t *src_addr;
    uint8_t *dst_addr;
    int ret;
    txl_ctrl_info_t info = {0};
#if TEST_MPAN_EN
    info.mpan_enabled = 1;
    info.mpan_idx = session->conf.pan_idx;
#else
    info.mpan_enabled = 0;
    info.mpan_idx = 0;
#endif

    if (session->conf.user_raw)
    {
        BUF_RESET(g_tx_buf);
        CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], session->conf.user_raw, session->conf.raw_len,
                   g_tx_buf.len);
    }
    else
    {
        fc_t fc = {0};
        fc.type = session->conf.frm_type;
        fc.sec_en = (session->conf.sec_level ? 1 : 0);
        fc.ack_req = ((session->conf.daddr == 0xffff) ? 0 : 1);
        fc.ver = (session->conf.frm_ver ? FRAME_VER_2015 : FRAME_VER_2006);

        if (session->conf.daddr > 0xffff)   // extend address
        {
            src_addr = mpan_GetLongAddress(info.mpan_idx);
            fc.src_addr_mode = ADDR_MODE_EXTEND;
            dst_addr = (uint8_t *)&session->conf.daddr;
            fc.dst_addr_mode = ADDR_MODE_EXTEND;
            if (fc.ver == FRAME_VER_2015)
            {
                spanid = 0;
            }
        }
        else     // short address
        {
            src_addr_short = mpan_GetShortAddress(info.mpan_idx);
            src_addr = (uint8_t *)&src_addr_short;
            fc.src_addr_mode = ADDR_MODE_SHORT;
            dst_addr_short = session->conf.daddr;
            dst_addr = (uint8_t *)&dst_addr_short;
            fc.dst_addr_mode = ADDR_MODE_SHORT;
        }

        if (session->conf.frm_type == FRAME_TYPE_BEACON && session->conf.frm_ver == 0)
        {
            fc.dst_addr_mode = ADDR_MODE_NOT_PRESENT;
            fc.ack_req = 0;
        }

        ss_t ss = { .bo = 5,
                    .so = 5,
                    .final_cap_slot = 15,
                    .ble = 0,
                    .rsv = 0,
                    .pan_coord = 1,
                    .assoc_permit = 1,
                  };
        aux_t aux = {0};
        nonce_t nonce = {0};
        uint8_t aux_len = 0;
        if (session->conf.sec_level)
        {
            aux.sec_ctl.sec_level = session->conf.sec_level;
            aux.sec_ctl.key_id_mode = session->conf.key_id_mode;
            aux.frame_counter = 5;
            if (aux.sec_ctl.key_id_mode == KEY_ID_MODE_1_NOOFFSET)
            {
                aux.key_id = 1;
            }
            mac_memcpy(&nonce.src_ext_addr, mpan_GetLongAddress(info.mpan_idx), sizeof(nonce.src_ext_addr));
            nonce.sec_level = aux.sec_ctl.sec_level;
            nonce.frame_counter = aux.frame_counter;

            aux_len = sizeof(aux);
            if (aux.sec_ctl.key_id_mode == KEY_ID_MODE_0_NOOFFSET)
            {
                aux_len--;
            }
        }
        g_tx_buf.len = generate_ieee_frame(fc.type, g_tx_buf.buf, 125, fc, session->seq,
                                           spanid, src_addr, dpanid, dst_addr,
                                           (uint8_t *)&aux, aux_len, session->conf.cmd_id, &ss);
        if (session->conf.sec_level)
        {
            info.hdr_len = g_tx_buf.len;
        }

        GEN_SEQ_DATA_MV_PTR(g_tx_buf.buf, 0, session->conf.data_len, g_tx_buf.len);

        if (session->conf.sec_level)
        {
            if (session->conf.sec_level < SEC_ENC)
            {
                info.hdr_len = g_tx_buf.len;
            }
            // set nonce
            mac_nonce_set((uint8_t *)&nonce);
            // set key
            mac_txn_key_set(g_mac_key);
            // set security level
            mac_txn_cipher_set(session->conf.sec_level);
        }
    }

    if (session->conf.defer_time)
    {
        info.tx_mode = TX_MODE_SCHEDULE;
        info.schedule_mode = TX_SCHEDULE_DEFER;
        info.schedule_time = session->conf.defer_time;
    }

    ret = NETSTACK_MAC.send(&packet_sent, (void *)session, &info);
    if (ret == PT_MAC_TX_OK)
    {
        session->wait_mac = 1;
        session->mac_seq = session->seq;
        session->start_timestamp = clock_time();
        session->pending_retry = 0;
    }
    else
    {
        if (session->retry_ev == 0 &&
            process_post(&pktgen_process, PROCESS_EVENT_CONTINUE, (void *)session) == PROCESS_ERR_OK)
        {
            session->retry_ev = 1;
        }
        session->pending_retry = 1;
    }
    return ret;
}

PROCESS_THREAD(pktgen_process, ev, data)
{
    PROCESS_BEGIN();
    dbg_printf("%s start\r\n", PROCESS_NAME_STRING(PROCESS_CURRENT()));
    pktgen_init();
    while (1)
    {
        PROCESS_WAIT_EVENT();
        if (ev == PROCESS_EVENT_EXIT)
        {
            pktgen_exit();
            PROCESS_EXIT();
        }
        else if (ev == pktgen_new_event)
        {
            pktgen_session_t *session = (pktgen_session_t *)data;
            if (session->conf.silent <= 2)
            {
                dbg_printf("[%u] open pktgen session [%p]\r\n", clock_time(), session);
            }
            if (session->conf.silent == 0)
            {
                session_print(session);
            }

            if (session->conf.interval == 0)
            {
                pktgen_send(session);
            }
            else
            {
                if (session->conf.random == 0)
                {
                    pktgen_send(session);
                    etimer_set(&session->interval_timer, session->conf.interval);
                }
                else
                {
                    uint32_t r_value = 0;
                    session->near_start = clock_time();
                    do
                    {
                        r_value = rand() % session->conf.interval;
                    }
                    while (r_value == 0);

                    if (session->conf.cnt)
                    {
                        session->conf.cnt++;
                    }
                    if (session->conf.silent == 0)
                    {
                        dbg_printf("rand %u\r\n", r_value);
                    }
                    etimer_set(&session->interval_timer, r_value);
                }
            }
        }
        else if (ev == PROCESS_EVENT_MSG || (ev == PROCESS_EVENT_TIMER &&
                                             etimer_expired((struct etimer *)data)))
        {
            pktgen_session_t *session;
            if (ev == PROCESS_EVENT_TIMER)
            {
                // data is the address of etimer, etimer is the second element of the session
                session = (pktgen_session_t *)(((uint8_t *)data) - sizeof(void *));
                if (session->conf.random == 0)
                {
                    etimer_reset(data);
                }
                else
                {
                    uint32_t pass_time = clock_time() - session->near_start;
                    if (pass_time > session->conf.interval)
                    {
                        uint32_t rand_max = (session->conf.interval << 1) - pass_time;
                        if (rand_max == 0 || rand_max > session->conf.interval)
                        {
                            rand_max = 1;
                        }
                        uint32_t r_value = rand() % rand_max;
                        if (session->conf.silent == 0)
                        {
                            dbg_printf("rand %u/%u\r\n", r_value, rand_max);
                        }
                        etimer_set(&session->interval_timer, r_value);
                    }
                    else
                    {
                        uint32_t r_value = rand() % session->conf.interval;
                        if (session->conf.silent == 0)
                        {
                            dbg_printf("rand %u\r\n", r_value);
                        }
                        etimer_set(&session->interval_timer, r_value + session->conf.interval - pass_time);
                    }
                    session->near_start += session->conf.interval;
                }
            }
            else
            {
                session = (pktgen_session_t *)data;
            }
            // clean up previous pending retry
            if (session->pending_retry == 1)
            {
                packet_sent((void *)session, GIVE_UP_EVENT, 0, 0, 0);
                session->pending_retry = 0;
            }

            if (session->pending_free || (session->conf.cnt && session->seq == session->conf.cnt - 1))
            {
                if (session->retry_ev || session->wait_mac)
                {
                    //dbg_printf("[%u] pktgen %p: want free? pending_free %u retry_ev %u wait_mac %u\r\n", clock_time(), session,
                    //    session->pending_free, session->retry_ev, session->wait_mac);
                    if (session->pending_free == 1 && session->wait_mac == 1)
                    {
                        dbg_printf("pktgen: wait_mac timeout. force free session %p\r\n", session);
                        session_free(session);
                    }
                    else
                    {
                        session->pending_free = 1;
                        etimer_set(&session->interval_timer, 1);
                    }
                }
                else
                {
                    session_free(session);
                }
            }
            else
            {
                session->seq++;
                pktgen_send(session);
            }
        }
        else if (ev == PROCESS_EVENT_CONTINUE)     // use PROCESS_EVENT_CONTINUE as retry event
        {
            pktgen_session_t *session = (pktgen_session_t *)data;
            session->retry_ev = 0;
            if (session->pending_retry == 1)
            {
                pktgen_send(session);
            }
        }
    }
    PROCESS_END();
}

static int pktgen_internal(int argc, void *data)
{
    int8_t key = -1, expr;
    uint32_t value = 0;
    char **argv;

    pktgen_conf_t conf =
    {
        .cnt = 1,
        .interval = 0, // ms
        .random = 0,
        .pan_idx = 0,
        .data_len = 10,
        .silent = 1,
        .defer_time = 0,
        .sec_level = 0,
        .key_id_mode = 0,
        .frm_type = FRAME_TYPE_DATA,
        .frm_ver = 0,
        .cmd_id = 0,
        .user_raw = NULL,
        .raw_len = 0,
        .user_cb = NULL,
    };

    if (argc < 0)   // call from function
    {
        pktgen_conf_t *pconf = data;
        mac_memcpy(&conf, pconf, sizeof(pktgen_conf_t));

        if (pconf->user_raw && pconf->raw_len)
        {
            conf.user_raw = osif_mem_alloc(RAM_TYPE_DATA_ON, pconf->raw_len);
            if (conf.user_raw == NULL)
            {
                if (pconf->silent <= 2)
                {
                    dbg_printf("Error: MALLOC Fail\r\n");
                }
                return FALSE;
            }
            mac_memcpy(conf.user_raw, pconf->user_raw, pconf->raw_len);
            conf.raw_len = pconf->raw_len;
        }
        else
        {
            pconf->user_raw = NULL;
            conf.raw_len = 0;
        }
    }
    else if (argc == 0)
    {
        goto error;
    }
    else
    {
        argv = data;
        conf.daddr = _strtoull((const char *)(argv[0]), (char **)NULL, 16);

        for (int i = 1; i < argc; i++)
        {
            if (key == 's' || key == 'c' || key == 'i' || key == 'p' ||
                key == 'd' || key == 'e' || key == 't' || key == 'u' ||
                key == 'r' || key == 'x')
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
                    if (value > 125)
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
            case 'j':
                conf.random = 1;
                break;
            case 'p':
                if (key == 'p')
                {
                    conf.pan_idx = value;
                    key = -1;
                }
                else
                {
                    key = 'p';
                }
                break;
            case 'd':
                if (key == 'd')
                {
                    if (value < 600)
                    {
                        goto error;
                    }
                    conf.defer_time = value;
                    key = -1;
                }
                else
                {
                    key = 'd';
                }
                break;
            case 't':
                if (key == 't')
                {
                    conf.frm_type = value;
                    if (conf.frm_type == FRAME_TYPE_ACK || conf.frm_type > FRAME_TYPE_COMMAND)
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
            case 'e':
                if (key == 'e')
                {
                    conf.sec_level = value / 10;
                    conf.key_id_mode = value % 10;
                    key = -1;
                }
                else
                {
                    key = 'e';
                }
                break;
            case 'u':
                if (key == 'u')
                {
                    conf.cmd_id = value;
                    key = -1;
                }
                else
                {
                    key = 'u';
                }
                break;
            case 'r':
                if (key == 'r')
                {
                    g_tx_buf.len = 0;
                    for (; i < argc; i++)
                    {
                        char *str = argv[i];
                        for (int j = 0; str[j] != 0; j += 2)
                        {
                            g_tx_buf.buf[g_tx_buf.len++] = (parse_digit(str[j]) << 4) | parse_digit(str[j + 1]);
                        }
                    }
                    if (g_tx_buf.len == 0)
                    {
                        goto error;
                    }
                    conf.user_raw = osif_mem_alloc(RAM_TYPE_DATA_ON, g_tx_buf.len);
                    if (conf.user_raw == NULL)
                    {
                        dbg_printf("Error: MALLOC Fail\r\n");
                        return FALSE;
                    }
                    mac_memcpy(conf.user_raw, g_tx_buf.buf, g_tx_buf.len);
                    conf.raw_len = g_tx_buf.len;
                    //dbg_mem_dump(conf.user_raw, conf.raw_len);
                    key = -1;
                }
                else
                {
                    key = 'r';
                }
                break;
            case 'x':
                if (key == 'x')
                {
                    if (value > 3)
                    {
                        goto error;
                    }
                    conf.silent = value;
                    key = -1;
                }
                else
                {
                    key = 'x';
                }
                break;
            case 'v':
                conf.frm_ver = 1;
                break;
            default:
                i = argc;
                key = '_';
                break;
            }
        }
    }

    if (key != -1)
    {
        goto error;
    }

    if (!process_is_running(&pktgen_process))
    {
        process_start(&pktgen_process, NULL);
    }

    pktgen_session_t *session = session_alloc();
    if (session == NULL)
    {
        if (conf.silent <= 2)
        {
            dbg_printf("Error: pktgen session full\r\n");
        }
        if (conf.user_raw)
        {
            osif_mem_free(conf.user_raw);
        }
        return FALSE;
    }
    mac_memcpy(&session->conf, &conf, sizeof(pktgen_conf_t));
    process_post_synch(&pktgen_process, pktgen_new_event, session);
    //dbg_printf("Done\r\n");
    return TRUE;
error:
    dbg_printf("Error: InvalidArgs\r\n");
    return FALSE;
}

int pktgen(pktgen_conf_t *conf)
{
    return pktgen_internal(-1, (void *)conf);
}

static int cmd_pktgen(int argc, char *argv[])
{
    return pktgen_internal(argc, (void *)argv);
}

void shell_register_cmd_pktgen(void)
{
    pktgen_process.state = 0;
    pktgen_new_event = 0;
    shell_register_pt((shell_program_t)cmd_pktgen, "pktgen",
                      BRIEF("packet generator")
                      SYNOPSIS("pktgen <shortaddr/extaddr>")
                      DESCRIPTION("	shortaddr: 0x0000 ~ 0xffff")
                      DESCRIPTION("	extaddr: 0x0000000000010000 ~ 0xffffffffffffffff")
                      DESCRIPTION("c <loop_cnt>")
                      DESCRIPTION("	0 ~ 4294967295")
                      DESCRIPTION("	0 - infinite loop")
                      DESCRIPTION("	1 (default)")
                      DESCRIPTION("d <defer>")
                      DESCRIPTION("	600 ~ 4294967295 us")
                      DESCRIPTION("e <sec_level><key_id_mode>")
                      DESCRIPTION("	sec_level: 1 ~ 7")
                      DESCRIPTION("	1 - SEC_MIC_32")
                      DESCRIPTION("	2 - SEC_MIC_64")
                      DESCRIPTION("	3 - SEC_MIC_128")
                      DESCRIPTION("	4 - SEC_ENC")
                      DESCRIPTION("	5 - SEC_ENC_MIC_32")
                      DESCRIPTION("	6 - SEC_ENC_MIC_64")
                      DESCRIPTION("	7 - SEC_ENC_MIC_128")
                      DESCRIPTION("	key_id_mode: 0 ~ 3")
                      DESCRIPTION("	For example: e 40 (SEC_ENC + key_id_mode 0)")
                      DESCRIPTION("i <interval>")
                      DESCRIPTION("	0 ~ 4294967295 ms")
                      DESCRIPTION("	0 (default)")
                      DESCRIPTION("j    random tx in the interval")
                      DESCRIPTION("p <pan_index>")
                      DESCRIPTION("	0 ~ 3")
                      DESCRIPTION("	0 (default)")
                      DESCRIPTION("r <raw data>")
                      DESCRIPTION("	option must be placed at the end of command")
                      DESCRIPTION("	For example: pktgen 0 r 0022008611341233cf000f040dae09350c803f0102")
                      DESCRIPTION("s <size>")
                      DESCRIPTION("	0 ~ 116")
                      DESCRIPTION("	10 (default)")
                      DESCRIPTION("t <frm_type>")
                      DESCRIPTION("	0 - BEACON")
                      DESCRIPTION("	1 - DATA")
                      DESCRIPTION("	3 - COMMAND")
                      DESCRIPTION("	1 (default)")
                      DESCRIPTION("u <cmd_id>")
                      DESCRIPTION("	0 ~ 255")
                      DESCRIPTION("v	frame version 2015")
                      DESCRIPTION("x <level>")
                      DESCRIPTION("	3 - silent mode")
                      DESCRIPTION("	2 - show error")
                      DESCRIPTION("	1 - show info (default)")
                      DESCRIPTION("	0 - show debug")
                      EXAMPLE("pktgen 0x1 c 10")
                      DESCRIPTION("	send 10 data packets to short address 0x1")
                      EXAMPLE("pktgen 0x10000 e 40 v")
                      DESCRIPTION("	send 802.15.4-2015 secure data packet to extend address 0x10000")
                     );
}
