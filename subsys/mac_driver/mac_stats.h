/**************************************************************************//**
   * @file      mac_stats.h
   * @brief     IEEE802.15.4 MAC tx/rx debug counter
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
**************************************************************************************
* @attention
* <h2><center>&copy; COPYRIGHT 2024 - 2025 Realtek Semiconductor Corporation</center></h2>
**************************************************************************************
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


#ifndef _MAC_STATS_H_
#define _MAC_STATS_H_

#include <stdint.h>
#include "mac_driver.h"
#include "mac_driver_mpan.h"

/* Compile-time flag to control statistics update code */
#ifndef MAC_STATS_EN
#define MAC_STATS_EN 0
#endif

#if MAC_STATS_EN
#ifndef MAC_STATS_TX_EN
#define MAC_STATS_TX_EN 1
#endif
#ifndef MAC_STATS_RX_EN
#define MAC_STATS_RX_EN 1
#endif
#ifndef MAC_STATS_IRQ_EN
#define MAC_STATS_IRQ_EN 1
#endif
#ifndef MAC_STATS_PKTSIG_EN
#define MAC_STATS_PKTSIG_EN 1
#endif
#else
#undef MAC_STATS_TX_EN
#undef MAC_STATS_RX_EN
#undef MAC_STATS_IRQ_EN
#undef MAC_STATS_PKTSIG_EN
#endif

/* Maximum number of PANs (default: 1) */
#ifndef MAX_PAN_NUM
#define MAX_PAN_NUM 1
#endif

/* Controls if separate unicast and broadcast counters are maintained */
#ifndef DIFF_UNI_BROAD
#define DIFF_UNI_BROAD 0
#endif

/* Frame type enumeration */
typedef enum
{
    MAC_STATS_FRM_UNICAST = 0,
    MAC_STATS_FRM_BROADCAST = 1
} mac_stats_frm_type_t;

/************************************
 * TX Statistics Structures
 ************************************/
typedef struct
{
    struct
    {
        uint32_t total;     /* Count of successful transmissions */
        uint32_t cca_fail;  /* Count of CCA failures in successful transmissions */
        uint32_t no_ack;    /* Count of no-ACK occurrences */
    } succ;
    struct
    {
        uint32_t total;     /* Total transmission failures */
        uint32_t cca_fail;  /* Failures due to CCA */
        uint32_t no_ack;    /* Failures due to no ACK */
        uint32_t phy_grant; /* Failures due to PHY grant */
        uint32_t receiving; /* Failures due to receiving activity */
        uint32_t notime;    /* Failures due to timing issues */
    } fail;
    uint32_t request;       /* Transmission request count */
} mac_stats_tx_frm_t;

typedef struct
{
    uint32_t max_time;   /* Maximum response time */
    uint32_t min_time;   /* Minimum response time */
    uint64_t avg_time;   /* Accumulated response time */
} mac_stats_tx_resp_t;

typedef struct
{
    mac_stats_tx_frm_t frm[1 + DIFF_UNI_BROAD];
    uint32_t request_time;      /* Time when TX was requested */
#if DIFF_UNI_BROAD
    uint8_t curr_tx_frame_type; /* Current frame type (unicast or broadcast) */
#endif
    mac_stats_tx_resp_t resp;
} mac_stats_tx_t;

/************************************
 * RX Statistics Structures
 ************************************/
typedef struct
{
    uint32_t total;  /* Count of received frames */
    uint32_t bytes;  /* Total received bytes */
} mac_stats_rx_frm_t;

typedef struct
{
    mac_stats_rx_frm_t frm[1 + DIFF_UNI_BROAD]; /* [0]=unicast; [1]=broadcast if enabled */
    uint32_t ack;       /* ACK count */
    uint32_t fifofull;  /* FIFO full count */
} mac_stats_rx_t;

/************************************
 * IRQ Statistics Structures
 ************************************/
typedef struct
{
    uint32_t rxelyif;   /* MHR RX interrupt count */
    uint32_t rxif;      /* RX interrupt count */
    uint32_t rxnakif;   /* RX NAK interrupt count */
    uint32_t txnterrif; /* TX error interrupt count */
    uint32_t txnif;     /* TX interrupt count */
} mac_stats_irq_t;

/************************************
 * Packet Signal Statistics Structure
 ************************************/
typedef struct
{
    uint32_t update_count;
    int32_t accum_rssi;
    uint32_t accum_lqi;
    int8_t max_rssi;
    int8_t min_rssi;
    uint8_t max_lqi;
    uint8_t min_lqi;
} mac_stats_pktsig_t;

/************************************
 * Combined Statistics Structure
 ************************************/
typedef struct
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t tx[MAX_PAN_NUM];
#endif
#if MAC_STATS_RX_EN
    mac_stats_rx_t rx[MAX_PAN_NUM];
#endif
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t irq;
#endif
#if MAC_STATS_PKTSIG_EN
    mac_stats_pktsig_t pktsig;
#endif
    struct
    {
        uint8_t max_pan_num;
        uint8_t diff_uni_broad;
    } cfg;
} mac_stats_t;

/************************************
 * External Global Variables
 ************************************/
#if MAC_STATS_EN
extern mac_stats_t g_mac_stats;
#endif

/* ------------------------------------------------------------------------
 * mac_stats_frame_type
 * ------------------------------------------------------------------------
 * When (MAC_STATS_EN == 1 && DIFF_UNI_BROAD == 1), only a function prototype
 * is provided (implementation is in the .c file). Otherwise, an inline
 * definition is provided that simply returns false.
 * ------------------------------------------------------------------------ */
#if (MAC_STATS_EN == 1) && (DIFF_UNI_BROAD == 1)
mac_stats_frm_type_t mac_stats_frame_type(const uint8_t *mhr, uint8_t frame_len);
static inline mac_stats_frm_type_t mac_stats_rx_frame_type(const uint8_t *mhr, uint8_t frame_len)
{
    return mac_stats_frame_type(mhr, frame_len);
}
#else
static inline mac_stats_frm_type_t mac_stats_frame_type(const uint8_t *mhr, uint8_t frame_len)
{
    return MAC_STATS_FRM_UNICAST;
}

static inline mac_stats_frm_type_t mac_stats_rx_frame_type(const uint8_t *mhr, uint8_t frame_len)
{
    return MAC_STATS_FRM_UNICAST;
}
#endif

static inline uint32_t time_diff(uint32_t start, uint32_t end)
{
    return (end >= start) ? (end - start) : ((MAX_BT_CLOCK_COUNTER - start) + end);
}


/* ------------------------------------------------------------------------
 * Getter Function Prototypes
 * When MAC_STATS_EN == 1, these getter functions are implemented
 * in the C file; otherwise, inline versions returning default values
 * (e.g., 0) are defined here.
 * ------------------------------------------------------------------------ */
#if MAC_STATS_TX_EN
void mac_stats_tx_print(uint8_t pan_idx);
uint32_t mac_stats_tx_request_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_succ_total_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_succ_cca_fail_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_succ_no_ack_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_total_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_cca_fail_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_no_ack_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_phy_grant_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_receiving_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_fail_notime_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_tx_resp_max_time_get(uint8_t pan_idx);
uint32_t mac_stats_tx_resp_min_time_get(uint8_t pan_idx);
uint64_t mac_stats_tx_resp_avg_time_get(uint8_t pan_idx);
#else
static inline void mac_stats_tx_print(uint8_t pan_idx) {};
static inline uint32_t mac_stats_tx_request_get(uint8_t pan_idx, mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_succ_total_get(uint8_t pan_idx,
                                                   mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_succ_cca_fail_get(uint8_t pan_idx,
                                                      mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_succ_no_ack_get(uint8_t pan_idx,
                                                    mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_total_get(uint8_t pan_idx,
                                                   mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_cca_fail_get(uint8_t pan_idx,
                                                      mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_no_ack_get(uint8_t pan_idx,
                                                    mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_phy_grant_get(uint8_t pan_idx,
                                                       mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_receiving_get(uint8_t pan_idx,
                                                       mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_fail_notime_get(uint8_t pan_idx,
                                                    mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_tx_resp_max_time_get(uint8_t pan_idx) { return 0; }
static inline uint32_t mac_stats_tx_resp_min_time_get(uint8_t pan_idx) { return 0; }
static inline uint64_t mac_stats_tx_resp_avg_time_get(uint8_t pan_idx) { return 0; }
#endif

#if MAC_STATS_RX_EN
void mac_stats_rx_print(uint8_t pan_idx);
uint32_t mac_stats_rx_total_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_rx_bytes_get(uint8_t pan_idx, mac_stats_frm_type_t frameType);
uint32_t mac_stats_rx_ack_get(uint8_t pan_idx);
uint32_t mac_stats_rx_full_get(uint8_t pan_idx);
uint32_t mac_stats_rx_total_sum_get(uint8_t pan_idx);
uint32_t mac_stats_rx_bytes_sum_get(uint8_t pan_idx);
#else
static inline void mac_stats_rx_print(uint8_t pan_idx) {};
static inline uint32_t mac_stats_rx_total_get(uint8_t pan_idx, mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_rx_bytes_get(uint8_t pan_idx, mac_stats_frm_type_t frameType) { return 0; }
static inline uint32_t mac_stats_rx_ack_get(uint8_t pan_idx) { return 0; }
static inline uint32_t mac_stats_rx_full_get(uint8_t pan_idx) { return 0; }
static inline uint32_t mac_stats_rx_total_sum_get(uint8_t pan_idx) { return 0; }
static inline uint32_t mac_stats_rx_bytes_sum_get(uint8_t pan_idx) { return 0; }
#endif

#if MAC_STATS_IRQ_EN
void mac_stats_irq_print(void);
uint32_t mac_stats_irq_rxelyif_get(void);
uint32_t mac_stats_irq_rxif_get(void);
uint32_t mac_stats_irq_rxnakif_get(void);
uint32_t mac_stats_irq_txnterrif_get(void);
uint32_t mac_stats_irq_txnif_get(void);
#else
static inline void mac_stats_irq_print(void) {};
static inline uint32_t mac_stats_irq_rxelyif_get(void) { return 0; }
static inline uint32_t mac_stats_irq_rxif_get(void) { return 0; }
static inline uint32_t mac_stats_irq_rxnakif_get(void) { return 0; }
static inline uint32_t mac_stats_irq_txnterrif_get(void) { return 0; }
static inline uint32_t mac_stats_irq_txnif_get(void) { return 0; }
#endif

#if MAC_STATS_PKTSIG_EN
void mac_stats_pktsig_print(void);
void mac_stats_pktsig_initialize(void);
void mac_stats_pktsig_reset(void);
void mac_stats_pktsig_update(int8_t rssi, uint8_t lqi);
int mac_stats_pktsig_get(mac_stats_pktsig_t *stats_copy, int8_t *avg_rssi, uint8_t *avg_lqi,
                         int clear);
#else
static inline void mac_stats_pktsig_print(void) {};
static inline void mac_stats_pktsig_initialize(void) {}
static inline void mac_stats_pktsig_reset(void) {}
static inline void mac_stats_pktsig_update(int8_t rssi, uint8_t lqi) {}
static inline int mac_stats_pktsig_get(mac_stats_pktsig_t *stats_copy, int8_t *avg_rssi,
                                       uint8_t *avg_lqi, int clear) { return -1; }
#endif

/************************************
 * Instance Getter Functions (inline)
 ************************************/
static inline mac_stats_t *mac_stats_get_instance(void)
{
#if MAC_STATS_EN
    return &g_mac_stats;
#else
    return NULL;
#endif
}

static inline mac_stats_tx_t *mac_stats_tx_get_instance(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    return (pan_idx < MAX_PAN_NUM) ? &g_mac_stats.tx[pan_idx] : NULL;
#else
    return NULL;
#endif
}

static inline mac_stats_rx_t *mac_stats_rx_get_instance(uint8_t pan_idx)
{
#if MAC_STATS_RX_EN
    return (pan_idx < MAX_PAN_NUM) ? &g_mac_stats.rx[pan_idx] : NULL;
#else
    return NULL;
#endif
}

static inline mac_stats_irq_t *mac_stats_irq_get_instance(void)
{
#if MAC_STATS_IRQ_EN
    return &g_mac_stats.irq;
#else
    return NULL;
#endif
}

static inline mac_stats_pktsig_t *mac_stats_pktsig_get_instance(void)
{
#if MAC_STATS_PKTSIG_EN
    return &g_mac_stats.pktsig;
#else
    return NULL;
#endif
}

/* TX increment functions */
static inline void mac_stats_tx_resp_time_update(uint8_t pan_idx, uint32_t resp_time)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (stats)
    {
        if (resp_time > stats->resp.max_time)
        {
            stats->resp.max_time = resp_time;
        }
        if (resp_time < stats->resp.min_time)
        {
            stats->resp.min_time = resp_time;
        }
        stats->resp.avg_time += resp_time;
    }
#endif
}

static inline void mac_stats_tx_request_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    pmac_txfifo_t pTxNFIFO = (pmac_txfifo_t)MAC_TXN_BASE_ADDR;
    uint8_t idx = mac_stats_frame_type(pTxNFIFO->payload, pTxNFIFO->frm_len);
    stats->curr_tx_frame_type = idx;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].request++;
    stats->request_time = mac_GetCurrentBTUS();
#endif
}

static inline void mac_stats_tx_succ_total_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].succ.total++;
    stats->frm[idx].succ.no_ack += mac_GetTxNRetryTimes();
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

static inline void mac_stats_tx_succ_cca_fail_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].succ.cca_fail++;
#endif
}

static inline void mac_stats_tx_fail_cca_fail_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].fail.cca_fail++;
    stats->frm[idx].fail.total++;
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

static inline void mac_stats_tx_fail_no_ack_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].fail.no_ack++;
    stats->frm[idx].fail.total++;
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

static inline void mac_stats_tx_fail_phy_grant_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].fail.phy_grant++;
    stats->frm[idx].fail.total++;
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

static inline void mac_stats_tx_fail_receiving_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].fail.receiving++;
    stats->frm[idx].fail.total++;
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

static inline void mac_stats_tx_fail_notime_inc(uint8_t pan_idx)
{
#if MAC_STATS_TX_EN
    mac_stats_tx_t *stats = mac_stats_tx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    uint8_t idx = stats->curr_tx_frame_type;
#else
    uint8_t idx = 0;
#endif
    stats->frm[idx].fail.notime++;
    stats->frm[idx].fail.total++;
    mac_stats_tx_resp_time_update(pan_idx, time_diff(stats->request_time, mac_GetCurrentBTUS()));
#endif
}

/* RX increment functions */
static inline void mac_stats_rx_total_inc(uint8_t pan_idx, mac_stats_frm_type_t frameType)
{
#if MAC_STATS_RX_EN
    mac_stats_rx_t *stats = mac_stats_rx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    stats->frm[frameType].total++;
#else
    stats->frm[0].total++;
#endif
#endif
}

static inline void mac_stats_rx_bytes_inc(uint8_t pan_idx, mac_stats_frm_type_t frameType,
                                          uint32_t bytes)
{
#if MAC_STATS_RX_EN
    mac_stats_rx_t *stats = mac_stats_rx_get_instance(pan_idx);
    if (!stats)
    {
        return;
    }
#if DIFF_UNI_BROAD
    stats->frm[frameType].bytes += bytes;
#else
    stats->frm[0].bytes += bytes;
#endif
#endif
}

static inline void mac_stats_rx_ack_inc(uint8_t pan_idx)
{
#if MAC_STATS_RX_EN
    mac_stats_rx_t *stats = mac_stats_rx_get_instance(pan_idx);
    if (stats)
    {
        stats->ack++;
    }
#endif
}

static inline void mac_stats_rx_full_inc(uint8_t pan_idx)
{
#if MAC_STATS_RX_EN
    mac_stats_rx_t *stats = mac_stats_rx_get_instance(pan_idx);
    if (stats)
    {
        stats->fifofull++;
    }
#endif
}

/* IRQ increment functions */
static inline void mac_stats_irq_rxelyif_inc(void)
{
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t *stats = mac_stats_irq_get_instance();
    if (stats)
    {
        stats->rxelyif++;
    }
#endif
}

static inline void mac_stats_irq_rxif_inc(void)
{
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t *stats = mac_stats_irq_get_instance();
    if (stats)
    {
        stats->rxif++;
    }
#endif
}

static inline void mac_stats_irq_rxnakif_inc(void)
{
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t *stats = mac_stats_irq_get_instance();
    if (stats)
    {
        stats->rxnakif++;
    }
#endif
}

static inline void mac_stats_irq_txnterrif_inc(void)
{
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t *stats = mac_stats_irq_get_instance();
    if (stats)
    {
        stats->txnterrif++;
    }
#endif
}

static inline void mac_stats_irq_txnif_inc(void)
{
#if MAC_STATS_IRQ_EN
    mac_stats_irq_t *stats = mac_stats_irq_get_instance();
    if (stats)
    {
        stats->txnif++;
    }
#endif
}

/************************************
 * Other API Function Prototypes
 ************************************/
#if MAC_STATS_EN
void mac_stats_init(void);
#else
static inline void mac_stats_init(void) {};
#endif

#endif /* _MAC_STATS_H_ */
