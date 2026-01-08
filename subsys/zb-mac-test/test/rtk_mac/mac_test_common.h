/**************************************************************************//**
* @file     mac_test_common.h
* @brief    Include file for IEEE 802.15.4 MAC test common function
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


#ifndef _MAC_TEST_COMMON_H_
#define _MAC_TEST_COMMON_H_

#include "mac_test_config.h"
#include "frame_802154.h"
#if (TEST_ZB_PM_EN == 1)
#include "power_manager_unit_zbmac.h"
#endif
#include "mac_driver_interface.h"

//++++++++++++++++++++++++++++++++++++++++++++++++
// type and macro define
//------------------------------------------------
#define TX_NONE         0
#define TX_SUCCESS      1
#define TX_BUSY         2
#define TX_NOACK        3
#define TX_AT_FAIL      4
#define TX_TERMED       5
#define TX_SUCCESS_MASK (1 << 0)
#define TX_BUSY_MASK    (1 << 1)
#define TX_NOACK_MASK   (1 << 2)
#define TX_AT_FAIL_MASK (1 << 3)
#define TX_TERMED_MASK  (1 << 4)

#define ADDR_MODE_NOT_PRESENT   0
#define ADDR_MODE_RSV           1
#define ADDR_MODE_SHORT         2
#define ADDR_MODE_EXTEND        3
#define ADDR_MODE_MAX           4

#define SEC_NONE        0
#define SEC_MIC_32      1
#define SEC_MIC_64      2
#define SEC_MIC_128     3
#define SEC_ENC         4
#define SEC_ENC_MIC_32  5
#define SEC_ENC_MIC_64  6
#define SEC_ENC_MIC_128 7

#define KEY_ID_MODE_0_NOOFFSET  0
#define KEY_ID_MODE_1_NOOFFSET  1
#define KEY_ID_MODE_2_NOOFFSET  2
#define KEY_ID_MODE_3_NOOFFSET  3

#define TX_LOOP_STOP 0
#define TX_LOOP_CONT 1

#define TX_MODE_NORMAL  0
#define TX_MODE_SCHEDULE    1
#define TX_SCHEDULE_DEFER   0
#define TX_SCHEDULE_TARGET  1

#define LOOP_REPORT_NONE 0x0    // no report
#define LOOP_REPORT_FIRST 0x1   // first report
#define LOOP_REPORT_FINAL 0x4   // final report
#define LOOP_REPORT_ALL 0x7     // report every time
#define TX_BUF_MAX_LEN 127
#define NV_FIELD 0 // Not Available Field

#define BRIEF(name) "\t"name"\r\n"
#define SYNOPSIS(synopsis) "\t"synopsis"\r\n"
#define DESCRIPTION(description) "\t"description"\r\n"
#define EXAMPLE(example) "\te.g. "example"\r\n"

#define CPY_MV_PTR(dst, src, size, ptr) \
    { \
        if ((size) != 0) { \
            void *__ptr = (void *)(src); \
            if (__ptr) { \
                mac_memcpy(dst, __ptr, size); \
            } else { \
                memset(dst, 0, size); \
            } \
            ptr += (size); \
        } \
    }

#define GEN_SEQ_DATA_MV_PTR(buf, start, size, ptr) \
    { \
        for (uint16_t __i = start; __i < (start + size); __i++) \
            buf[ptr++] = __i; \
    }

#define WAIT_FOR_TXDOWN() {while (g_tx_done == TX_NONE);}

#define WAIT_FOR_TXDOWN_UNTIL(cond) \
    { \
        do { \
            if (cond) \
                break; \
        } while (g_tx_done == TX_NONE); \
    }

#define RESET_TXDOWN() {g_tx_done = TX_NONE;}

#define BUF_RESET(_buf) \
    { \
        memset(_buf.buf, 0, TX_BUF_MAX_LEN); \
        _buf.len = 0; \
    }

#define MAC_RX_PKT_LEN(buf) (buf[0] - 2)
#define MAC_RX_PKT(buf) ((uint8_t *)&buf[1])
#define MAC_RX_PKT_CRC(buf) ((buf[buf[0] - 1]) | (buf[buf[0]] << 8))
#define MAC_RX_PKT_INFO(buf) ((mac_rxfifo_tail_t *)&buf[buf[0] + 1])
#define MAC_RX_PKT_LQI(buf) (MAC_RX_PKT_INFO(buf)->lqi)
#define CONVERT_154CH_TO_BTCHIDX(ch) (((ch) - 10) * 5 - 2) // the input parameter channel is BT's channel index, which is started from 2402 MHz
#define MAC_RX_PKT_RSSI(buf) (mac_rssi_get(MAC_RX_PKT_INFO(buf)->rssi, CONVERT_154CH_TO_BTCHIDX(mac_channel_get())))

extern uint64_t bt_clk_offset;
#define MAC_RX_PKT_TIMESTAMP(buf) (bt_clk_offset + mac_btclk_to_us(MAC_RX_PKT_INFO(buf)->bt_time))

typedef struct fc_s
{
    uint16_t type: 3;
    uint16_t sec_en: 1;
    uint16_t pending: 1;
    uint16_t ack_req: 1;
    uint16_t panid_compress: 1;
    uint16_t rsv: 1;
    uint16_t seq_num_suppress: 1;
    uint16_t ie_present: 1;
    uint16_t dst_addr_mode: 2;
    uint16_t ver: 2;
    uint16_t src_addr_mode: 2;
} __attribute__((packed)) fc_t;

typedef struct ss_s
{
    uint16_t bo: 4;
    uint16_t so: 4;
    uint16_t final_cap_slot: 4;
    uint16_t ble: 1;
    uint16_t rsv: 1;
    uint16_t pan_coord: 1;
    uint16_t assoc_permit: 1;
} __attribute__((packed)) ss_t;

typedef struct gts_spec_s
{
    uint8_t gts_desc_cnt: 3;
    uint8_t rsv: 4;
    uint8_t gts_permit: 1;
} gts_spec_t;

typedef struct pendaddr_spec_s
{
    uint8_t saddr_pend_num: 3;
    uint8_t rsv0: 1;
    uint8_t extaddr_pend_num: 3;
    uint8_t rsv1: 1;
} pendaddr_spec_t;

typedef struct aux_sec_ctl_s
{
    uint8_t sec_level: 3;
    uint8_t key_id_mode: 2;
    uint8_t rsv: 3;
} aux_sec_ctl_t;

typedef struct aux_s
{
    aux_sec_ctl_t sec_ctl;
    uint32_t frame_counter;
    uint8_t key_id;
} __attribute__((packed)) aux_t;

typedef struct nonce_s
{
    uint8_t sec_level;
    uint32_t frame_counter;
    uint64_t src_ext_addr;
} __attribute__((packed)) nonce_t;

typedef struct tx_buf_s
{
    uint8_t buf[TX_BUF_MAX_LEN];
    uint8_t len;
} tx_buf_t, buf_t;

typedef struct txl_ctrl_info_s
{
    uint8_t hdr_len;
    uint8_t prt_tx_mask;        // print tx result mask
    uint8_t fix_seq : 1;        // 0: disable, 1: enable
    uint8_t dump_pkt : 1;       // 0: disable, 1: enable
    uint8_t dump_enc_pkt : 1;   // 0: disable, 1: enable
    uint8_t tx_mode : 1;        // 0: TX_MODE_NORMAL, 1: TX_MODE_SCHEDULE
    uint8_t mpan_enabled : 1;   // 0: disable, 1: enable
    uint8_t mpan_idx : 2;       // pan index: 0~3
    uint8_t schedule_mode : 1;  // 0: deferred time, 1: target time
    uint8_t manual_tx_lock : 1; // 0: auto, 1: manual
    uint8_t rsvd : 7;
    uint32_t schedule_time;     // for TX_MODE_SCHEDULE
} txl_ctrl_info_t;

typedef uint8_t (*loop_check_cb)(uint32_t count);
typedef int (*loop_exec_cb)(uint32_t *exec_result, uint32_t count, uint32_t ctrl_info);
typedef void (*loop_report_cb)(uint32_t count, uint32_t exec_info);

//++++++++++++++++++++++++++++++++++++++++++++++++
// global variable
//------------------------------------------------
extern mac_attribute_t g_mac_attribute;
extern const uint8_t ADDR_MODE2LEN[ADDR_MODE_MAX];
extern volatile uint32_t g_recent_rx_timestamp;

extern volatile uint32_t g_tx_done;
extern tx_buf_t g_tx_buf;
extern uint8_t g_mac_key[16];

extern volatile bool g_tx_loop_state;
extern uint32_t g_tx_interval_ms;
extern bool g_enh_ack_early;
extern bool g_test_enh_ack_late;

extern void *zb_sem;
extern void *zb_task_handle;

#if (TEST_ZB_PM_EN == 1)
extern int g_zbpm_wakeup_diff_min;
extern int g_zbpm_wakeup_diff_max;
extern int g_zbpm_wakeup_diff_avg;
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++
// public function
//------------------------------------------------
#if (TEST_GET_IC_EUID == 1)
uint64_t get_ic_eui64(void);
#endif
void clear_tx_stat(void);
void clear_rx_stat(void);
void zb_mac_interrupt_enable(void);
void zb_mac_drv_enable(void);
void zb_mac_drv_init(void);
void dbg_mem_dump(const uint8_t *addr, uint32_t len);
int32_t edscan_lv2dbm(int32_t level);
uint8_t parse_digit(char c);
void print_tx_result(uint8_t val, uint8_t prt_tx_mask);
void print_rx_packet(uint8_t *rx_data, uint8_t pan_idx);
uint8_t txl_check(uint32_t count);
int txl_exec(uint32_t *exec_result, uint32_t count, uint32_t ctrl_info);
#if TEST_AUTO_TEST_EN
void txl_report(uint32_t count, uint32_t exec_info);
#else
#define txl_report NULL
#endif
void loop_ctrl(uint32_t count, loop_check_cb check_cb, loop_exec_cb exec_cb,
               loop_report_cb report_cb, uint8_t report_mode, uint32_t interval,
               uint32_t ctrl_info);
uint16_t generate_ieee_frame(uint8_t frm_type,
                             uint8_t *buf, uint16_t buf_len,
                             fc_t fc, uint8_t seq,
                             uint16_t spid, const uint8_t *saddr,
                             uint16_t dpid, const uint8_t *daddr,
                             const uint8_t *aux, uint16_t aux_len,
                             uint8_t cmd_id, const void *ss);
#if (TEST_ZB_PM_EN == 1)
int zbmac_power_manager_set(uint32_t next, uint32_t period);
void zbmac_power_manager_init(zbpm_callback_t exit_callback);
#endif

#if TEST_GPIO_DBG_EN
void debug_gpio_init(void);
void debug_gpio_toggle(uint32_t GPIO_Pin);
void debug_gpio_high(uint32_t GPIO_Pin);
void debug_gpio_low(uint32_t GPIO_Pin);
uint8_t debug_gpio_get(uint32_t GPIO_Pin);
#endif

typedef void (*work_queue_task_cb_t)(void *);
void work_queue_task_init(void);
void work_queue_msg_send(work_queue_task_cb_t callback, void *arg);
extern void *work_queue_task_handle;  // Work queue task handle for debugging
#endif /* _MAC_TEST_COMMON_H_ */
