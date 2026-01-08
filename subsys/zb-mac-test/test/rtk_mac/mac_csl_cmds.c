/**************************************************************************//**
 * @file     mac_csl_cmds.c
 * @brief    Source file for proprietary Coordinated Sample Listening implementation
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

#include <os_msg.h>
#include <os_mem.h>
#include <os_task.h>
#include "shell.h"
#include "dbg_printf.h"
#include "strproc.h"
#include "mac_test_common.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/bitmap.h"
#include "mac_csl.h"
#include "crc16btx.h"
#include "rtl876x_pinmux.h"

extern void *mac_memcpy(void *dest, const void *src, uint32_t len);

#define OPTION_CSL_NONE         0x0
#define OPTION_CSL_SYN          0x1
#define OPTION_CSL_WAKEUP_REQ   0x2
#define OPTION_CSL_WAKEUP_REP   0x3
#define OPTION_CSL_SLEEP_REQ    0x4
#define OPTION_CSL_SLEEP_REP    0x5 // not used, send OPTION_CSL_SYN instead
#define OPTION_CSL_DATA         0x6
#define OPTION_CSL_FRAG         0x7

#define CSL_EV_NONE             0x0
#define CSL_EV_LISTEN_END       0x1
#define CSL_EV_PM_EXIT          0x2
#define CSL_EV_INDIRECT_SEND    0x3
#define CSL_EV_DIRECT_SEND      0x4
#define CSL_EV_WAKEUP_REQ       0x5
#define CSL_EV_WAKEUP_REP       0x6
#define CSL_EV_SLEEP_REQ        0x7
#define CSL_EV_DATA_SEND        0x8
#define CSL_EV_SWEEP            0x9
#define CSL_EV_SCHE_START       0xa
#define CSL_EV_SCHE_STOP        0xb
#define CSL_EV_MAX              0xc

#define MAX_QUEUE_BUF_LEN 16
#define MAX_QUEUE_PACKETS 5

#define MAX_RETRY 3
#define PPM 1000000
#define TASK_NOTIFY_DELAY 34 // us
#define BLOCK_SIZE 64
#define DEFAULT_PERIOD 6000000 // us
#define DEFAULT_LISTEN_WINDOW 7000 // us
#if (TEST_BT_PM_EN == 1)
#define DEFAULT_AUTO_SYNC_PERIOD 60000000 // us
#else
#define DEFAULT_AUTO_SYNC_PERIOD 480000000 // us
#endif

#define INIT_WAKEUP_PIN P2_3 // key 3

enum CSL_SCHE_STATE
{
    CSL_SCHE_STOP        = 0,
    CSL_SCHE_ENTR_PM     = 1,
    CSL_SCHE_EXIT_PM     = 2,
    CSL_SCHE_EV_PM       = 3,
    CSL_SCHE_ENTR_TM     = 4,
    CSL_SCHE_EXIT_TM     = 5,
    CSL_SCHE_EV_TM       = 6,
};

struct option_head
{
    uint8_t option_type;
    uint8_t option_len;
} __attribute__((packed));

struct option_csl_sync
{
    struct option_head head;
    uint32_t csl_period;
    uint32_t csl_listen_window;
} __attribute__((packed));

struct option_csl_frag
{
    struct option_head head;
    uint16_t frag_crc;
    uint16_t total_len;
    uint16_t m_flag : 1;
    uint16_t block_id : 15;
} __attribute__((packed));

struct queuebuf
{
    uint8_t buf[MAX_QUEUE_BUF_LEN];
    uint8_t len;
};

struct packet_queue
{
    struct packet_queue *next;
    struct queuebuf buf;
    void *ptr;
    uint8_t len;
};

struct ep_info_s
{
    struct ep_info_s *next;
    uint32_t period; // csl period
    uint32_t anchor; // csl anchor timestamp
    uint32_t window; // csl listen window
    uint16_t addr;   // endpoint address
};
LIST(g_csl_ep_list);

static void *g_csl_ctimer = NULL;
static buf_t g_csl_buf;
static uint32_t g_csl_anchor_timestamp = 0;
static volatile uint32_t g_csl_new_anchor_timestamp = 0;
static volatile bool g_csl_schedule_stop = TRUE;
static void *g_csl_ev_queue_handle;
static void *g_csl_task_handle;
MEMB(packet_memb, struct packet_queue, MAX_QUEUE_PACKETS);
LIST(g_csl_queue_list);
static uint32_t g_csl_period; // us
static uint32_t g_csl_recent_sync_timestamp = 0;
static uint8_t g_csl_sche_state = CSL_SCHE_STOP;
static uint32_t g_csl_dbg_counter = 0;
static bool task_is_running = FALSE;
static uint32_t g_csl_autosync_period = 0;
static uint32_t g_csl_resync_req = 0;
static uint32_t g_csl_conf_period = DEFAULT_PERIOD; // us
static bool g_csl_loop_test = FALSE;
static bool g_csl_debug_timing = FALSE;
static bool g_csl_debug = FALSE;
#define MAX_TEST_DATA (MAX_QUEUE_BUF_LEN - 2)
static uint8_t g_csl_test_data[MAX_TEST_DATA];
static int csl_start_task(uint32_t *period, uint32_t *window, uint8_t role, uint16_t *peer_addr);
static int csl_sync_process(void);
static uint8_t *g_image = NULL;
static uint32_t g_image_size = 0;
static uint16_t g_frag_crc = 0;
static bitmap_t *g_frag_bitmap = NULL;

// global variable
struct protocol_fn PROTOCOL = {0};
uint16_t g_csl_peer_addr;
int g_csl_ppm = 0;
uint8_t g_csl_role = CSL_ROLE_INVALID;
int g_csl_tx_check_offest = 0;
uint32_t g_csl_ack_require_time = 0;
uint32_t g_csl_prepare_to_rx = 0;
uint32_t g_csl_listen_window = DEFAULT_LISTEN_WINDOW; // us

#if TEST_GPIO_DBG_EN
void trigger_gpio_at_anchor(bool reverse)
{
    if (PROTOCOL.get_curr_timestamp() > g_csl_anchor_timestamp)
    {
        return;
    }

    while (g_csl_anchor_timestamp > PROTOCOL.get_curr_timestamp());
    if (reverse)
    {
        debug_gpio_low(GPIO_OUTPUT_PIN_0);
        debug_gpio_high(GPIO_OUTPUT_PIN_0);
    }
    else
    {
        debug_gpio_high(GPIO_OUTPUT_PIN_0);
        debug_gpio_low(GPIO_OUTPUT_PIN_0);
    }
}
#endif

static uint8_t *create_csl_msg(uint8_t *msgptr)
{
    return msgptr;
}

static uint8_t *add_option_csl_sync(uint8_t *optptr, uint32_t period, uint32_t listen_window)
{
    struct option_csl_sync *option_csl_sync = (struct option_csl_sync *)optptr;
    option_csl_sync->head.option_type = OPTION_CSL_SYN;
    option_csl_sync->head.option_len = 8;
    option_csl_sync->csl_period = period;
    option_csl_sync->csl_listen_window = listen_window;
    return optptr + sizeof(struct option_csl_sync);
}

static uint8_t *add_option_csl_data(uint8_t *optptr, uint8_t *data, uint8_t len)
{
    struct option_head *option_head = (struct option_head *)optptr;
    option_head->option_type = OPTION_CSL_DATA;
    option_head->option_len = len;
    mac_memcpy(optptr + sizeof(struct option_head), data, len);
    return optptr + sizeof(struct option_head) + len;
}

static uint8_t *add_option_csl_frag(uint8_t *optptr, uint8_t *data, uint16_t frag_crc,
                                    uint16_t total_len,
                                    uint8_t m_flag, uint16_t block_id, uint8_t data_len)
{
    struct option_csl_frag *option_csl_frag = (struct option_csl_frag *)optptr;
    option_csl_frag->head.option_type = OPTION_CSL_FRAG;
    option_csl_frag->head.option_len = sizeof(struct option_csl_frag) - sizeof(
                                           struct option_head) + data_len;
    option_csl_frag->frag_crc = frag_crc;
    option_csl_frag->total_len = total_len;
    option_csl_frag->m_flag = m_flag;
    option_csl_frag->block_id = block_id;
    mac_memcpy(optptr + sizeof(struct option_csl_frag), data, data_len);
    return optptr + sizeof(struct option_csl_frag) + data_len;
}

static uint8_t *add_option_csl_no_data_command(uint8_t *optptr, uint8_t type)
{
    struct option_head *option_head = (struct option_head *)optptr;
    option_head->option_type = type;
    option_head->option_len = 0;
    return optptr + sizeof(struct option_head);
}

static void csl_timer_cb(void *arg)
{
    g_csl_sche_state = CSL_SCHE_EXIT_TM;
    uint8_t event = (g_csl_role == CSL_ROLE_ENDPOINT) ? CSL_EV_LISTEN_END : CSL_EV_INDIRECT_SEND;
    if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
    {
        dbg_printf("os_msg_send failed\r\n");
    }
    //if (g_csl_debug == TRUE)
    //    dbg_printf("csl_timer_cb ev %u\r\n", event);
}

static void csl_schedule_periodic_rx(void)
{
    if (PROTOCOL.timer_stop)
    {
        PROTOCOL.timer_stop(g_csl_ctimer);
    }

    if (g_csl_schedule_stop == TRUE)
    {
        dbg_printf("Wake up on coordinator request\r\n");
        return;
    }

    if (g_csl_new_anchor_timestamp)
    {
        dbg_printf("Coordinator request re-sync.\r\n");
        g_csl_anchor_timestamp = g_csl_new_anchor_timestamp;
        g_csl_recent_sync_timestamp = g_csl_anchor_timestamp;
        g_csl_new_anchor_timestamp = 0;
    }

    uint32_t now = PROTOCOL.get_curr_timestamp();
    if (g_csl_debug_timing)
    {
        dbg_printf("sleep %u diff %u recent %u\r\n", now, now - g_csl_anchor_timestamp,
                   now - g_recent_rx_timestamp);
    }

    // re-sync from endpoint
    if (g_csl_autosync_period && (now - g_csl_recent_sync_timestamp > g_csl_autosync_period))
    {
        if (csl_sync_process() == TRUE)
        {
            return;
        }
    }

    /* The next anchor point will be greater than the current time,
     * or wrap around to the minimum value of u32 */
    bool close_to_32_max = (now + g_csl_period) < now ? 1 : 0;
    while (g_csl_anchor_timestamp < now)
    {
        uint32_t new_anchor = g_csl_anchor_timestamp + g_csl_period;
        if (close_to_32_max == 1)
        {
            if (new_anchor < g_csl_anchor_timestamp)
            {
                g_csl_anchor_timestamp = new_anchor;
                break;
            }
        }
        g_csl_anchor_timestamp = new_anchor;
    }

    /* Too close to meet quard time requirement (6ms) of power management */
    if (g_csl_anchor_timestamp - now < g_csl_prepare_to_rx + 6000) // add more 6 ms guard time
    {
        g_csl_anchor_timestamp += g_csl_period;
    }

#if TEST_GPIO_DBG_EN
    debug_gpio_low(GPIO_OUTPUT_PIN_0);
#endif
    g_csl_sche_state = CSL_SCHE_ENTR_PM;
    g_csl_dbg_counter = g_csl_anchor_timestamp - PROTOCOL.get_curr_timestamp() - g_csl_prepare_to_rx;
    if (g_csl_debug == TRUE)
    {
        dbg_printf("entr_pm %u\r\n", g_csl_dbg_counter);
        // update g_csl_dbg_counter again, because printf may take 200 us
        g_csl_dbg_counter = g_csl_anchor_timestamp - PROTOCOL.get_curr_timestamp() - g_csl_prepare_to_rx;
    }

    if (PROTOCOL.pm_set(g_csl_dbg_counter) == FALSE)
    {
        dbg_printf("pm_set fail\r\n");
    }
}
static void csl_schedule_periodic_tx(void)
{
    if (PROTOCOL.timer_stop)
    {
        PROTOCOL.timer_stop(g_csl_ctimer);
    }

    if (g_csl_schedule_stop == TRUE)
    {
        dbg_printf("The endpoint has woken up and terminated the tx scheduler\r\n");
        return;
    }

    uint32_t now = PROTOCOL.get_curr_timestamp();
    /* The next anchor point will be greater than the current time,
     * or wrap around to the minimum value of u32 */
    bool close_to_32_max = (now + g_csl_period) < now ? 1 : 0;
    while (g_csl_anchor_timestamp < now)
    {
        uint32_t new_anchor = g_csl_anchor_timestamp + g_csl_period;
        if (close_to_32_max == 1)
        {
            if (new_anchor < g_csl_anchor_timestamp)
            {
                g_csl_anchor_timestamp = new_anchor;
                break;
            }
        }
        g_csl_anchor_timestamp = new_anchor;
    }

    /* Too close to meet quard time requirement (20us) of timer set */
    if (g_csl_tx_check_offest > 0)
    {
        if (g_csl_anchor_timestamp - now + g_csl_tx_check_offest < 20)
        {
            g_csl_anchor_timestamp += g_csl_period;
        }
    }
    else
    {
        if (g_csl_anchor_timestamp - now < 20 - g_csl_tx_check_offest)
        {
            g_csl_anchor_timestamp += g_csl_period;
        }
    }

    if (PROTOCOL.timer_start)
    {
        g_csl_sche_state = CSL_SCHE_ENTR_TM;
        g_csl_dbg_counter = g_csl_anchor_timestamp - now + g_csl_tx_check_offest;
        PROTOCOL.timer_start(g_csl_ctimer, g_csl_dbg_counter, csl_timer_cb, 0);
        if (g_csl_debug_timing == TRUE)
        {
            dbg_printf("schedule tx after %u us\r\n", g_csl_dbg_counter);
        }
    }
}

static void csl_pm_enter(void)
{
    Pad_ClearWakeupINTPendingBit(INIT_WAKEUP_PIN);
    System_WakeUpPinEnable(INIT_WAKEUP_PIN, PAD_WAKEUP_POL_LOW, PAD_WK_DEBOUNCE_DISABLE, 0);
}

static void csl_pm_exit(void)
{
    g_csl_sche_state = CSL_SCHE_EXIT_PM;
#if TEST_GPIO_DBG_EN
    debug_gpio_low(GPIO_OUTPUT_PIN_0);
#endif
    if (PROTOCOL.pm_exit_cb)
    {
        PROTOCOL.pm_exit_cb();
    }
    uint8_t event = CSL_EV_PM_EXIT;
    if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
    {
        dbg_printf("os_msg_send failed\r\n");
    }
}

void csl_tx_queue_add(uint8_t option, uint8_t *data, uint8_t data_len)
{
    uint8_t *end;
    if (data)   // length check
    {
        if (data_len + sizeof(struct option_head) > MAX_QUEUE_BUF_LEN)
        {
            dbg_printf("packet too long\r\n");
            return;
        }
    }

    // buffer alloc
    struct packet_queue *packet = memb_alloc(&packet_memb);
    if (packet == NULL)
    {
        dbg_printf("no buffer\r\n");
        return;
    }
    memset(packet, 0, sizeof(struct packet_queue));
    uint8_t *buf = packet->buf.buf;

    end = create_csl_msg(buf);
    if (data)
    {
        end = add_option_csl_data(end, data, data_len);
    }
    else
    {
        end = add_option_csl_no_data_command(end, option);
    }
    packet->buf.len = end - packet->buf.buf;
    // append to queue list
    list_add(g_csl_queue_list, packet);
    if (g_csl_schedule_stop == TRUE)
    {
        uint8_t event = CSL_EV_DIRECT_SEND;
        if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
        {
            dbg_printf("os_msg_send failed\r\n");
        }
    }
}

static int csl_sync_process(void)
{
    uint8_t *end;
    uint32_t timestamp;

    end = create_csl_msg(g_csl_buf.buf);
    end = add_option_csl_sync(end, g_csl_conf_period, g_csl_listen_window);

    for (int i = 0; i < 3; i++)
    {
        if (PROTOCOL.send(g_csl_buf.buf, end - g_csl_buf.buf, &timestamp) == TRUE)
        {
            g_csl_anchor_timestamp = timestamp;
            g_csl_recent_sync_timestamp = g_csl_anchor_timestamp;
            g_csl_new_anchor_timestamp = 0;
            g_csl_schedule_stop = FALSE;
            csl_schedule_periodic_rx();
            dbg_printf("connecting to 0x%x\r\n", g_csl_peer_addr);
            return TRUE;
        }
    }
    dbg_printf("connecting to 0x%x fail\r\n", g_csl_peer_addr);
    return FALSE;
}

static void csl_wakeup_reply_send()
{
    uint8_t *end;
    uint32_t timestamp;
    end = create_csl_msg(g_csl_buf.buf);
    end = add_option_csl_no_data_command(end, OPTION_CSL_WAKEUP_REP);
    PROTOCOL.send(g_csl_buf.buf, end - g_csl_buf.buf, &timestamp);
}

static void *image_buffer_alloc(uint32_t size)
{
    if (g_image && g_image_size < size)
    {
        os_mem_free(g_image);
        g_image = NULL;
        g_image_size = 0;
    }
    if (g_image == NULL)
    {
        g_image = os_mem_alloc(RAM_TYPE_DATA_ON, size);
        if (g_image)
        {
            g_image_size = size;
        }
        else
        {
            dbg_printf("image buffer alloc fail\r\n");
        }
    }
    return g_image;
}

static struct ep_info_s *ep_info_find(uint16_t ep_addr)
{
    struct ep_info_s *ep;
    for (ep = list_head(g_csl_ep_list); ep != NULL; ep = list_item_next(ep))
    {
        if (ep->addr == ep_addr)
        {
            return ep;
        }
    }
    return NULL;
}

static struct ep_info_s *ep_info_alloc(uint16_t ep_addr)
{
    struct ep_info_s *ep = os_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct ep_info_s));
    if (ep)
    {
        memset(ep, 0, sizeof(struct ep_info_s));
        ep->addr = ep_addr;
        list_add(g_csl_ep_list, ep);
    }
    return ep;
}

static void ep_info_update(struct ep_info_s *ep, uint32_t period, uint32_t anchor, uint32_t window)
{
    if (ep)
    {
        ep->period = period;
        ep->anchor = anchor;
        ep->window = window;
    }
}

static void ep_info_active(struct ep_info_s *ep)
{
    g_csl_period = ep->period;
    g_csl_anchor_timestamp = ep->anchor;
    g_csl_listen_window = ep->window;
    g_csl_recent_sync_timestamp = ep->anchor;
    if (PROTOCOL.start_periodic_tx_sched_cb)
    {
        PROTOCOL.start_periodic_tx_sched_cb();
    }
    dbg_printf("endpoint 0x%04x period %u listen window %u connected\r\n", g_csl_peer_addr,
               g_csl_period, g_csl_listen_window);
    g_csl_schedule_stop = FALSE;
    csl_schedule_periodic_tx();
}

void csl_input(uint8_t *data, uint8_t len, uint32_t rx_timestamp, uint16_t saddr)
{
    uint8_t *optptr = data;
    uint8_t *end = optptr + len;
    //dbg_mem_dump(optptr, len);
    struct option_head *option_head;
    //dbg_printf("csl_input: data %p len %u rx_timestamp %u saddr 0x%x\r\n", data, len, rx_timestamp, saddr);
    for (; optptr < end; optptr += (sizeof(struct option_head) + option_head->option_len))
    {
        option_head = (struct option_head *)optptr;
        //dbg_printf("type %u len %u\r\n", option_head->option_type, option_head->option_len);
        //dbg_mem_dump(optptr + sizeof(struct option_head), option_head->option_len);
        switch (option_head->option_type)
        {
        case OPTION_CSL_SYN:
            {
                if (g_csl_role == CSL_ROLE_COORD)
                {
                    struct option_csl_sync *option_csl_sync = (struct option_csl_sync *)optptr;
                    struct ep_info_s *ep = ep_info_find(saddr);
                    if (ep == NULL)
                    {
                        ep = ep_info_alloc(saddr);
                    }

                    if (ep)
                    {
                        ep_info_update(ep, option_csl_sync->csl_period, rx_timestamp, option_csl_sync->csl_listen_window);
                    }

                    if (g_csl_peer_addr == saddr)
                    {
                        uint8_t event = CSL_EV_SCHE_START;
                        if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
                        {
                            dbg_printf("os_msg_send failed\r\n");
                        }
                    }
                }
                else if (g_csl_role == CSL_ROLE_ENDPOINT)
                {
                    g_csl_new_anchor_timestamp = rx_timestamp; // get rx time as new csl anchor point
                }
                break;
            }
        case OPTION_CSL_WAKEUP_REQ:
            {
                g_csl_schedule_stop = TRUE;
                csl_wakeup_reply_send();
                break;
            }
        case OPTION_CSL_WAKEUP_REP:
            {
                uint8_t event = CSL_EV_WAKEUP_REP;
                if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
                {
                    dbg_printf("os_msg_send failed\r\n");
                }
                break;
            }
        case OPTION_CSL_SLEEP_REQ:
            {
                if (csl_start_task(NULL, NULL, CSL_ROLE_ENDPOINT, &saddr) == FALSE)
                {
                    csl_sync_process();
                }
                break;
            }
        case OPTION_CSL_DATA:
            {
                uint32_t anchor = g_csl_anchor_timestamp;
                if ((rx_timestamp + (g_csl_period >> 1)) < g_csl_anchor_timestamp)
                {
                    anchor = g_csl_anchor_timestamp - g_csl_period;
                }
                uint32_t diff = ((rx_timestamp > anchor) ? (rx_timestamp - anchor)
                                 : (anchor - rx_timestamp));
                dbg_printf("diff:%c%u msg:%s\r\n", (rx_timestamp > anchor) ? '+' : '-', diff,
                           optptr + sizeof(struct option_head));
                break;
            }
        case OPTION_CSL_FRAG:
            {
                struct option_csl_frag *option_csl_frag = (struct option_csl_frag *)optptr;
                uint8_t data_length = option_csl_frag->head.option_len + sizeof(struct option_head) - sizeof(
                                          struct option_csl_frag);
                if (!g_frag_bitmap)
                {
                    g_frag_crc = option_csl_frag->frag_crc;
                }

                if (g_frag_bitmap && option_csl_frag->frag_crc != g_frag_crc)
                {
                    bitmap_free(g_frag_bitmap);
                    g_frag_bitmap = NULL;
                }
                /*
                * case 1: there is only one block, block_size = data_length.
                * case 2: it is an intermediate block, block_size = data_length.
                * case 3: it is the last block, calculate the block size.
                */
                uint8_t block_size = data_length;
                if (option_csl_frag->m_flag == 0 && option_csl_frag->block_id != 0) // case 3
                {
                    block_size = (option_csl_frag->total_len - data_length) / option_csl_frag->block_id;
                }

                if (g_frag_bitmap == NULL && block_size)
                {
                    dbg_printf("start receiving fragment data. total length = %u block size = %u now = %u\r\n",
                               option_csl_frag->total_len, block_size, PROTOCOL.get_curr_timestamp());
                    g_frag_bitmap = bitmap_alloc((option_csl_frag->total_len + block_size - 1) / block_size);
                    image_buffer_alloc(option_csl_frag->total_len);
                }

                if (g_frag_bitmap)
                {
                    int ret = bitmap_get(g_frag_bitmap, option_csl_frag->block_id);
                    if (ret == 0)
                    {
                        bitmap_set(g_frag_bitmap, option_csl_frag->block_id);
                        if (g_image)
                        {
                            mac_memcpy(g_image + option_csl_frag->block_id * block_size,
                                       optptr + sizeof(struct option_csl_frag), data_length);
                        }
                        // All data received?
                        if (g_frag_bitmap->received_blocks == g_frag_bitmap->total_blocks)
                        {
                            uint16_t re_crc = 0x0000;
                            if (g_image)
                            {
                                re_crc = btxfcs(0x0000, g_image, option_csl_frag->total_len);
                            }
                            if (re_crc == g_frag_crc)
                            {
                                dbg_printf("All data received successfully. now = %u\r\n", PROTOCOL.get_curr_timestamp());
                                if (g_csl_debug == TRUE)
                                {
                                    if (g_image)
                                    {
                                        dbg_mem_dump(g_image, option_csl_frag->total_len);
                                    }
                                }
                            }
                            else
                            {
                                dbg_printf("The received data is incorrect. image = %p\r\n", g_image);
                                if (g_csl_debug == TRUE)
                                {
                                    if (g_image)
                                    {
                                        dbg_mem_dump(g_image, option_csl_frag->total_len);
                                    }
                                }
                            }
                            bitmap_free(g_frag_bitmap);
                            g_frag_bitmap = NULL;
                        }
                    }
                    else if (ret == 1)
                    {
                        dbg_printf("Ignore duplicated block\r\n");
                    }
                    else
                    {
                        dbg_printf("Ignore wrong block_id\r\n");
                    }
                }
                else
                {
                    dbg_printf("error: no bitmap\r\n");
                }
                break;
            }
        default:
            dbg_printf("unknown option\r\n");
            break;
        }
    }
}

void System_Handler(void)
{
    if (System_WakeUpInterruptValue(INIT_WAKEUP_PIN) == SET)
    {
        Pad_ClearWakeupINTPendingBit(INIT_WAKEUP_PIN);
        System_WakeUpPinDisable(INIT_WAKEUP_PIN);
        g_csl_schedule_stop = TRUE;
        if (PROTOCOL.pm_init_wakeup)
        {
            PROTOCOL.pm_init_wakeup();
        }
    }
}

static void csl_main_task(void *p_param)
{
    static uint8_t retry = 0;
    uint8_t event;

    // register initiate wakeup pin
    Pad_Config(INIT_WAKEUP_PIN, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

    if (PROTOCOL.config_init)
    {
        PROTOCOL.config_init();
    }

    os_msg_queue_create(&g_csl_ev_queue_handle, 3, sizeof(uint8_t));
    // create timer to wait packet in listening window
    if (PROTOCOL.timer_init)
    {
        g_csl_ctimer = PROTOCOL.timer_init();
        if (g_csl_ctimer == NULL)
        {
            dbg_printf("csl daemon start: timer_init fail\r\n");
            return;
        }
    }

    if (g_csl_role == CSL_ROLE_ENDPOINT)
    {
        dbg_printf("csl daemon start: endpoint\r\n");
        g_csl_autosync_period = DEFAULT_AUTO_SYNC_PERIOD;
        g_csl_period = g_csl_conf_period + ((int)g_csl_conf_period * g_csl_ppm / PPM);
        // register DLPS exit callback
        PROTOCOL.pm_init(csl_pm_enter, csl_pm_exit);
        csl_sync_process();
    }
    else
    {
        dbg_printf("csl daemon start: coordinator\r\n");
        memb_init(&packet_memb);
    }

    while (true)
    {
        if (os_msg_recv(g_csl_ev_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            //dbg_printf("ev %u\r\n", event);
            switch (event)
            {
            case CSL_EV_SCHE_START:
                {
                    if (g_csl_role == CSL_ROLE_COORD)
                    {
                        struct ep_info_s *ep = ep_info_find(g_csl_peer_addr);
                        if (ep)
                        {
                            ep_info_active(ep);
                        }
                    }
                    else if (g_csl_role == CSL_ROLE_ENDPOINT)
                    {
                        g_csl_schedule_stop = FALSE;
                        csl_schedule_periodic_rx();
                    }
                    break;
                }
            case CSL_EV_SCHE_STOP:
                {
                    g_csl_schedule_stop = TRUE;
                    break;
                }
            case CSL_EV_LISTEN_END:
                {
                    g_csl_sche_state = CSL_SCHE_EV_TM;
                    if (g_csl_debug == TRUE)
                    {
                        dbg_printf("exit_tm\r\n");
                    }
                    // Check whether the listening window needs to be extended
                    uint32_t now = PROTOCOL.get_curr_timestamp();
                    uint32_t recent_rx_offset = now - g_recent_rx_timestamp;
                    if (PROTOCOL.timer_start && recent_rx_offset + 30 < g_csl_ack_require_time)
                    {
                        g_csl_sche_state = CSL_SCHE_ENTR_TM;
                        g_csl_dbg_counter = g_csl_ack_require_time - recent_rx_offset;
                        //PROTOCOL.timer_start(g_csl_ctimer, require_time, csl_timer_cb, 0);
                        PROTOCOL.timer_start(g_csl_ctimer, g_csl_dbg_counter, csl_timer_cb, 0);
                        if (g_csl_debug == TRUE)
                        {
                            dbg_printf("entr_tm: extend %u\r\n", g_csl_dbg_counter);
                        }
                    }
                    else
                    {
#if TEST_GPIO_DBG_EN
                        debug_gpio_low(GPIO_OUTPUT_PIN_0);
#endif
                        csl_schedule_periodic_rx();
                    }
                    break;
                }
            case CSL_EV_PM_EXIT:
                {
                    g_csl_sche_state = CSL_SCHE_EV_PM;
                    if (g_csl_schedule_stop == TRUE)
                    {
                        if (g_csl_debug == TRUE)
                        {
                            dbg_printf("exit_pm with schedule stop\r\n");
                        }
                        break;
                    }
                    trigger_gpio_at_anchor(0);
                    uint32_t now = PROTOCOL.get_curr_timestamp();
                    uint32_t diff = ((now > g_csl_anchor_timestamp) ? (now - g_csl_anchor_timestamp) :
                                     (g_csl_anchor_timestamp - now));
                    if (g_csl_debug_timing)
                    {
                        dbg_printf("wakeup %u diff %s%u\r\n", now, (now > g_csl_anchor_timestamp) ? "+" : "-", diff);
                    }
#if TEST_GPIO_DBG_EN
                    debug_gpio_high(GPIO_OUTPUT_PIN_0);
#endif
                    if (now - (g_csl_anchor_timestamp - TASK_NOTIFY_DELAY - g_csl_listen_window) >
                        (g_csl_listen_window << 1))
                    {
                        // The listening window time has expired, give up listening
                        //if (g_csl_debug == TRUE)
                        dbg_printf("exit_pm: listening window expired. diff %s%u\r\n",
                                   (now > g_csl_anchor_timestamp) ? "+" : "-", diff);
                        csl_schedule_periodic_rx();
                    }
                    else
                    {
                        // start listening window
                        if (g_csl_debug == TRUE)
                        {
                            dbg_printf("exit_pm\r\n");
                        }
                        if (PROTOCOL.timer_start)
                        {
                            g_csl_sche_state = CSL_SCHE_ENTR_TM;
                            g_csl_dbg_counter = g_csl_anchor_timestamp + g_csl_listen_window - TASK_NOTIFY_DELAY - now;
                            PROTOCOL.timer_start(g_csl_ctimer, g_csl_dbg_counter, csl_timer_cb, 0);
                            if (g_csl_debug == TRUE)
                            {
                                dbg_printf("entr_tm %u\r\n", g_csl_dbg_counter);
                            }
                        }
                    }
                    break;
                }
            case CSL_EV_INDIRECT_SEND:
                {
                    g_csl_sche_state = CSL_SCHE_EV_TM;
                    uint32_t now = PROTOCOL.get_curr_timestamp();
                    uint32_t diff = ((now > g_csl_anchor_timestamp) ? (now - g_csl_anchor_timestamp) :
                                     (g_csl_anchor_timestamp - now));
                    // check queue and send only one packet
                    struct packet_queue *packet = list_head(g_csl_queue_list);
                    if (packet)
                    {
                        uint32_t timestamp;
                        uint8_t *end;
                        int ret;
                        /*
                         * re-sync condition:
                         *     1. the number of re-sync request is configured a value
                         *  or 2. auto sync period is not 0
                        */
                        if (g_csl_resync_req || (g_csl_autosync_period &&
                                                 (now - g_csl_recent_sync_timestamp > g_csl_autosync_period)))
                        {
                            end = create_csl_msg(g_csl_buf.buf);
                            end = add_option_csl_sync(end, NV_FIELD,
                                                      NV_FIELD); // period field is not used for Coord to Endpoint sync
                            mac_memcpy(end, packet->buf.buf, packet->buf.len);
                            end += packet->buf.len;
                            ret = PROTOCOL.send(g_csl_buf.buf, end - g_csl_buf.buf, &timestamp);
                            if (ret == TRUE)
                            {
                                if (g_csl_resync_req)
                                {
                                    g_csl_resync_req--;
                                }
                                g_csl_anchor_timestamp = timestamp;
                                dbg_printf("Coordinator request re-sync. success\r\n");
                                g_csl_recent_sync_timestamp = timestamp;
                                struct ep_info_s *ep = ep_info_find(g_csl_peer_addr);
                                if (ep)
                                {
                                    ep_info_update(ep, ep->period, timestamp, ep->window);
                                }
                            }
                        }
                        else
                        {
                            ret = PROTOCOL.send(packet->buf.buf, packet->buf.len, &timestamp);
                        }
                        if (g_csl_loop_test == FALSE)
                        {
                            if (ret == FALSE && retry < MAX_RETRY)
                            {
                                retry++;
                            }
                            else
                            {
                                list_pop(g_csl_queue_list);
                                memb_free(&packet_memb, packet);
                                retry = 0;
                            }
                        }
                    }
                    if (g_csl_debug_timing)
                    {
                        dbg_printf("checkpoint %u diff %s%u\r\n", now, (now > g_csl_anchor_timestamp) ? "+" : "-", diff);
                    }
                    trigger_gpio_at_anchor(0);
                    csl_schedule_periodic_tx();
                    break;
                }
            case CSL_EV_WAKEUP_REP:
            case CSL_EV_SWEEP:
                if (g_csl_schedule_stop == FALSE)
                {
                    if (PROTOCOL.stop_periodic_tx_sched_cb)
                    {
                        PROTOCOL.stop_periodic_tx_sched_cb();
                    }
                }
                g_csl_schedule_stop = TRUE;
                csl_schedule_periodic_tx();
            case CSL_EV_DIRECT_SEND:
                {
                    // send all packet
                    struct packet_queue *packet;
                    uint32_t timestamp;
                    packet = list_pop(g_csl_queue_list);
                    while (packet)
                    {
                        if (event != CSL_EV_SWEEP)
                        {
                            PROTOCOL.send(packet->buf.buf, packet->buf.len, &timestamp);
                        }
                        memb_free(&packet_memb, packet);
                        packet = list_pop(g_csl_queue_list);
                    }
                    break;
                }
            case CSL_EV_WAKEUP_REQ:
                csl_tx_queue_add(OPTION_CSL_WAKEUP_REQ, NULL, 0);
                break;
            case CSL_EV_SLEEP_REQ:
                csl_tx_queue_add(OPTION_CSL_SLEEP_REQ, NULL, 0);
                break;
            case CSL_EV_DATA_SEND:
                csl_tx_queue_add(OPTION_CSL_DATA, g_csl_test_data, MAX_TEST_DATA);
                break;
            default:
                break;
            }
        }
    }
}

// add fragmentation function
static uint8_t *get_tx_frag_frm(uint8_t *image, uint16_t image_size, uint16_t block_id,
                                uint8_t block_size, uint8_t *data_length)
{
    uint16_t last_block_id = ((image_size + block_size - 1) / block_size) - 1;
    if (block_id > last_block_id)
    {
        return NULL;
    }
    uint8_t last_block_size  = image_size % block_size;
    if (block_id == last_block_id && last_block_size != 0)
    {
        *data_length = last_block_size;
    }
    else
    {
        *data_length = block_size;
    }
    return image + block_id * block_size;
}

static int csl_start_task(uint32_t *period, uint32_t *window, uint8_t role, uint16_t *peer_addr)
{
    if (task_is_running == FALSE)
    {
        task_is_running = TRUE;
        if (period)
        {
            g_csl_conf_period = *period;
        }
        if (window)
        {
            g_csl_listen_window = *window;
        }
        g_csl_role = role;
        if (peer_addr)
        {
            g_csl_peer_addr = *peer_addr;
        }
        // create csl application task
        os_task_create(&g_csl_task_handle, "csl_task", csl_main_task, NULL,
                       1024 * 4, 4);
        return TRUE;
    }
    return FALSE;
}
#include "ftl.h"
#include "board.h"
bool save_cfg_shortaddr(uint16_t addr)
{
#if defined(FTL_CFG_SHORTADDR_BASE_ADDR)
    //dbg_printf("[save_cfg_shortaddr] shortaddr = 0x%02x\r\n", addr);
    uint32_t result = false;
    uint32_t cfg_shortaddr = addr;

    result = ftl_save(&cfg_shortaddr, FTL_CFG_SHORTADDR_BASE_ADDR,
                      FTL_CFG_SHORTADDR_LEN);

    return (result == 0);
#else
    dbg_printf("Not support. Please define FTL_CFG_SHORTADDR_BASE_ADDR!!\r\n");
    return 0;
#endif
}

uint32_t load_cfg_shortaddr(uint16_t *addr)
{
#if defined(FTL_CFG_SHORTADDR_BASE_ADDR)
    uint32_t ftl_res = 0;
    uint32_t cfg_shortaddr = 0;

    ftl_res = ftl_load(&cfg_shortaddr, FTL_CFG_SHORTADDR_BASE_ADDR, FTL_CFG_SHORTADDR_LEN);
    if (ftl_res == FTL_READ_SUCCESS)
    {
        *addr = (uint16_t)cfg_shortaddr;
    }
    return ftl_res;
#else
    dbg_printf("Not support. Please define FTL_CFG_SHORTADDR_BASE_ADDR!!\r\n");
    return FTL_READ_ERROR_NOT_INIT;
#endif
}

static int cmd_csl(int argc, char *argv[])
{
    uint16_t addr = 0x2;
    uint32_t value = DEFAULT_PERIOD; // 3s
    uint32_t listen_window = DEFAULT_LISTEN_WINDOW; // 4ms
    uint8_t role;
    switch (argc)
    {
    case 4:
        listen_window = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
    case 3:
        addr = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
    case 2:
        value = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
    case 1:
        if (strcmp(argv[0], "data") == 0)
        {
            strncpy((char *)g_csl_test_data, argv[1], MAX_TEST_DATA);
            g_csl_test_data[MAX_TEST_DATA - 1] = 0;

            if (g_csl_role == CSL_ROLE_COORD)
            {
                uint8_t event = CSL_EV_DATA_SEND;
                if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
                {
                    dbg_printf("cmd_csl os_msg_send failed\r\n");
                }
            }
            else if (g_csl_role == CSL_ROLE_ENDPOINT)
            {
                if (power_manager_interface_get_unit_status(PM_SLAVE_ZIGBEE, PM_UNIT_ZIGBEE) == PM_UNIT_ACTIVE)
                {
                    uint8_t *end;
                    uint32_t timestamp;
                    end = create_csl_msg(g_csl_buf.buf);
                    end = add_option_csl_data(end, g_csl_test_data, MAX_TEST_DATA);
                    PROTOCOL.send(g_csl_buf.buf, end - g_csl_buf.buf, &timestamp);
                    dbg_printf("send data to COORD\r\n");
                }
                else
                {
                    dbg_printf("can not send in inactive state\r\n");
                }
            }
            goto done;
        }
        else if (strcmp(argv[0], "sync") == 0)
        {
            g_csl_resync_req = value;
            goto done;
        }
        else if (strcmp(argv[0], "wakeup") == 0)
        {
            if (g_csl_role == CSL_ROLE_COORD)
            {
                g_csl_loop_test = FALSE;
                uint8_t event = CSL_EV_WAKEUP_REQ;
                if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
                {
                    dbg_printf("os_msg_send failed\r\n");
                }
            }
            else if (g_csl_role == CSL_ROLE_ENDPOINT)
            {
                g_csl_schedule_stop = TRUE;
                if (PROTOCOL.pm_init_wakeup)
                {
                    PROTOCOL.pm_init_wakeup();
                }
            }
            goto done;
        }
        else if (strcmp(argv[0], "sleep") == 0)
        {
            uint8_t event = CSL_EV_MAX;
            if (g_csl_role == CSL_ROLE_COORD)
            {
                event = CSL_EV_SLEEP_REQ;
            }
            else if (g_csl_role == CSL_ROLE_ENDPOINT && g_csl_schedule_stop == TRUE)
            {
                event = CSL_EV_SCHE_START;
            }
            if (event != CSL_EV_MAX)
            {
                if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
                {
                    dbg_printf("os_msg_send failed\r\n");
                }
            }
            goto done;
        }
        else if (strcmp(argv[0], "connect") == 0)
        {
            if (argc < 2)
            {
                dbg_printf("Error: InvalidArgs\r\n");
                goto done;
            }

            if (PROTOCOL.id == PROTOCOL_NONE)
            {
                dbg_printf("Error: Select protocol first\r\n");
                goto done;
            }
            addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
            // change address
            g_csl_peer_addr = addr;
            // send sleep command
            dbg_printf("connecting to 0x%x\r\n", g_csl_peer_addr);
            uint8_t event = CSL_EV_SWEEP;
            if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
            {
                dbg_printf("os_msg_send failed\r\n");
            }
            struct ep_info_s *ep = ep_info_find(g_csl_peer_addr);
            if (ep)
            {
                event = CSL_EV_SCHE_START;
            }
            else
            {
                event = CSL_EV_SLEEP_REQ;
            }
            if (!os_msg_send(g_csl_ev_queue_handle, &event, 0))
            {
                dbg_printf("os_msg_send failed\r\n");
            }
            goto done;
        }
        else if (strcmp(argv[0], "154") == 0)
        {
            p154_init();
            goto done;
        }
        else if (strcmp(argv[0], "fragment") == 0)
        {
            // add frag
            uint8_t *data;
            uint8_t *end;
            uint8_t m_flag = 1;
            uint8_t data_length = 0;
            uint16_t block_id = 0;
            uint32_t timestamp;
            uint16_t image_crc = 0x0000;
            if (argc < 2 || value > 10000)
            {
                dbg_printf("Error: The maximum image size must be less than 10k\r\n");
                goto done;
            }

            if (image_buffer_alloc(value) == NULL)
            {
                goto done;
            }

            /* Customize the transmission data here*/
            for (uint32_t i = 0; i < value; i++)
            {
                g_image[i] = i;
            }

            image_crc = btxfcs(image_crc, g_image, value);
            dbg_printf("image_crc = %x\r\n", image_crc);
            uint16_t total_block = (value + BLOCK_SIZE - 1) / BLOCK_SIZE;
            dbg_printf("start image transfer: total blocks %u now %u\r\n", total_block,
                       PROTOCOL.get_curr_timestamp());

            for (; block_id < total_block; block_id++)
            {
                if (block_id == (total_block - 1))
                {
                    m_flag = 0;
                }
                data = get_tx_frag_frm(g_image, value, block_id, BLOCK_SIZE, &data_length);
                end = create_csl_msg(g_csl_buf.buf);
                end = add_option_csl_frag(end, data, image_crc, value, m_flag, block_id, data_length);
                if (g_csl_debug == TRUE)
                {
                    dbg_mem_dump(g_csl_buf.buf, end - g_csl_buf.buf);
                }
                for (int i = 0; i < 3; i++)
                {
                    if (g_csl_debug == TRUE)
                    {
                        dbg_printf("send block %u\r\n", block_id);
                    }
                    if (PROTOCOL.send(g_csl_buf.buf, end - g_csl_buf.buf, &timestamp) == TRUE)
                    {
                        //dbg_printf("send successful!\r\n");
                        break;
                    }
                }
            }
            dbg_printf("transfer completed. now %u\r\n", PROTOCOL.get_curr_timestamp());
            if (g_image)
            {
                os_mem_free(g_image);
                g_image_size = 0;
                g_image = NULL;
            }
            goto done;
        }
        else
        {
            role = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
            if (role >= CSL_ROLE_INVALID)
            {
                dbg_printf("Error: InvalidArgs\r\n");
                return FALSE;
            }
        }
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    if (PROTOCOL.id == PROTOCOL_NONE)
    {
        dbg_printf("Error: Select protocol first\r\n");
        return FALSE;
    }

    if (csl_start_task(&value, &listen_window, role, &addr) == FALSE)
    {
        dbg_printf("Error: Task is running\r\n");
        return FALSE;
    }
done:
    dbg_printf("Done\r\n");
    return TRUE;
}

static int cmd_csl_config(int argc, char *argv[])
{
    int value;
    if (argc == 2)
    {
        value = _strtol((const char *)(argv[1]), (char **)NULL, 10);
        if (strcmp(argv[0], "ppm") == 0)
        {
            g_csl_ppm = value;
            g_csl_period = g_csl_conf_period + ((int)g_csl_conf_period * g_csl_ppm / PPM);
        }
        else if (strcmp(argv[0], "p2r") == 0)
        {
            g_csl_prepare_to_rx = value;
        }
        else if (strcmp(argv[0], "txc") == 0)
        {
            g_csl_tx_check_offest = value;
        }
        else if (strcmp(argv[0], "debug_t") == 0)
        {
            g_csl_debug_timing = value;
        }
        else if (strcmp(argv[0], "debug") == 0)
        {
            g_csl_debug = value;
        }
        else if (strcmp(argv[0], "ack") == 0)
        {
            g_csl_ack_require_time = value;
        }
        else if (strcmp(argv[0], "autosync") == 0)
        {
            g_csl_autosync_period = value;
        }
        else if (strcmp(argv[0], "loop") == 0)
        {
            g_csl_loop_test = value;
        }
        else if (strcmp(argv[0], "shortaddr") == 0)
        {
            uint32_t addr = _strtoul((const char *)(argv[1]), (char **)NULL, 16);
            save_cfg_shortaddr(addr);
            dbg_printf("save_cfg_shortaddr 0x%02x\r\n", addr);
            return TRUE;
        }
    }
    dbg_printf("sche_state %u\r\n", g_csl_sche_state);
    dbg_printf("sche_dbg_counter %u\r\n", g_csl_dbg_counter);
    dbg_printf("schedule_stop %u\r\n", g_csl_schedule_stop);
    dbg_printf("loop %u\r\n", g_csl_loop_test);
    dbg_printf("autosync %u\r\n", g_csl_autosync_period);
    dbg_printf("ppm %d\r\n", g_csl_ppm);
    dbg_printf("p2r %u\r\n", g_csl_prepare_to_rx);
    dbg_printf("txc %d\r\n", g_csl_tx_check_offest);
    dbg_printf("ack %u\r\n", g_csl_ack_require_time);
    dbg_printf("debug_t %u\r\n", g_csl_debug_timing);
    dbg_printf("debug %u\r\n", g_csl_debug);
    dbg_printf("g_csl_period %u\r\n", g_csl_period);
    dbg_printf("Done\r\n");
    return TRUE;
}

void shell_register_user_cmd(void)
{
    shell_register((shell_program_t)cmd_csl, "csl",
                   BRIEF("proprietary csl implementation")
                   SYNOPSIS("csl <role> [<csl_period>] [<shortaddr>] [<listen window>]")
                   DESCRIPTION("    role: 0/1 - Coordinator/Endpoint")
                   DESCRIPTION("    shortaddr: 0x0001 ~ 0xfffe")
                   DESCRIPTION("    csl_period: 1 ~ 4294967295 (us) - default 3000000")
                   DESCRIPTION("    listen window: 1 ~ 4294967295 (us) - default 4000")
                   SYNOPSIS("csl connect <shortaddr>")
                   DESCRIPTION("    shortaddr: 0x0001 ~ 0xfffe")
                   SYNOPSIS("csl <wakeup/sleep/sync>")
                   DESCRIPTION("    wakeup: wakeup a sleep endpoint")
                   DESCRIPTION("    sleep: let endpoint go to sleep")
                   DESCRIPTION("    sync: send re-sync. request to endpoint")
                   SYNOPSIS("csl data <string>")
                   DESCRIPTION("    string: no longer than 14 character")
                   SYNOPSIS("csl fragment <length>")
                   DESCRIPTION("    length: bulk data length")
                   SYNOPSIS("")
                   EXAMPLE("csl 154                - use IEEE-802.15.4 as transport protocol")
                   EXAMPLE("csl 24g                - use proprietary 2.4G as transport protocol")
                   EXAMPLE("csl 0                  - start as coordinator")
                   EXAMPLE("csl 1 6000000 0x1 7000 - start as endpoint")
                   EXAMPLE("csl data hello         - send test data to endpoint")
                   EXAMPLE("csl fragment 8000      - send bulk data (8k) to endpoint")
                  );

    shell_register((shell_program_t)cmd_csl_config, "csl_config",
                   BRIEF("csl configuration")
                   SYNOPSIS("csl_config [<shortaddr/loop/autosync/ppm/p2r/txc/ack/debug_t/debug> <value>]")
                   DESCRIPTION("shortaddr:0x0000 ~ 0xffff   - save short address in cofiguration data")
                   DESCRIPTION("loop:     1/0               - enable/disable send test data in loop")
                   DESCRIPTION("autosync: 10000000 (us)     - the period of auto sync")
                   DESCRIPTION("ppm:      -50 ~ 50          - adjust endpoint clock (only for endpoint)")
                   DESCRIPTION("p2r:      0 ~ 2000 (us)     - wake up early and prepare for RX (only for endpoint)")
                   DESCRIPTION("txc:      -2000 ~ 2000 (us) - adjust tx check point (only for coordinator)")
                   DESCRIPTION("ack:      0 ~ 1000 (us)     - extend listen window to ensure ack reply (only for endpoint)")
                   DESCRIPTION("debug_t:  1/0               - enable/disable timing debug")
                   DESCRIPTION("debug:    1/0               - enable/disable debug"));
    //p154_init(); // for auto start
}
