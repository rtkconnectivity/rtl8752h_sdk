/**************************************************************************//**
 * @file     mac_test_common.c
 * @brief    Source file for IEEE 802.15.4 MAC test common function
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
#include <limits.h>
#include "platform_port.h"
#include "shell.h"
#include "dbg_printf.h"
#include "trace.h"
#include "vector_table.h"
#include "mac_test_common.h"

#if (TEST_ZB_PM_EN == 1)
#include "power_manager_unit_zbmac.h"
#include "power_manager_slave.h"
#include "power_manager_interface.h"
#endif
#include "mac_driver_interface.h"
#include "mac_stats.h"
#include "lib/ringbufindex.h"
#include "services/auto_test.h"
#if (TEST_PROTOTHREAD_EN == 1)
#include "shell/pt_cmd.h"
#if (TEST_FPGA_DBG_PORT_EN == 1)
#include "debug_port.h"
#endif
#endif
#include "net/pt_mac.h"

#if TEST_ATCMD_ZIGBTEST
// Centralized AT+ZIGBTEST module
#include "../../at_zigbtest/at_zigbtest_module.h"
// Include MP test types and constants
#include "zb_mp_test.h"
#endif

/* Constant Global Variable */
const uint8_t ADDR_MODE2LEN[ADDR_MODE_MAX] = {0, 0, 2, 8};
const uint8_t g_default_mac_key[16] =
{
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf
};

//++++++++++++++++++++++++++++++++++++++++++++++++
// global variable
//------------------------------------------------
/* MAC Global Variable */
mac_attribute_t g_mac_attribute;
static mac_driver_t g_mac_driver;
static pan_mac_comm_t g_pan_mac_comm;
static pan_mac_t pan_mac[TEST_MAX_PAN_NUM];

/* mac power management */
#if (TEST_ZB_PM_EN == 1)
static uint8_t g_zbpm_inited;
static zbpm_adapter_t g_zbpm_adap;
#endif

/* interface statistic */
volatile uint32_t g_recent_rx_timestamp;

/* tx state and global tx buffer */
volatile uint32_t g_tx_done;
tx_buf_t g_tx_buf;

/* mac test key */
uint8_t g_mac_key[16];

/* additional test control flag */
volatile bool g_tx_loop_state;
uint32_t g_tx_interval_ms;
bool g_enh_ack_early;
bool g_test_enh_ack_late;

//++++++++++++++++++++++++++++++++++++++++++++++++
// global function
//------------------------------------------------
void dbg_mem_dump(const uint8_t *addr, uint32_t len)
{
    for (int i = 0; i < len; i++)
    {
        dbg_printf("%02x ", addr[i]);
        if ((i + 1) % 16 == 0)
        {
            dbg_printf("\r\n");
        }
    }
    dbg_printf("\r\n");
}

int32_t edscan_lv2dbm(int32_t level)
{
    return (level << 1) - 90;
}

uint8_t parse_digit(char c)
{
    if (('A' <= c) && (c <= 'F'))
    {
        return (c - 'A' + 10);
    }
    if (('a' <= c) && (c <= 'f'))
    {
        return (c - 'a' + 10);
    }
    if (('0' <= c) && (c <= '9'))
    {
        return (c - '0');
    }
    return 0xff;
}

void print_tx_result(uint8_t val, uint8_t prt_tx_mask)
{
    if (prt_tx_mask)
    {
        switch (g_tx_done)
        {
        case TX_SUCCESS:
            if (prt_tx_mask & TX_SUCCESS_MASK)
            {
                dbg_printf("%u success\r\n", val);
            }
            break;
        case TX_BUSY:
            if (prt_tx_mask & TX_BUSY_MASK)
            {
                dbg_printf("%u busy\r\n", val);
            }
            break;
        case TX_NOACK:
            if (prt_tx_mask & TX_NOACK_MASK)
            {
                dbg_printf("%u noack\r\n", val);
            }
            break;
        case TX_AT_FAIL:
            if (prt_tx_mask & TX_AT_FAIL_MASK)
            {
                dbg_printf("%u txat fail\r\n", val);
            }
            break;
        case TX_TERMED:
            if (prt_tx_mask & TX_TERMED_MASK)
            {
                dbg_printf("%u terminated\r\n", val);
            }
            break;
        default:
            dbg_printf("%u undefined error. g_tx_done = %u\r\n", val, g_tx_done);
            break;
        }
    }
}

void print_rx_packet(uint8_t *rx_data, uint8_t pan_idx)
{
    uint8_t *buf = MAC_RX_PKT(rx_data);
    uint8_t buf_len = MAC_RX_PKT_LEN(rx_data);
    //uint16_t crc = MAC_RX_PKT_CRC(rx_data);
    uint8_t lqi = MAC_RX_PKT_LQI(rx_data);
    int8_t rssi = MAC_RX_PKT_RSSI(rx_data);
    //uint64_t rx_timestamp = MAC_RX_PKT_TIMESTAMP(rx_data);

    for (uint32_t i = 0; i < buf_len; i++)
    {
        dbg_printf("%02x ", buf[i]);
    }

    dbg_printf("LEN %u LQI %u RSSI %d PAN_IDX %u\r\n", buf_len, lqi, rssi, pan_idx);
}

uint8_t txl_check(uint32_t count)
{
    RESET_TXDOWN();
    return g_tx_loop_state;
}

static uint8_t mac_TrigTxNDelay(uint8_t ackreq, uint8_t secreq, uint8_t frm_ver,
                                txl_ctrl_info_t *info,
                                uint32_t *base_us)
{

    if (info->tx_mode == TX_MODE_SCHEDULE)
    {
        uint32_t target_us;
        if (info->schedule_mode == TX_SCHEDULE_DEFER)
        {
            target_us = mac_btus_get();
            target_us += info->schedule_time;
            if (base_us)
            {
                *base_us = (target_us % MAX_BT_CLOCK_COUNTER);
            }
        }
        else     // TX_SCHEDULE_TARGET
        {
            target_us = info->schedule_time;
        }
        uint8_t do_cca = (mac_cca_mode_get() == MAC_CCA_NONE) ? 0 : 1;
#if TEST_MPAN_EN
        if (info->mpan_enabled)
        {
            return mpan_TrigTxNAtUS(ackreq, secreq, do_cca, target_us, info->mpan_idx);
        }
        else
        {
            return mac_txn_trig_at_us(ackreq, secreq, do_cca, target_us);
        }
#else
        return mac_txn_trig_at_us(ackreq, secreq, do_cca, target_us);
#endif
    }
    else // TX_MODE_NORMAL
    {
#if TEST_MPAN_EN
        if (info->mpan_enabled)
        {
            return mpan_TrigTxN(ackreq, secreq, info->mpan_idx);
        }
        else
        {
            return mac_txn_trig(ackreq, secreq);
        }
#else
        return mac_txn_trig(ackreq, secreq);
#endif
    }
}

// return 0 success, otherwise fail
int txl_exec(uint32_t *exec_result, uint32_t count, uint32_t ctrl_info)
{
    txl_ctrl_info_t *info = (txl_ctrl_info_t *)ctrl_info;
    fc_t *fc = (fc_t *)g_tx_buf.buf;
    uint8_t *seq = &g_tx_buf.buf[sizeof(fc_t)];

    if (g_tx_buf.len < sizeof(fc_t))
    {
        return -1;
    }

    if (!info->fix_seq && count)
    {
        (*seq)++;
    }

    if (info->manual_tx_lock == 0)
    {
#if TEST_MPAN_EN
        mpan_mac_lock(info->mpan_idx);
#else
        mpan_mac_lock(0);
#endif
    }

    mac_txn_payload_set(info->hdr_len, g_tx_buf.len, g_tx_buf.buf);
    if (info->dump_pkt)
    {
        dbg_printf("MAC_TX: ");
        //dbg_mem_dump((const uint8_t *)g_tx_buf.buf, g_tx_buf.len);
        for (uint32_t i = 0; i < g_tx_buf.len; i++)
        {
            dbg_printf("%02x ", g_tx_buf.buf[i]);
        }
        dbg_printf("LEN %u PAN_IDX %u\r\n", g_tx_buf.len, info->mpan_idx);
    }
    if (fc->sec_en)
    {
        RESET_TXDOWN();
        mac_upper_enc_trig();
        if (info->dump_enc_pkt)
        {
            pmac_txfifo_t pTxNFIFO = (pmac_txfifo_t)MAC_TXN_BASE_ADDR;
            dbg_mem_dump(pTxNFIFO->payload, g_tx_buf.len);
            uint8_t mic_len = pTxNFIFO->frm_len - g_tx_buf.len;
            if (mic_len)
            {
                dbg_printf("MIC-");
                dbg_mem_dump(pTxNFIFO->payload + g_tx_buf.len, mic_len);
            }
        }
    }
    RESET_TXDOWN();
    uint8_t tx_result = mac_TrigTxNDelay(fc->ack_req, false, fc->ver, info,
                                         exec_result);

    if (tx_result == MAC_STS_SUCCESS)
    {
        WAIT_FOR_TXDOWN_UNTIL(info->manual_tx_lock == 1);

        if (info->manual_tx_lock == 0)
        {
            mpan_mac_unlock();
        }

        if (info->prt_tx_mask)
        {
            print_tx_result(*seq, info->prt_tx_mask);
        }
    }
    else
    {
        if (info->manual_tx_lock == 0)
        {
            mpan_mac_unlock();
        }
        dbg_printf("mac_TrigTxNDelay fail %u\r\n", tx_result);
        return -1;
    }
    return 0;
}

#if TEST_AUTO_TEST_EN
void txl_report(uint32_t count, uint32_t exec_info)
{
    unsigned char bytes[] = {0x04, 0x0e, 0x06, 0x02, 0x00, 0xfc, 0x01, 0x01, 0x03};
    UART_SendData(ZB_DBG_UART, bytes, sizeof(bytes));
}
void cmd_data_report(uint32_t count, uint32_t exec_info)
{
    dbg_printf("report %u exec_info %u\r\n", count, exec_info);
    uint32_t trig_time = mac_txn_timestamp_get();
    unsigned char bytes[] = {0x04, 0x0e, 0x0c, 0x08, 0x00, 0xfc, 0x01,
                             exec_info >> 24, exec_info >> 16, exec_info >> 8, exec_info,
                             trig_time >> 24, trig_time >> 16, trig_time >> 8, trig_time
                            };
    UART_SendData(ZB_DBG_UART, bytes, sizeof(bytes));
}
#endif

/**
*
* @fn void loop_ctrl(uint32_t count, loop_check_cb check_cb,
*                   loop_exec_cb exec_cb, loop_report_cb report_cb,
*                   uint8_t report_mode, uint32_t interval, uint32_t ctrl_info)
*
* @brief Provide an event loop control process.
*
* @param count loop count. 0 is infinite loop
*
* @param check_cb user defined callback will be executed at the end of each loop
*
* @param exec_cb user defined callback will be executed at the beginning of each loop
*
* @param report_cb user defined callback will be executed after exec_cb
*
* @param report_mode could be LOOP_REPORT_NONE, LOOP_REPORT_FIRST, LOOP_REPORT_FINAL, LOOP_REPORT_ALL
*
* @param interval interval between each loop
*
* @param ctrl_info additional information used in loop_ctrl
*
* @return None
*
*/
void loop_ctrl(uint32_t count, loop_check_cb check_cb,
               loop_exec_cb exec_cb, loop_report_cb report_cb,
               uint8_t report_mode, uint32_t interval, uint32_t ctrl_info)
{
    uint32_t i, exec_info = 0;

    g_tx_loop_state = TX_LOOP_CONT;
    for (i = 0;; i++)
    {
        if (exec_cb)
        {
            exec_cb(&exec_info, i, ctrl_info);
        }

        if (report_cb)
        {
            if (report_mode & LOOP_REPORT_FIRST)
            {
                report_mode &= ~LOOP_REPORT_FIRST;
                if (count == 1)
                {
                    report_mode &= ~LOOP_REPORT_FINAL;
                }
                report_cb(i, exec_info);
            }
            else if (report_mode & 0x2)
            {
                report_cb(i, exec_info);
            }
        }
        if (interval)
        {
            mac_btus_intr_set(MAC_BT_TIMER0, mac_btus_get() + interval * 1000);
            osif_sem_take(zb_sem, 0xffffffff);
        }
        // terminate condition
        if ((count && ((i + 1) == count)) || (check_cb && check_cb(i) == TX_LOOP_STOP))
        {
            break;
        }
    }
    if (report_cb && (report_mode & LOOP_REPORT_FINAL) && (report_mode & 0x2) == 0)
    {
        report_cb(i, exec_info);
    }
    RESET_TXDOWN();
    g_tx_loop_state = TX_LOOP_STOP;
}

__attribute__((section(".app.ram.text")))
uint16_t generate_ieee_frame(uint8_t frm_type,
                             uint8_t *buf, uint16_t buf_len,
                             fc_t fc, uint8_t seq,
                             uint16_t spid, const uint8_t *saddr,
                             uint16_t dpid, const uint8_t *daddr,
                             const uint8_t *aux, uint16_t aux_len,
                             uint8_t cmd_id, const void *ss)
{
    uint16_t off = 0;

    /* -----------------------------------------------------------------------
     * Step 1. Determine PAN ID compression using LUT
     * -------------------------------------------------------------------- */
    if (fc.ver == FRAME_VER_2015)
    {
        const uint8_t dp = (dpid > 0);
        const uint8_t sp = (spid > 0);
        fc.panid_compress = panid_compress_v10_LUT[fc.dst_addr_mode][fc.src_addr_mode][dp][sp];
    }
    else
    {
        const uint8_t same_pan = (dpid == spid) ? 1 : 0;
        fc.panid_compress = panid_compress_v00_01_LUT[fc.dst_addr_mode][fc.src_addr_mode][same_pan];
    }

    /* -----------------------------------------------------------------------
     * Step 2. Write Frame Control + Seq
     * -------------------------------------------------------------------- */
    if (off + 2 > buf_len) { return 0; }
    wr16(buf + off, *((uint16_t *)&fc)); off += 2;

    if (!fc.seq_num_suppress)
    {
        //if (off + 1 > buf_len) return 0;
        buf[off++] = seq;
    }

    /* -----------------------------------------------------------------------
     * Step 3. Addressing fields
     * -------------------------------------------------------------------- */
    const uint8_t dlen = ADDR_MODE2LEN[fc.dst_addr_mode];
    const uint8_t slen = ADDR_MODE2LEN[fc.src_addr_mode];

    if (fc.ver == FRAME_VER_2015)
    {
        if (dpid > 0) { wr16(buf + off, dpid); off += 2; }
        for (uint8_t i = 0; i < dlen; i++) { buf[off++] = daddr[i]; }
        if (spid > 0) { wr16(buf + off, spid); off += 2; }
        for (uint8_t i = 0; i < slen; i++) { buf[off++] = saddr[i]; }
    }
    else
    {
        if (fc.dst_addr_mode > 0 && fc.src_addr_mode > 0)
        {
            wr16(buf + off, dpid); off += 2;
            for (uint8_t i = 0; i < dlen; i++) { buf[off++] = daddr[i]; }
            if (!fc.panid_compress) { wr16(buf + off, spid); off += 2; }
            for (uint8_t i = 0; i < slen; i++) { buf[off++] = saddr[i]; }
        }
        else
        {
            if (fc.dst_addr_mode > 0)
            {
                if (dpid > 0) { wr16(buf + off, dpid); off += 2; }
                for (uint8_t i = 0; i < dlen; i++) { buf[off++] = daddr[i]; }
            }
            if (fc.src_addr_mode > 0)
            {
                if (spid > 0) { wr16(buf + off, spid); off += 2; }
                for (uint8_t i = 0; i < slen; i++) { buf[off++] = saddr[i]; }
            }
        }
    }

    /* -----------------------------------------------------------------------
     * Step 4. Security Header
     * -------------------------------------------------------------------- */
    if (fc.sec_en && aux && aux_len)
    {
        for (uint16_t i = 0; i < aux_len; i++)
        {
            buf[off++] = aux[i];
        }
    }

    /* -----------------------------------------------------------------------
     * Step 5. Payload extras (Beacon / Command)
     * -------------------------------------------------------------------- */
    if (frm_type == FRAME_TYPE_COMMAND)
    {
        buf[off++] = cmd_id;
    }
    else if (frm_type == FRAME_TYPE_BEACON)
    {
        if (ss)
        {
            const uint8_t *sp = (const uint8_t *)ss;
            for (uint8_t i = 0; i < sizeof(ss_t); i++) // generic superframe size
            {
                buf[off++] = sp[i];
            }
        }
        buf[off++] = 0; /* GTS info */
        buf[off++] = 0; /* Pending addr */
    }

    return off;
}

static tx_buf_t g_enh_ack_buf;

//TXNTERRIF

// TXNIF
__attribute__((section(".app.ram.text")))
static void txn_handler(uint8_t pan_idx, uint32_t arg)
{
    uint32_t tx_status = mac_txn_status_get();
    uint32_t txat_status = mac_txat_status_get();
    uint32_t txterm_status = mac_tx_termed_status_get();

#if TEST_ATCMD_ZIGBTEST
    // Delegate to centralized AT+ZIGBTEST module hook
    at_zigbtest_txn_handler_hook(pan_idx, tx_status, txat_status, txterm_status);
#endif

    // Normal MAC TX handling
    if (txat_status)
    {
        g_tx_done = TX_AT_FAIL;
        mac_stats_tx_fail_notime_inc(pan_idx);
    }
    else if (tx_status & 0x1)
    {
        if (tx_status & 0x20)
        {
            g_tx_done = TX_BUSY;
            mac_stats_tx_fail_cca_fail_inc(pan_idx);
        }
        else if (txterm_status & 0x4) //normal tx
        {
            g_tx_done = TX_TERMED;
            mac_stats_tx_fail_phy_grant_inc(pan_idx);
        }
        else
        {
            g_tx_done = TX_NOACK;
            mac_stats_tx_fail_no_ack_inc(pan_idx);
        }
    }
    else
    {
        g_tx_done = TX_SUCCESS;
        mac_stats_tx_succ_total_inc(pan_idx);
    }
#if (TEST_PROTOTHREAD_EN == 1)
    NETSTACK_MAC.sent();
#endif
}

// TXG1IF

// TXG2IF

/**
 * @brief Prepare an Enhanced ACK frame based on the last received MAC frame.
 *
 * This function constructs an IEEE 802.15.4-2015 Enhanced ACK frame.
 * It retrieves header/security information from the last parsed RX frame
 * (g_parsed_rx) and fills g_enh_ack_buf with the corresponding ACK payload.
 *
 * @param pan_idx  Index of the PAN context for retrieving PAN ID and keys.
 */
__attribute__((section(".app.ram.text")))
static inline void mac_enhanced_ack_prepare(uint8_t pan_idx)
{
    static uint32_t sMacFrameCounter = 0;
    uint16_t panid = 0;
    uint16_t saddr = 0;
    uint64_t laddr = 0;
    uint8_t *daddr = NULL;

    fc_t fc_ack = {0};
    aux_t aux   = {0};

    // ----------------------------------------------------
    // 1. Build Frame Control for Enhanced ACK
    // ----------------------------------------------------
    fc_ack.type             = FRAME_TYPE_ACK;
    fc_ack.ver              = FRAME_VER_2015;                   // Enhanced ACK (2015)
    fc_ack.sec_en           = mac_rx_frm_sec_en_get();          // Copy Security Enable bit
    fc_ack.seq_num_suppress = mac_rx_frm_seq_compress_get();    // Sequence number suppression
    fc_ack.dst_addr_mode    =
        mac_rx_frm_src_addr_mode_get();   // Destination = Source of received frame
    fc_ack.ie_present       = 1;

    // Set Frame Pending bit if there is a source match hit
    if (mac_GetSrcMatchStatus())
    {
        mac_ClrSrcMatchStatus();
        fc_ack.pending = 1;
    }

    // ----------------------------------------------------
    // 2. Construct Auxiliary Security Header if enabled
    // ----------------------------------------------------
    if (fc_ack.sec_en)
    {
        aux.sec_ctl.sec_level   = mac_rx_frm_sec_level_get();
        aux.sec_ctl.key_id_mode = mac_rx_frm_sec_keyid_mode_get();
        aux.frame_counter       = sMacFrameCounter++;
        aux.key_id              = mac_rx_frm_sec_keyid_get();
    }

    // ----------------------------------------------------
    // 3. Determine PAN ID field
    // ----------------------------------------------------
    if (!mac_rx_frm_panid_compress_get())
    {
        panid = mpan_GetPANId(pan_idx);
    }

    // ----------------------------------------------------
    // 4. Determine Destination Address (from RX source)
    // ----------------------------------------------------
    if (fc_ack.dst_addr_mode == ADDR_MODE_SHORT)
    {
        saddr = mac_rx_frm_short_addr_get();
        daddr = (uint8_t *)&saddr;
    }
    else if (fc_ack.dst_addr_mode == ADDR_MODE_EXTEND)
    {
        laddr = mac_rx_frm_long_addr_get();
        daddr = (uint8_t *)&laddr;
    }

    // ----------------------------------------------------
    // 5. Generate Enhanced ACK frame payload
    // ----------------------------------------------------
    g_enh_ack_buf.len = generate_ieee_frame(FRAME_TYPE_ACK,
                                            g_enh_ack_buf.buf, TX_BUF_MAX_LEN,
                                            fc_ack, mac_rx_frm_seq_get(),
                                            NV_FIELD, NV_FIELD,    // Src PAN/Addr not used for ACK
                                            panid, daddr,
                                            (uint8_t *)&aux, sizeof(aux),
                                            NV_FIELD, NV_FIELD);

    // ----------------------------------------------------
    // 6. Apply security material if ACK is secured
    // ----------------------------------------------------
    if (fc_ack.sec_en)
    {
        nonce_t nonce = {0};

        mac_memcpy(&nonce.src_ext_addr,
                   mpan_GetLongAddress(pan_idx),
                   sizeof(nonce.src_ext_addr));
        nonce.sec_level     = aux.sec_ctl.sec_level;
        nonce.frame_counter = aux.frame_counter;

        mac_nonce_set((uint8_t *)&nonce);           // Set AES nonce
        mac_tx_enh_ack_key_set(g_mac_key);          // Load encryption key
        mac_tx_enh_ack_cipher_set(aux.sec_ctl.sec_level); // Set cipher level
    }

    // ----------------------------------------------------
    // 7. Configure TX hardware buffer for ACK transmission
    // ----------------------------------------------------
    mac_tx_enh_ack_payload_set(fc_ack.sec_en ? g_enh_ack_buf.len : 0,
                               g_enh_ack_buf.len,
                               g_enh_ack_buf.buf);
}

// EXELYIF
__attribute__((section(".app.ram.text")))
static void rxely_handler(uint8_t pan_idx, uint32_t arg)
{
#if TEST_GPIO_DBG_EN
    //GPIO_WriteBit(GPIO_PIN_OUTPUT_1, (BitAction)(1));
    //GPIO_WriteBit(GPIO_PIN_OUTPUT_1, (BitAction)(0));
#endif
    if (mac_rx_frm_version_get() == FRAME_VER_2015 && mac_rx_frm_ack_req_get() == 1)
    {
        mac_enhanced_ack_prepare(pan_idx);
        if (g_enh_ack_early)
        {
            mac_tx_enh_ack_trig(TRUE, mac_rx_frm_sec_en_get());
        }
        else
        {
            mac_tx_enh_ack_set_pending(TRUE);  // set enh-ack tx trigger is pending
        }
    }
    g_recent_rx_timestamp = (uint32_t)mac_timestamp_get();
}

// SECIF
#if (TEST_PROTOTHREAD_EN == 0)
static rx_item_t mac_rx_buf;
#endif
// RXIF
__attribute__((section(".app.ram.text")))
static void rxdone_handler(uint8_t pan_idx, uint32_t arg)
{
    uint8_t enh_ack_tx_sts = MAC_STS_SUCCESS;
    rx_item_t *pkt;
#if (TEST_MPAN_EN == 1)
    uint8_t *buf = (uint8_t *)arg;
#endif

#if (TEST_PROTOTHREAD_EN == 1)
    int rx_index = ringbufindex_peek_put(&pt_mac_rx_ring);
    if (rx_index == -1)
    {
        mac_stats_rx_full_inc(pan_idx);
#if (TEST_MPAN_EN == 1)
        if (buf == NULL)
        {
            mac_RxFlush();
        }
#else
        mac_RxFlush();
#endif
        dbg_printf("rx queue full\r\n");
#if TEST_ATCMD_ZIGBTEST
        // Delegate to centralized AT+ZIGBTEST module hook for queue full
        at_zigbtest_rx_queue_full_hook(pan_idx);
#endif

    }
    else
#endif
    {
#if (TEST_PROTOTHREAD_EN == 1)
        pkt = &pt_mac_rx_buf[rx_index];
#else
        pkt = &mac_rx_buf;
#endif
#if (TEST_MPAN_EN == 1)
        if (buf)
        {
            memcpy(pkt->raw, buf, buf[0] + 8);
        }
        else
        {
            mac_rx(pkt->raw);
        }
#else
        mac_rx(pkt->raw);
#endif
        uint8_t frm_len = pkt->raw[0];
        const uint8_t *mhr = &pkt->raw[1];
        uint8_t curr_tx_frame_type = mac_stats_rx_frame_type(mhr, frm_len);
        mac_stats_rx_total_inc(pan_idx, curr_tx_frame_type);
        mac_stats_rx_bytes_inc(pan_idx, curr_tx_frame_type, frm_len);


#if TEST_ATCMD_ZIGBTEST
        // Delegate RX statistics to the centralized AT+ZIGBTEST module
        // Check packet validity using CRC error status
        extern uint8_t mac_GetRxFrmCrcError(void);
        at_zigbtest_rxdone_handler_hook(pan_idx, pkt->raw, mac_GetRxFrmCrcError());
#endif

        if (mac_GetRxFrmType() == FRAME_TYPE_ACK)
        {
            mac_stats_rx_ack_inc(pan_idx);
        }

        pkt->pan_idx = pan_idx;
#if (TEST_PROTOTHREAD_EN == 1)
        /* Add current input to ringbuf */
        ringbufindex_put(&pt_mac_rx_ring);
#endif
    }

    if (mac_tx_enh_ack_get_pending())
    {
        if (g_test_enh_ack_late)
        {
            while (mac_tx_enh_ack_state_get());
            enh_ack_tx_sts = mac_tx_enh_ack_trig(FALSE, mac_rx_frm_sec_en_get());
        }
        else
        {
            enh_ack_tx_sts = mac_tx_enh_ack_trig(FALSE, mac_rx_frm_sec_en_get());
        }

        if (enh_ack_tx_sts == MAC_STS_TIMEOUT)
        {
            at_send_enh_ack_timeout();
        }
    }
#if (TEST_PROTOTHREAD_EN == 1)
    NETSTACK_MAC.input();
#else
    mac_stats_pktsig_update(MAC_RX_PKT_RSSI(pkt->raw), MAC_RX_PKT_LQI(pkt->raw));
    print_rx_packet(pkt->raw, pkt->pan_idx);
#endif
}

// BTCMP0IF
__attribute__((section(".app.ram.text")))
static void btcmp0_handler(uint8_t pan_idx, uint32_t arg)
{
    osif_sem_give(zb_sem);
}

__attribute__((section(".app.ram.text")))
static void btcmp1_handler(uint8_t pan_idx, uint32_t arg)
{
}

__attribute__((section(".app.ram.text")))
static void edscan_handler(uint8_t pan_idx, uint32_t arg)
{
}

#if (TEST_ZB_IRQ_MANUAL_EN == 1)
__attribute__((weak)) uint32_t zb_vector_num_get(void)
{
    return Zigbee_VECTORn;
}
#endif

void zb_mac_interrupt_enable(void)
{
    //NVIC_InitTypeDef NVIC_InitStruct;
    // TODO: enable MAC interrupt
    /* share the same IRQ number with BT_MAC on FPGA temporary, so the interrupt
       shall be initialed in BT lower stack initialization */
#if (TEST_ZB_IRQ_MANUAL_EN == 1)
    NVIC_SetPriority(Zigbee_IRQn, 2);
    NVIC_EnableIRQ(Zigbee_IRQn);
    extern void Zigbee_Handler_Patch(void);
    RamVectorTableUpdate(zb_vector_num_get(), Zigbee_Handler_Patch);
    DBG_DIRECT("RamVectorTableUpdate");
#else
    /* interrupt was enabled in mac_init_ext() */
#endif
}

void zb_mac_drv_init(void)
{
    mac_attribute_init(&g_mac_attribute);
    g_mac_attribute.mac_cfg.rf_early_term = 0;
    //g_mac_attribute.mac_cfg.frm06_rx_early = 0;
#if (TEST_PROTOTHREAD_EN == 1)
    if (g_phy_arbitration_method != 0xFFFF) // not a default value
    {
        mac_InitPHYArbitration(&g_mac_attribute, g_phy_arbitration_method);
    }
    else
#endif
    {
        mac_InitPHYArbitration(&g_mac_attribute, MAC_PHY_ARBI_IMMEDIATE); // or MAC_PHY_ARBI_ANCH
    }

    mac_enable();
    mac_init(&g_mac_driver, &g_mac_attribute);
#if (TEST_FPGA_DBG_PORT_EN == 1) && (TEST_PROTOTHREAD_EN == 1)
    if (g_dbg_port != -1 && g_rf_dbg_port != -1)
    {
        dbg_printf("initiate dbgport: ZB_DBGI %d dbg_port %d rf_dbg_port %d\r\n", ZB_DBGI, g_dbg_port,
                   g_rf_dbg_port);
        debug_port_open(ZB_DBGI);
        extern void macdbg_SetDbgPort(uint8_t dbg_port, uint8_t rf_dbg_port);
        macdbg_SetDbgPort((uint8_t)g_dbg_port, (uint8_t)g_rf_dbg_port);
    }
#endif
    mac_init_ext();
#if (TEST_ZB_PM_EN == 1)
    // reinit pm state
    PMUnitStatus state = power_manager_interface_get_unit_status(PM_SLAVE_ZIGBEE, PM_UNIT_ZIGBEE);
    if (state != PM_UNIT_ACTIVE)
    {
        PowerManagerSlaveUnit *zbmac = power_manager_slave_get_unit(PM_UNIT_ZIGBEE);
        if (zbmac)
        {
            dbg_printf("re-register 15.4 power unit\r\n");
            power_manager_slave_register_unit(PM_UNIT_ZIGBEE, zbmac);
        }
    }
    // wake up BT
    state = power_manager_interface_get_unit_status(PM_SLAVE_BTMAC, PM_UNIT_BTMAC);
    if (state != PM_UNIT_ACTIVE)
    {
        dbg_printf("wakeup BT\r\n");
        power_manager_interface_check_unit_active(PM_SLAVE_BTMAC, PM_UNIT_BTMAC);
        mac_radio_off();
        mac_radio_on();
    }
#endif
    //mac_cca_mode_set(MAC_CCA_CS);
    //mac_txn_csma_set(true);
    //mac_txn_retry_set(3);
    mac_panid_set(0x5);
    mac_channel_set(12);
    mac_short_addr_set(0x1);
#if (TEST_GET_IC_EUID == 1)
    uint64_t addr = get_ic_eui64();
    mac_long_addr_set((uint8_t *)&addr);
#endif
}

#if (TEST_ZB_PM_EN == 1)
int g_zbpm_wakeup_diff_min = INT_MAX;
int g_zbpm_wakeup_diff_max = INT_MIN;
int g_zbpm_wakeup_diff_avg = 0;
static zbpm_callback_t g_app_zbpm_exit = NULL;
static void default_zbpm_exit(void)
{
    mac_radio_on();
    //dbg_printf("default_zbpm_exit\r\n");
}

__WEAK void default_zbpm_enter(void)
{
}

void zbmac_power_manager_init(zbpm_callback_t exit_callback)
{
    g_app_zbpm_exit = exit_callback;
    if (TRUE == g_zbpm_inited)
    {
        return;
    }
    g_zbpm_inited = TRUE;

    mac_power_manager_init(&g_zbpm_adap, default_zbpm_enter, default_zbpm_exit);
    dbg_printf("zbmac_pm_init\r\n");
}

int zbmac_power_manager_set(uint32_t next, uint32_t period)
{
    if (FALSE == g_zbpm_inited)
    {
        dbg_printf("fail: not initiated\r\n");
        return FALSE;
    }

    zbpm_adapter_t *padapter = &g_zbpm_adap;

    if (mac_power_manager_set(padapter, next))
    {
        dbg_printf("fail: still in sleep state\r\n");
        return FALSE;
    }

    next = (next + 999) / 1000; // ms
    bool ext_awake = osif_sem_take(zb_sem, next);

#ifdef CFG_BOARD_BEE4
    // Bee4 workaround: osif_sem_take() returns inverted boolean (true = timeout)
    ext_awake = !ext_awake;
#endif
    //if (ext_awake)
    //    dbg_printf("ZbPmSet: awakened early by external events\r\n");

    if (padapter->power_mode == ZBMAC_DEEP_SLEEP)
    {
        //dbg_printf("warn: still in sleep state. error_code = %u wakeup_reason = %u\r\n", padapter->error_code, padapter->wakeup_reason);
        if (padapter->error_code != ZBMAC_PM_ERROR_UNKNOWN)
        {
            dbg_printf("ZbPmSet: not sleep. force change state to active\r\n");
            padapter->power_mode = ZBMAC_ACTIVE;
        }
        uint64_t now = osif_sys_time_get();
        while (padapter->power_mode == ZBMAC_DEEP_SLEEP)
        {
            if (now + 2 < osif_sys_time_get()) // Avoid unknown errors and getting stuck in loops
            {
                dbg_printf("ZbPmSet: wait wake_up timeout. mac_enable = %u error_code = %u wakeup_reason = %u\r\n",
                           mac_enabled_check(), padapter->error_code, padapter->wakeup_reason);
                padapter->power_mode = ZBMAC_ACTIVE;
                break;
            }
        }
        //dbg_printf("exit sleep state\r\n");
    }

    if (ext_awake == 0)
    {
        int new_delay = ((int)(mac_btus_get() - padapter->wakeup_time_us)) << 7;
        if (new_delay < g_zbpm_wakeup_diff_min)
        {
            g_zbpm_wakeup_diff_min = new_delay;
        }

        if (new_delay > g_zbpm_wakeup_diff_max)
        {
            g_zbpm_wakeup_diff_max = new_delay;
        }

        if (g_zbpm_wakeup_diff_avg == 0)
        {
            g_zbpm_wakeup_diff_avg = new_delay;
        }
        else
        {
            g_zbpm_wakeup_diff_avg = (g_zbpm_wakeup_diff_avg * 7 + new_delay) >> 3;
        }
        // dbg_printf("new: %d avg: %d\r\n", new_delay, g_zbpm_wakeup_diff_avg);
    }

    if (g_app_zbpm_exit)
    {
        g_app_zbpm_exit();
    }
    return TRUE;
}
#endif

static void reset_all_global_var(void)
{
#if (TEST_ZB_PM_EN == 1)
    g_zbpm_inited = FALSE;
#endif
    g_recent_rx_timestamp = 0;
    g_tx_done = TX_NONE;
    memset(&g_tx_buf, 0, sizeof(tx_buf_t));
    memcpy(g_mac_key, g_default_mac_key, sizeof(g_mac_key));
    g_tx_loop_state = TX_LOOP_STOP;
    g_tx_interval_ms = 1;
    g_enh_ack_early = TRUE;
    g_test_enh_ack_late = FALSE;
}

typedef void (*bt_hci_reset_handler_t)(void);
extern void mac_RegisterBtHciResetHanlder(bt_hci_reset_handler_t handler);
void zb_mac_drv_enable(void)
{
    reset_all_global_var();
    mpan_CommonInit(&g_pan_mac_comm);
    for (int i = 0; i < TEST_MAX_PAN_NUM; i++)
    {
        mpan_Init(&pan_mac[i], i);
        mpan_RegisterISR(i, txn_handler, txn_handler, rxely_handler, rxdone_handler, edscan_handler);
    }
#if (TEST_AUTO_INIT_EN == 1)
    zb_mac_drv_init();
#endif
#if (TEST_ZB_PM_EN == 1)
    zbmac_power_manager_init(NULL);
#endif
#if TEST_ATCMD_ZIGBTEST
    at_zigbtest_module_init();
#endif
    mac_RegisterBtHciResetHanlder(zb_mac_drv_init);
    /* share the same IRQ number with BT_MAC on FPGA temporary, so the interrupt
       shall be initialed in BT lower stack initialization */
    mac_callback_register(NULL, edscan_lv2dbm, set_zb_priority,
                          set_zb_cca_combination);

    mpan_RegisterTimer(0, MAC_BT_TIMER0, btcmp0_handler, 0);
    mpan_RegisterTimer(0, MAC_BT_TIMER1, btcmp1_handler, 0);
    mpan_EnableCtl(0, 1);
    //mpan_EnableCtl(1, 1);
#if TEST_GPIO_DBG_EN
    debug_gpio_init();
#endif
    work_queue_task_init();
}

#if (TEST_GET_IC_EUID == 1)
uint64_t get_ic_eui64(void)
{
    static uint64_t gIeeeEui64;
    static bool gIeeeEui64_init = 0;
    if (gIeeeEui64_init == 0)
    {
        uint32_t sha256_output[8];
        hw_sha256(get_ic_euid(), 14, sha256_output, 0);
        mac_memcpy(&gIeeeEui64, sha256_output, sizeof(uint64_t));
        gIeeeEui64_init = 1;
    }
    return gIeeeEui64;
}
#endif

volatile work_queue_task_cb_t work_queue_task_cb;
volatile void *work_queue_task_parm;
void *work_queue_task_handle;

static void work_queue_task(void *p_param)
{
    uint32_t notify = 0;
    dbg_printf("work_queue_task start\r\n");
    while (1)
    {
        osif_task_notify_take(1, 0xffffffff, &notify);
        if (work_queue_task_cb)
        {
            work_queue_task_cb((void *)work_queue_task_parm);
        }
    }
}

void work_queue_task_init(void)
{
    work_queue_task_cb = NULL;
    work_queue_task_parm = NULL;
    osif_task_create(&work_queue_task_handle, "work_queue_task", work_queue_task, NULL,
                     1024, 4);
}

void work_queue_msg_send(work_queue_task_cb_t callback, void *arg)
{
    work_queue_task_cb = callback;
    work_queue_task_parm = arg;
    osif_task_notify_give(work_queue_task_handle);
}
