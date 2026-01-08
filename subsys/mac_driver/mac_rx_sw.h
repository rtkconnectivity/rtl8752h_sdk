/**************************************************************************//**
 * @file      mac_rx_sw.h
 * @brief     IEEE 802.15.4 Software-based MAC RX driver header file.
 * @author    felix.yu
 * @version   V1.10
 * @date      2025-11-04
 *
 * @note
 *   This module provides a pure software implementation of the MAC RX path,
 *   including Frame Parser, RX Filter, and Source Address Match Filter.
 *
 ******************************************************************************
 * @attention
 * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
 ******************************************************************************
 * @license
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 ******************************************************************************/

#ifndef _MAC_RX_SW_H_
#define _MAC_RX_SW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ============================================================================
// Debug output macro
// ============================================================================
// Define RX_PARSER_PRINT as your debug printer.
// Example: #define RX_PARSER_PRINT(fmt, ...) dbg_printf(fmt "\r\n", ##__VA_ARGS__)
#ifndef RX_PARSER_PRINT
extern int dbg_printf(const char *fmt, ...);
#define RX_PARSER_PRINT(fmt, ...) dbg_printf(fmt "\r\n", ##__VA_ARGS__)
#endif

// -----------------------------------------------------------------------------
// Structure: parsed_rx_frame
// -----------------------------------------------------------------------------
/// Parsed RX frame information structure (filled by SW frame parser)
typedef struct
{
    // ---- Frame Control ----
    uint32_t frame_control;
    struct
    {
        uint32_t frame_type   : 3;
        uint32_t sec_en       : 1;
        uint32_t pend         : 1;
        uint32_t ack_req      : 1;
        uint32_t panid_comp   : 1;
        uint32_t              : 1;
        uint32_t seq_comp     : 1;
        uint32_t ie_exist     : 1;
        uint32_t dst_adr_mode : 2;
        uint32_t frm_ver      : 2;
        uint32_t src_adr_mode : 2;
        uint32_t              : 16;
    } frame_control_b;

    // ---- Sequence / IE Update ----
    uint32_t sn_ie_update;
    struct
    {
        uint32_t seq_num    : 8;
        uint32_t            : 1;
        uint32_t csl_update : 1;
        uint32_t            : 22;
    } sn_ie_update_b;

    // ---- CSL IE ----
    uint32_t csl_ie;
    struct
    {
        uint32_t csl_period : 16;
        uint32_t csl_phase  : 16;
    } csl_ie_b;

    // ---- Auxiliary Security Header ----
    uint32_t aux_sec_hdr;
    struct
    {
        uint32_t sec_level       : 3;
        uint32_t keyid_mode      : 2;
        uint32_t sec_frm_cnt_comp: 1;
        uint32_t sec_asn_nonce   : 1;
        uint32_t                 : 1;
        uint32_t key_index       : 8;
        uint32_t                 : 16;
    } aux_sec_hdr_b;

    // ---- Security Frame Counter ----
    uint32_t aux_frmcnt;
    struct
    {
        uint32_t sec_frm_cnt;
    } aux_frmcnt_b;

    // ---- Key Source ----
    uint32_t aux_key_src_l;
    struct { uint32_t key_src_l; } aux_key_src_l_b;

    uint32_t aux_key_src_h;
    struct { uint32_t key_src_h; } aux_key_src_h_b;

    // ---- Addressing Fields ----
    struct
    {
        uint16_t dst_panid;
        uint16_t src_panid;

        union
        {
            uint16_t short_addr;
            uint64_t long_addr;
        } dst_addr;

        union
        {
            uint16_t short_addr;
            uint64_t long_addr;
        } src_addr;
    } addressing;

    // ---- Frame length / status ----
    uint16_t frame_length;   ///< Total frame length in bytes
    uint16_t mhr_length;     ///< MAC header length
    uint8_t  parsed_ok;      ///< Header valid flag (1 = successfully parsed)
    uint8_t  reserved[3];    ///< Reserved for alignment (4-byte boundary)
} parsed_rx_frame;

// -----------------------------------------------------------------------------
// Global Variable
// -----------------------------------------------------------------------------
extern parsed_rx_frame g_parsed_rx;

// -----------------------------------------------------------------------------
// Inline Access APIs (Software-based MAC RX Path)
// -----------------------------------------------------------------------------
void mac_rx_sw_parse_frame(const uint8_t *psdu, uint16_t frame_len);
int  mac_should_send_ack(uint16_t my_panid, uint16_t my_short, uint64_t my_ext, uint8_t short_en);
void mac_rx_parsed_dump(void);
// Basic frame header accessors
static inline uint8_t mac_rx_frm_version_get_sw(void)
{ return g_parsed_rx.frame_control_b.frm_ver; }

static inline uint8_t mac_rx_frm_ack_req_get_sw(void)
{ return g_parsed_rx.frame_control_b.ack_req; }

static inline uint8_t mac_rx_frm_sec_en_get_sw(void)
{ return g_parsed_rx.frame_control_b.sec_en; }

static inline uint8_t mac_rx_frm_seq_compress_get_sw(void)
{ return g_parsed_rx.frame_control_b.seq_comp; }

static inline uint8_t mac_rx_frm_src_addr_mode_get_sw(void)
{ return g_parsed_rx.frame_control_b.src_adr_mode; }

static inline uint8_t mac_rx_frm_panid_compress_get_sw(void)
{ return g_parsed_rx.frame_control_b.panid_comp; }

// Security field accessors
static inline uint8_t mac_rx_frm_sec_level_get_sw(void)
{ return g_parsed_rx.aux_sec_hdr_b.sec_level; }

static inline uint8_t mac_rx_frm_sec_keyid_mode_get_sw(void)
{ return g_parsed_rx.aux_sec_hdr_b.keyid_mode; }

static inline uint8_t mac_rx_frm_sec_keyid_get_sw(void)
{ return g_parsed_rx.aux_sec_hdr_b.key_index; }

// Source address accessors
static inline uint16_t mac_rx_frm_src_short_addr_get_sw(void)
{ return g_parsed_rx.addressing.src_addr.short_addr; }

static inline uint64_t mac_rx_frm_src_long_addr_get_sw(void)
{ return g_parsed_rx.addressing.src_addr.long_addr; }

// Frame sequence number accessor
static inline uint8_t mac_rx_frm_seq_get_sw(void)
{ return g_parsed_rx.sn_ie_update_b.seq_num; }

// Header validity check
static inline uint8_t mac_rx_frm_hdr_is_valid_sw(void)
{ return g_parsed_rx.parsed_ok; }

#ifdef __cplusplus
}
#endif

#endif  // _MAC_RX_SW_H_
