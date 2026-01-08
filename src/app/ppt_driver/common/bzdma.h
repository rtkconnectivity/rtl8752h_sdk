/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef __BB_BZDMA_H__
#define __BB_BZDMA_H__

#include <stdint.h>
#include <stdbool.h>
#include "platform_utils.h"

#ifndef FALSE
#define FALSE false
#endif

#ifndef TRUE
#define TRUE true
#endif

//typedef bool BOOLEAN;
//typedef unsigned char UCHAR;
//typedef uint8_t UINT8;
//typedef uint16_t UINT16;
//typedef uint32_t UINT32;
//typedef uint64_t UINT64;
//typedef signed char CHAR;
//typedef int8_t INT8;
//typedef int16_t INT16;
//typedef int32_t INT32;
//typedef int64_t INT64;

#include "DataType.h"
//#include "bt_fw_os.h"
//#include "platform.h"

#define BZDMA_TX_DESC_ENTRY_NUM             13
#define BZDMA_LEONLY_TX_MAX_ENTRY_NUM             11

/* ============= BZ DMA Register Structure (End) =================== */
/* The structure of one segment of TX Descriptor */
typedef struct BZDMA_TX_DESC_SEGMENT_
{
    union
    {
        struct
        {
#ifdef _NEW_BZDMA_FROM_V10_FROM_160223_
            UINT32 start_addr: 18;  /* bit[17:0], the start address of SRAM */
#else
            UINT32 start_addr: 16;  /* bit[15:0], the start address of SRAM */
            UINT32 rsvd0: 2;        /* bit[17:16], reserved */
#endif
            UINT32 len: 11;         /* bit[28:18], the copied length*/
            UINT32 rsvd1: 3;        /* bit[31:29], reserved */
        };
        UINT32 DWord0;
    };
    union
    {
        struct
        {
            UINT32 rsvd2: 29;       /* bit[60:32], reserved */
            UINT32 llid: 2;         /* bit[62:61], the link layer id */
            UINT32 isLast: 1;       /* bit[63], is last segment */
        };
        UINT32 DWord1;
    };
} BZDMA_TX_DESC_SEGMENT, *PBZDMA_TX_DESC_SEGMENT;


typedef union BZDMA_BLEONLY_TX_DESC_SEGMENT_
{
    struct
    {
        UINT32 start_addr: 16;   /* bit[15:0], the start address of buffer SRAM
                                   (note: it's 8 byte units in 18 bits mode) */
        UINT32 len: 11;         /* bit[26:16], the copied length*/
#if defined (_NEW_BZDMA_FROM_V10_) || defined (_BT5_0_LE_LONG_RANGE_SUPPORT_)
        UINT32 le_ci: 2;        /* bit[28:27], code indicator for LE LR */
#else
        UINT32 rsvd: 2;         /* bit[28:27], reserved */
#endif
        UINT32 llid: 2;         /* bit[30:29], the link layer id (only valid for data) */
        UINT32 isLast: 1;       /* bit[31], is last segment (only valid for adv)
                                   (170524) after _BT5_1_LE_CONNECTION_CTE_RECEIVE_TRANSMIT_PROCEDURE_
                                            this bit is used to indicate the sp flag in the header of
                                            data channel pkt. */
    };
    UINT32 DWord;
} BZDMA_BLEONLY_TX_DESC_SEGMENT, *PBZDMA_BLEONLY_TX_DESC_SEGMENT;

/* The structure of one fragment of TX Descriptor for BLE only */
typedef union BZDMA_BLEONLY_TX_DESC_FRAGMENT_
{
    struct
    {
        UINT32 start_addr: 16;  /* bit[15:0], the start address of buffer SRAM */
        UINT32 len: 11;         /* bit[26:16], the copied length*/
        UINT32 rsvd: 4;         /* bit[30:27], reserved */
        UINT32 isLast: 1;       /* bit[31], is last segment */
    };
    struct
    {
        UINT32 start_addr_ex: 18; /* bit[17:0], the start address of buffer SRAM */
        UINT32 len_ex: 11;       /* bit[28:18], the copied length*/
        UINT32 rsvd_ex: 2;       /* bit[30:29], reserved */
        UINT32 isLast_ex: 1;     /* bit[31], is last segment */
    };
    UINT32 DWord;
} BZDMA_BLEONLY_TX_DESC_FRAGMENT, *PBZDMA_BLEONLY_TX_DESC_FRAGMENT;

/* The structure of TX descriptor entry status of BZDMA */
typedef struct BZDMA_TX_DESC_ENTRY_STATUS_
{
    BZDMA_TX_DESC_SEGMENT *pTxDesc; /* The pointer of multiple segments */
    UINT32 entry_idx: 4;            /* my entry index*/
    UINT32 pkt_id: 2;               /* current packet Id */
    UINT32 ch_id: 2;                /* current channel Id */
    UINT32 used: 1;                 /* the entry is used ? */
    UINT32 piconet_id: 2;           /* current piconet id */
    UINT32 total_len: 11;           /* the total data length of all segments */
    UINT32 total_segs: 4;           /* the total segements */
    UINT32 is_sco_lt: 1;            /* the logic link is sco ? */
#ifndef _NEW_HCI_DMA_DESIGN_FOR_ACL_SNIFF_SCHEDULE_
    UINT32 rsvd1: 5;                /* reserved */
#else
    UINT32 is_in_sniff: 1;          /* the command is used in sniff ? */
    UINT32 rsvd1: 4;                /* reserved */
#endif

    UINT32 le_mode_en: 1;           /* Enable Txdma in LE mode */
    UINT32 le_pdu_cmd: 2;           /* The Txdma PDU Command Type in LE */
    UINT32 le_conn_entry: 6;        /* Connection Entry in LE */
    UINT32 lt_addr: 3;              /* my lt_addr */
    UINT32 rsvd2: 20;               /* reserved */
} BZDMA_TX_DESC_ENTRY_STATUS, *PBZDMA_TX_DESC_ENTRY_STATUS;

/* Fragment memory layout:
 *               adv_id=0         adv_id=1         adv_id=2
 *          +----------------+----------------+----------------+
 * seg_id=0 | MAX_FRAGS_ADV0 | MAX_FRAGS_ADV1 | MAX_FRAGS_ADV2 |
 *          +----------------+----------------+----------------+
 * seg_id=1 | MAX_FRAGS_ADV0 | MAX_FRAGS_ADV1 | MAX_FRAGS_ADV2 |
 *          +----------------+----------------+----------------+
 */
/* The structure of Ble Advertising TX descriptor entry status of BZDMA */
typedef struct BZDMA_BLEONLY_ADV_TX_DESC_ENTRY_STATUS_
{
    BZDMA_BLEONLY_TX_DESC_SEGMENT *pTxSegDesc;   /* The start pointer of multiple segments */
    BZDMA_BLEONLY_TX_DESC_FRAGMENT *pTxFragDesc; /* The start pointer of multiple fragments */
    UINT32 max_segs: 2;             /* maximum segments in the tx descriptor */
    UINT32 max_frags: 4;            /* maximum fragments in one segment */
    UINT32 le_pdu_cmd: 2;           /* The Txdma PDU Command Type in LE */
#ifdef BT5_0_LE_ADV_EXT_SUPPORT_NEW_BZDMA_FOR_LR_AUX_SCAN_RSP
    UINT32 frag_row_num: 8;         /* Number of fragments in a row */
    UINT32 rsvd1: 16;               /* reserved */
#else
    UINT32 rsvd1: 24;               /* reserved */
#endif
    UINT8 TxSegDescOffset[3];       /* the start offset array of Tx segment descriptor for every advertising channels */
    UINT8 TxFragDescOffset[3];      /* the start offset array of Tx fragment descriptor for every advertising channels */
} BZDMA_BLEONLY_ADV_TX_DESC_ENTRY_STATUS, *PBZDMA_BLEONLY_ADV_TX_DESC_ENTRY_STATUS;

/* The structure of Ble Data TX descriptor entry status of BZDMA */
typedef struct BZDMA_BLEONLY_DATA_TX_DESC_ENTRY_STATUS_
{
    BZDMA_BLEONLY_TX_DESC_SEGMENT *pTxSegDesc;   /* The pointer of multiple segments */
    BZDMA_BLEONLY_TX_DESC_FRAGMENT *pTxFragDesc; /* The pointer of multiple fragment */
    UINT32 seg_wptr: 4;             /* write pointer of segment table */
    UINT32 seg_rptr: 4;             /* read pointer of segment table */
    UINT32 free_segs: 5;            /* free packet counts of segment table */
    UINT32 rsvd1: 3;                /* reserved */
    UINT32 bm_used_segs: 16;        /* index bitmap of used segments */
} BZDMA_BLEONLY_DATA_TX_DESC_ENTRY_STATUS, *PBZDMA_BLEONLY_DATA_TX_DESC_ENTRY_STATUS;

#ifdef _NEW_BZDMA_FROM_V20_200512_
#ifdef _BT5_0_LE_ADV_EXT_PA_HW_
/* The structure of Ble PA Data TX descriptor entry status of BZDMA */
typedef struct BZDMA_PA_TX_DESC_ENTRY_STATUS_
{
    BZDMA_BLEPA_TX_DESC_SEGMENT *pTxSegDesc;   /* The pointer of multiple segments */
    BZDMA_BLEPA_TX_DESC_FRAGMENT *pTxFragDesc; /* The pointer of multiple fragment */
    UINT32 seg_wptr: 4;                          /* write pointer of segment table (0~15) */
    UINT32 seg_rptr: 4;                          /* read pointer of segment table (0~15) */
    UINT32 free_segs: 5;                         /* free packet counts of segment table (max ?) */
    UINT32 rsvd1: 19;                            /* reserved */
} BZDMA_PA_TX_DESC_ENTRY_STATUS, *PBZDMA_PA_TX_DESC_ENTRY_STATUS;
#endif
#endif /* end of _NEW_BZDMA_FROM_V20_200512_ */

/* The structure of management of BZDMA */
typedef struct BZDMA_MANAGE_
{
    BZDMA_TX_DESC_ENTRY_STATUS TxEntSta[BZDMA_TX_DESC_ENTRY_NUM];
    UINT16 bmFreeTxEnt;     /* free Tx command entry bitmap */


    BZDMA_BLEONLY_ADV_TX_DESC_ENTRY_STATUS BleAdvTxEntSta;
    BZDMA_BLEONLY_DATA_TX_DESC_ENTRY_STATUS BleTxEntSta[BZDMA_LEONLY_TX_MAX_ENTRY_NUM];
    UINT16 bmFreeBleTxEnt;  /* free Tx command entry bitmap */

#ifdef _BT5_0_LE_ADV_EXT_PA_HW_
    BZDMA_PA_TX_DESC_ENTRY_STATUS BlePATxEntSta[BZDMA_LEONLY_PA_TX_MAX_ENTRY_NUM];
#endif
} BZDMA_MANAGE, *PBZDMA_MANAGE;


extern BZDMA_MANAGE Bzdma_Manager;

#endif /* __BB_BZDMA_H__ */

