/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#ifndef __LL_COMMON_H__
#define __LL_COMMON_H__

//#include "DataType.h"
#include "rtl876x.h"
#include "bzdma.h"

#define _NEW_BZDMA_FROM_V7_
#define _NEW_BZDMA_FROM_V8_
#define _NEW_BZDMA_FROM_V10_FROM_160223_
#define _NEW_BZDMA_FROM_V10_
#define _BT5_0_LE_LONG_RANGE_SUPPORT_
#define _VENDOR_REG_MUST_UPDATE_FROM_RTL_SVN_VER_17316_
#define _RTL8821A_
#define _NEW_MODEM_PI_ACCESS_
#define _NEW_MODEM_PI_ACCESS_BY_VENDOR_REG_

#define BT_SOC_VENDOR_BASE_ADDR     VENDOR_REG_BASE//0x40006000
#define BT_SOC_VENDOR_ADDR(offs)    (BT_SOC_VENDOR_BASE_ADDR + (offs))
#define BTMAC_ISR_STATUS_REG_ADDR   BT_SOC_VENDOR_ADDR(0x10)

/* The Base of LE Register */
#define LE_REG_BASE                    0x40051000
#define PPT_REG_ADDR_BASE              (BLUEWIZ_REG_BASE + 0x1c00)

/* =========================== vendor section ===========================*/
/* note: vender reg can be access by 1, 2, 4 byte */
/* note: should be 1-byte aligned, 2-byte aligned, 4-byte aligned */
#define VENDOR_BASE_ADDRESS                 0x40058000
#define BT_SYS_CTRL_REG                     0x00

#define VENDOR_REG_ADDR(offset) (VENDOR_BASE_ADDRESS + (offset))
#define VENDOR_READ(Vendor_offset) \
    ((uint32_t)*((volatile uint32_t*)(VENDOR_BASE_ADDRESS+(Vendor_offset))))
#define VENDOR_WRITE(Vendor_offset, Value) \
    ((*((volatile uint32_t*)(VENDOR_BASE_ADDRESS + (Vendor_offset)))) = (Value))
#define VENDOR_U16_READ(Vendor_offset) \
    ((uint16_t)*((volatile uint16_t*)(VENDOR_BASE_ADDRESS+(Vendor_offset))))
#define VENDOR_U16_WRITE(Vendor_offset, Value) \
    ((*((volatile uint16_t*)(VENDOR_BASE_ADDRESS + (Vendor_offset)))) = (Value))
#define VENDOR_BYTE_READ(Vendor_offset) \
    (*((volatile uint8_t*)(VENDOR_BASE_ADDRESS+(Vendor_offset))))
#define VENDOR_BYTE_WRITE(Vendor_offset, Value) \
    ((*((volatile uint8_t*)(VENDOR_BASE_ADDRESS + (Vendor_offset)))) = (Value))
#define VENDOR_U32_REG_UPDATE(Vendor_offset, Mask, Value) \
    {uint32_t temp; \
        temp = VENDOR_READ(Vendor_offset) & ~(Mask);\
        VENDOR_WRITE(Vendor_offset, (temp | ((Value) & (Mask))));}
#define VENDOR_U16_REG_UPDATE(Vendor_offset, Mask, Value) \
    {uint16_t temp; \
        temp = VENDOR_U16_READ(Vendor_offset) & ~(Mask);\
        VENDOR_U16_WRITE(Vendor_offset, (temp | ((Value) & (Mask))));}
#define VENDOR_U8_REG_UPDATE(Vendor_offset, Mask, Value) \
    {uint8_t temp; \
        temp = VENDOR_BYTE_READ(Vendor_offset) & ~(Mask);\
        VENDOR_BYTE_WRITE(Vendor_offset, (temp | ((Value) & (Mask))));}

#define RD_32BIT_IO(base, offset) \
    (*(volatile UINT32*)((base) + (offset)))
#define RD_16BIT_IO(base, offset) \
    (*((volatile UINT16*)((base) + (offset))))
#define RD_8BIT_IO(base, offset) \
    (*((volatile UINT8*)((base) + (offset))))
#define WR_32BIT_IO(base, offset, val) \
    (*((volatile UINT32*)((base) + (offset))) = (val))
#define WR_16BIT_IO(base, offset, val) \
    (*((volatile UINT16*)((base) + (offset))) = (val))
#define WR_8BIT_IO(base, offset, val) \
    (*((volatile UINT8*)((base) + (offset))) = (val))

#define RD_U32_BZDMA_REG(offset)        HAL_READ32(BZDMA_REG_BASE, offset)//RD_32BIT_IO(BZDMA_REG_BASE, offset)
#define RD_U16_BZDMA_REG(offset)        HAL_READ16(BZDMA_REG_BASE, offset)//RD_16BIT_IO(BZDMA_REG_BASE, offset)
#define RD_U8_BZDMA_REG(offset)         HAL_READ8(BZDMA_REG_BASE, offset)//RD_8BIT_IO(BZDMA_REG_BASE, offset)
#define WR_U32_BZDMA_REG(offset, val)   HAL_WRITE32(BZDMA_REG_BASE, offset, val)//WR_32BIT_IO(BZDMA_REG_BASE, offset, val)
#define WR_U16_BZDMA_REG(offset, val)   HAL_WRITE16(BZDMA_REG_BASE, offset, val)//WR_16BIT_IO(BZDMA_REG_BASE, offset, val)
#define WR_U8_BZDMA_REG(offset, val)    HAL_WRITE8(BZDMA_REG_BASE, offset, val)//WR_8BIT_IO(BZDMA_REG_BASE, offset, val)

#ifdef _NEW_BZDMA_FROM_V10_
#define BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(num)      ((((num)&BIT3)?0x464:0x440)+(((num)&0x7) << 2))
#else
#define BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(num)      (0x440 + ((num) << 2))
#endif

/* BZ DMA register list */
#define BZDMA_REG_BASE              0x40058000
#define BZDMA_REG_ACL_RXFIFO_PTR    0x33C

#define BZDMA_ACL_RXFIFO_PTR_LEN    11
#define BZDMA_ACL_RXFIFO_PTR_MASK   0x7FF
#define BZDMA_ACL_RXFIFO_WPTR(reg_value)    ((reg_value) & BZDMA_ACL_RXFIFO_PTR_MASK)
#define BZDMA_ACL_RXFIFO_RPTR(reg_value)    (((reg_value) >> 16) & BZDMA_ACL_RXFIFO_PTR_MASK)
#define BZDMA_ACL_RXFIFO_GET_LEFT(reg)      (((reg.wr_ptr >= reg.rd_ptr ? reg.wr_ptr - reg.rd_ptr : reg.wr_ptr + (1 << BZDMA_ACL_RXFIFO_PTR_LEN) - reg.rd_ptr) + reg.rdbuffer_cnt) << 1)

/* Structure of Vendor Register 0x33C(BZDMA_REG_ACL_RXFIFO_PTR)*/
typedef union ACL_RX_FIFO_S_
{
    uint32_t d32;
    struct
    {
        UINT32 wr_ptr: 11;      /* [10:0] write pointer (unit: word)*/
        UINT32 rsvd1: 5;        /* [15:11] reserved */
        UINT32 rd_ptr: 11;      /* [26:16] read pointer (unit: word) */
        UINT32 rdbuffer_cnt: 2; /* [28:27] read_buffer_cnt (unit: word)(Range:0~2)*/
UINT32 rxfifo_busy:
        1;  /* [29] Rx fifo busy indicator. 1: HW is pushing data into acl rx fifo or sco fifo. */
        UINT32 rsvd2: 2;        /* [31:30] reserved 2 */
    };
} ACL_RX_FIFO_S, *PACL_RX_FIFO_S;

/* Baseband Control */
#define INSTRUCTION_REGISTER                                  0x00
#define CONNECTOR_REGISTER                                    0x02

#define BB_BASE_ADDR    0x40050000
#define BB_REG_ADDR(offset) (BB_BASE_ADDR + (offset))

/* BASEBAND CONTROLLER OPCODES (Please Sort the OpCode) */
#define      BB_NOP                                 0X00
#define      BB_INQUIRY                             0X01
#define      BB_INQUIRY_SCAN_FIRST_ID               0X02
#define      BB_PAGE                                0X03
#define      BB_PAGE_SCAN                           0X04
#define      BB_EXECUTE                             0X05
#define      BB_HOLD                                0X06
#define      BB_SNIFF                               0X07
#define      BB_START_BEACON                        0X08
#define      BB_UNPARK_REQUEST                      0X09
#define      BB_INQUIRY_SCAN_SECOND_ID              0X0A
#define      BB_START_SLEEP                         0X0B
#ifdef _RTL8821A_
#define      BB_INQUIRY_SCAN_3DD                    0X0B
#endif
#define      BB_FLUSH                               0X0C
#define      BB_SEND_PACKET                         0X0D
#define      BB_MASTER_SLAVE_SWITCH                 0X0E
#define      BB_LOAD_OFFSET                         0X0F

#define      BB_SWITCH_MASTER_PICONET               0X10
#define      BB_PRIORITY_SEND_PACKET                0x12
#define      BB_KILL_SEND_PACKET                    0x13
#ifdef _RTL8821A_
#define      BB_START_SYNC_3DD                      0x14
#define      BB_START_BEACON_3DD                    0x15
#endif
#ifdef _SUPPORT_CSB_RECEIVER_
#define      BB_START_SYNC_SCAN_3DD                 0x16
#define      BB_START_CSB_SCAN_3DD                  0x17
#endif
#define      BB_SWITCH_SLAVE_PICONET                0x18

#define      BB_SEQN_INIT                           0x21
#define      BB_SEND_FLOW_ZERO                      0x23
#define      BB_SEND_FLOW_GO                        0x24

#define      BB_START_TPOLL                         0x30
#define      BB_STOP_TPOLL                          0x31
#ifdef _NEW_MODEM_PSD_SCAN_
#define      BB_SET_PSD_EN                          0x40
#define      BB_CLR_PSD_EN                          0x41
#endif

#define      BB_RESET                               0X80
#define      BB_KILL_INQUIRY                        0X81
#define      BB_KILL_PAGE                           0X82
#define      BB_KILL_HOLD                           0X83
#define      BB_KILL_SNIFF                          0x84
#define      BB_KILL_SLEEP                          0x85
#define      BB_EXIT_BEACON                         0x86
#define      BB_KILL_BEACON                         0x87
#define      BB_ENTER_SNIFF_TRAN_MODE_DURING_EXIT_SNIFF_MASTER    0x89
#define      BB_ENTER_SNIFF_TRAN_MODE               0x89
#define      BB_EXIT_SNIFF_TRAN_MODE                0x8c
#ifdef _RTL8821A_
#ifdef _DAPE_TEST_NEW_HW_INSTRUCTION
#define      BB_KILL_INQUIRY_SCAN                   0X8E
#define      BB_KILL_PAGE_SCAN                      0X8F
#endif
#define      BB_KILL_SYNC_3DD                       0x90
#define      BB_KILL_BEACON_3DD                     0x91
#endif
#ifdef _SUPPORT_CSB_RECEIVER_
#define      BB_KILL_SYNC_SCAN_3DD                  0x92
#define      BB_KILL_CSB_SCAN_3DD                   0x93
#endif

#ifdef _NEW_MODEM_PI_ACCESS_
extern uint16_t (*modem_pi_read)(uint8_t, uint8_t);
extern void (*modem_pi_write)(uint8_t, uint8_t, uint16_t);
extern void (*modem_pi_update)(uint8_t, uint8_t, uint16_t, uint16_t);

#define RTK_READ_MODEM_REG_PI(modem_page, waddr) modem_pi_read(modem_page, waddr)
#define RTK_WRITE_MODEM_REG_PI(modem_page, waddr, value) modem_pi_write(modem_page, waddr, value)
#define RTK_UPDATE_MODEM_REG_PI(modem_page, waddr, bm_mask, value) modem_pi_update(modem_page, waddr, bm_mask, value)
#endif

#ifdef _NEW_MODEM_PI_ACCESS_
#ifdef _NEW_MODEM_PI_ACCESS_BY_VENDOR_REG_
#define REG_BTPHY_FPI_IF                0x354
#define PI_READ_MODEM_REG               0
#define PI_WRITE_MODEM_REG              1
/* Read wiki page for more information about modem PI access:     *
 * https://wiki.realtek.com/pages/viewpage.action?pageId=20234492 */
typedef union
{
    struct
    {
        UINT32 pi_wrdata          : 16; //[15: 0] rw
        UINT32 pi_modem_addr      :  6; //[21:16] rw
        UINT32 pi_modem_page      :  6; //[27:22] rw
        UINT32 pi_wr              :  1; //[28]    w, 0: read modem, 1: write modem
        UINT32 bt_phy_pi_en       :  1; //[29]    r
        UINT32 pi_rising_edge_time:  2; //[31:30] rw, 4*(1 + pi_rising_edge_time)T must >= 100ns
    };
    UINT32 u32;
} MODEM_PI_VENDOR_REG_S;
#else
extern UINT8 g_efuse_modem_pi_enable;
#endif
extern UINT8 g_modem_init_completed;
#define MODEM_PI_PAGE_0                 0
#define MODEM_PI_PAGE_1                 1
#define MODEM_PI_PAGE_2                 2
#define MODEM_PI_PAGE_3                 3
#define MODEM_PI_PAGE_4                 4
#define MODEM_PI_PAGE_5                 5
#define MODEM_PI_PAGE_6                 6
#define MODEM_PI_PAGE_7                 7
#define MODEM_PI_PAGE_NEW_AGC_TAB0      8
#define MODEM_PI_PAGE_NEW_AGC_TAB1      9
#define MODEM_PI_PAGE_NEW_AGC_TAB2      10
#define MODEM_PI_PAGE_NEW_AGC_TAB3      11
#define MODEM_PI_PAGE_OLD_AGC_TAB0      12
#define MODEM_PI_PAGE_W_ADDR0_START       MODEM_PI_PAGE_NEW_AGC_TAB0
#define MODEM_PI_PAGE_W_ADDR0_END         MODEM_PI_PAGE_OLD_AGC_TAB0
#endif
#define TRANS_MODEM_REG(x)              (x)  /* this is used to translate byte address to word address*/

typedef union BTMAC_ISR_STATUS_REG_
{
    struct
    {
        __I uint32_t timer_intr1_intr0_r: 1;
        __I uint32_t bluewiz_intr_r: 1;
        __I uint32_t RESERVED_1: 1;
        __I uint32_t bluewiz_dma_intr_r: 1;
        __I uint32_t pro_intr_r: 1;
        __I uint32_t RESERVED_0: 27;
    };
    UINT32 d32;
} BTMAC_ISR_STATUS_REG;

///* The structure of one fragment of TX Descriptor for BLE only */
//typedef union BZDMA_BLEONLY_TX_DESC_FRAGMENT_
//{
//    struct
//    {
//        UINT32 start_addr: 16;  /* bit[15:0], the start address of buffer SRAM */
//        UINT32 len: 11;         /* bit[26:16], the copied length*/
//        UINT32 rsvd: 4;         /* bit[30:27], reserved */
//        UINT32 isLast: 1;       /* bit[31], is last segment */
//    };
//#ifdef _NEW_BZDMA_FROM_V10_FROM_160223_
//    struct
//    {
//        UINT32 start_addr_ex: 18; /* bit[17:0], the start address of buffer SRAM */
//        UINT32 len_ex: 11;       /* bit[28:18], the copied length*/
//        UINT32 rsvd_ex: 2;       /* bit[30:29], reserved */
//        UINT32 isLast_ex: 1;     /* bit[31], is last segment */
//    };
//#endif
//    UINT32 DWord;
//} BZDMA_BLEONLY_TX_DESC_FRAGMENT, *PBZDMA_BLEONLY_TX_DESC_FRAGMENT;

/* The structure of one segment of RX Descriptor (only use for FW) */
typedef union BZDMA_RX_DESC_SEGMENT_
{
    struct
    {
#ifdef _NEW_BZDMA_FROM_V10_FROM_160223_
        UINT32 addr: 18;  /* the address of destination address */
#else
        UINT32 addr: 16;  /* the address of destination address */
#endif
        UINT32 len: 11;   /* the copied byte length */
        UINT32 flush: 1;  /* flush data from rx fifo */
#ifdef _NEW_BZDMA_FROM_V10_FROM_160223_
        UINT32 rsvd: 2;   /* reserved */
#else
        UINT32 rsvd: 4;   /* reserved */
#endif
    };
    UINT32 DWord;
} BZDMA_RX_DESC_SEGMENT, *PBZDMA_RX_DESC_SEGMENT;

/*====================================================================*/
/* Table 2.3 LLID Defination                                          */
/*====================================================================*/
enum LL_LLID_SET
{
    LL_LLID_RSVD            = 0,
    LL_LLID_DATA_PDU_CONT   = 1,
    LL_LLID_EMPTY_PDU       = 1,
    LL_LLID_DATA_PDU_START  = 2,
    LL_LLID_CTRL_PDU        = 3
};

/* The HW Definition of Tx/Rx PID  */
enum BZDMA_PID_
{
    BZDMA_TX_PID_ACL        = 0,
#ifdef _SUPPORT_CSB_TRANSMITTER_
    BZDMA_TX_PID_CSB        = 1,
#endif
    BZDMA_TX_PID_BCST       = 2,
    BZDMA_TX_PID_SCO        = 3,
#ifdef _NEW_BZDMA_FROM_V7_
    BZDMA_TX_PID_NEW_SCO    = 4,
#endif
    BZDMA_TX_PID_TOTAL      = 3,
    BZDMA_RX_PID_ACL        = 0,
    BZDMA_RX_PID_SCO        = 1,
    BZDMA_RX_PID_TOTAL      = (BZDMA_RX_PID_SCO + 1),
};

/*
 * bit
 * 0:1:         System clock:
 *                  00b: 5MHz; 01b:10MHz; 10b:20MHz; 11b:40MHz
 * 2:            Bluewiz reset:
 *                  defaut: 0,reset state
 *                  1: disable reset
 * 3:4:
 * 5:            Modem reset:
 *                  defaut: 0,reset state
 *                  1: disable reset
 * 6:7:
 * 8:            The extern clock gat of bluewiz:
 *                  defaut: 0,gating state
 *                  1: disable gating
 * 9:            CPU clock 40MHz switch:
 *                  defaut: 0, 32K mode
 *                  1: enable 40MHz mode
 * 10:           Exist bluewiz seep sleep mode:
 *                  defaut: 0
 *                  1: force leaving deep sleep mode
 * 11:31:
**/
typedef union BT_SYS_CTRL_REG_S
{
    UINT32 d32;
    struct
    {
        UINT32 sys_clk                          : 2; //bit0~1
        UINT32 bluewiz_rst                      : 1; //bit2
        UINT32 reserved_3_4                     : 2; //bit3~4
        UINT32 modem_rst                        : 1; //bit5
        UINT32 pon_rst_n                        : 1; //bit6
        UINT32 pro_rst_n                        : 1; //bit7
        UINT32 external_bluewiz_clk_gat         : 1; //bit8
        UINT32 cpu_40mhz_en                     : 1; //bit9
        UINT32 exist_deep_sleep_mode            : 1; //bit10
#ifdef _VENDOR_REG_MUST_UPDATE_FROM_RTL_SVN_VER_17316_
        UINT32 reserved_11_15                   : 5; //bit 11~15
        UINT32 rfc_ret_rst_n                    : 1; //bit16, RFC retention register reset
        UINT32 rfc_rst_n                        : 1; //bit17, RFC off domain register reset
        UINT32 bb_ret_rst_n                     : 1; //bit18, modem retention FF reset
        UINT32 modem_clk_force_on               : 1; //bit19, force modem clock always on
        UINT32 dp_bb_ret_rst_n                  : 1; //bit20
        UINT32 reserved_21_24                   : 5; //bit21~24
        UINT32 zb_ret_rst_n                     : 1; //bit25
        UINT32 pro_clk_en                       : 1; //bit26
        UINT32 rfc_clk_force_on                 : 1; //bit27
        UINT32 bt_le_clk_en                     : 1; //bit28, le clock enable
        UINT32 bt_br_edr_clk_en                 : 1; //bit29, BR EDR clock enable
        UINT32 force_adda_iso_off               : 1; //bit30, force ADDA isolation release.
        UINT32 force_adda_pwron                 : 1; //bit31, force ADDA LDO on
#else
        UINT32 reserved_11_31                   : 21; //bit 11~31
#endif
    } b;
} BT_SYS_CTRL_REG_S_TYPE;

typedef union MODEM_PSD_REPORT_ELEMENT_S
{
    UINT32 d32_array[4];
    struct // RTL8703B, RTL8822B, RTL8821B-MP
    {
        UINT32 psd_avg_dc: 16;              //[15:0]
        UINT32 psd_avg_pos: 16;             ///[31:16]
        UINT32 psd_avg_neg: 16;             //[47:32]
        UINT32 mp_gain_idx: 6;              //[53:48]
        UINT32 gnt_bt_hold: 1;              //[54]
        UINT32 gnt_bt_final: 1;             //[55]

        UINT32 psd_u28_scale: 3;            //[58:56]
        UINT32 gnt_wl_hold: 1;              //[59]
        UINT32 gnt_wl_final: 1;             //[60]
        UINT32 wifi_txon_hold: 1;           //[61]
        UINT32 wifi_txon_final: 1;          //[62]
        UINT32 rsvd1: 1;                    //[63]

        UINT32 psd_avg_dfir_lsb: 16;        //[79:64]
        UINT32 psd_avg_dfir_msb: 4;         //[83:80]
        UINT32 psd_clip_ratio_start: 6;     //[89:84]
        UINT32 psd_clip_ratio_end: 6;       //[95:90]

        UINT32 agc_ste: 4;                  //[99:96]
        UINT32 lna_idx_use: 3;              //[102:100]
        UINT32 bpf_mode_use: 1;             //[103]
        UINT32 gain_idx_use: 5;             //[108:104]
        UINT32 rf_pd_en_use: 1;             //[109]
        UINT32 signature_bit: 2;            //[111:110]    //default 2'b0
        UINT32 rsvd3: 16;                   //[127:112]
    };
} MODEM_PSD_REPORT_ELEMENT_S_TYPE;

typedef UINT8(*PF_ROM_CODE_PATCH_FUNC)(void *buf, ...);
extern PF_ROM_CODE_PATCH_FUNC rcp_BTMAC_Handler;
extern PF_ROM_CODE_PATCH_FUNC rcp_BB_handle_psd_end_intr_func;

extern UINT8 bzdma_supported_le_max_seg_num;
extern MODEM_PSD_REPORT_ELEMENT_S_TYPE *g_modem_psd_report_array;
extern UINT16 g_modem_psd_report_entry_num;
extern uint16_t g_efuse_modem_psd_setting_1;
#define PSD_CHANN_NUM           128
#define PSD_TIMEOUT_DEFAULT     0x3ff
#define PPT_RF_MODE             0
extern int8_t (*calculate_log_from_rssi)(uint16_t, uint8_t, uint8_t);

#define RD_REG_MMIO(type, addr) \
    (*((volatile type*) (addr)))
#define WR_REG_MMIO(type, addr, val) \
    (*((volatile type*) (addr)) = (val))

__STATIC_INLINE BTMAC_ISR_STATUS_REG btmac_isr_status_reg_read(void)
{
    return RD_REG_MMIO(BTMAC_ISR_STATUS_REG, BTMAC_ISR_STATUS_REG_ADDR);
}

#if defined (_NEW_BZDMA_FROM_V10_) || defined (_BT5_0_LE_LONG_RANGE_SUPPORT_)
extern UINT8(*bzdma_send_packet_to_ble_data_ring_fifo)(UINT8 conn_entry, UINT8 llid,
                                                       BZDMA_BLEONLY_TX_DESC_FRAGMENT *pfrag, UINT8 Nfrag,
                                                       UINT8 codeind);
#else
UINT8 bzdma_send_packet_to_ble_data_ring_fifo(UINT8 conn_entry, UINT8 llid,
                                              BZDMA_BLEONLY_TX_DESC_FRAGMENT *pfrag, UINT8 Nfrag);

#endif
UINT8 bzdma_update_fw_rptr_of_ble_data_ring_fifo(UINT8 conn_entry,
                                                 UINT8 free_cnts, UINT8 check_cam);


void bzdma_send_burst_rxcmd_and_wait_complete(BZDMA_RX_DESC_SEGMENT *pRxDesc,
                                              UINT8 cmd_count, UINT8 fifo_type, UINT8 sco_ch_id, UINT8 wait_complete);
void bzdma_flush_ble_data_ring_fifo(UINT8 conn_entry);
void bzdma_reset_ble_data_ring_fifo(UINT8 conn_entry);
void bzdma_ble_set_segment_valid_bitmap(UINT8 entry, UINT16 bitmap);
void bzdma_generic_ble_seg_cmd_clear(UINT8 entry, BOOLEAN clear_all);
void bzdma_wait_rxcmd_complete(void);

/* Prototypes of Baseband Driver Functions exported to other modules */
UINT16 BB_read_baseband_register(UINT16 bb_reg_offset);
void BB_write_baseband_register(UINT16 bb_reg_offset, UINT16 val);
uint16_t BB_read_native_counter(void);
uint32_t BB_read_native_clock(void);

#ifdef _NEW_MODEM_PI_ACCESS_
UCHAR lc_read_modem_pi_page(void);
void lc_write_modem_pi_page(UCHAR modem_page);
extern UINT32(*rtk_read_modem_radio_reg_pi)(UCHAR modem_page, UCHAR addr, UCHAR type);
extern void (*rtk_write_modem_radio_reg_pi)(UCHAR modem_page, UCHAR addr, UCHAR type, UINT16 val);
#endif

void phy_init(UINT8 dlps_flow);
extern void (*ll_hw_init)(UINT8);

extern uint32_t (*platform_rtc_get_counter)(void);

extern uint32_t (*lowerstack_SystemCall)(uint32_t, uint32_t, uint32_t,
                                         uint32_t);

#ifndef COUNT_ARG
#define ARG_T(t)                                t
#define ARG_N(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,a17,a18,a19,a20,a21,N,...)  N
#define ARG_N_HELPER(...)                       ARG_T(ARG_N(__VA_ARGS__))
#define COUNT_ARG1(...)                         ARG_N_HELPER(__VA_ARGS__,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_ARG(...)                          ((COUNT_ARG1(1, ##__VA_ARGS__) - 1)%21)
#endif

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

#endif
