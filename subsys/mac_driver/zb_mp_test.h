/**************************************************************************//**
 * @file     zb_mp_test.h
 * @brief    Zigbee(IEEE802.15.4) MAC/PHY Mass Production test functions macros
 *           and definition.
* @author    JustinWu
 * @version  V1.00
 * @date     2023-01-19
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


#ifndef _ZB_MP_TEST_H_
#define _ZB_MP_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "mac_driver.h"

/**
  \brief  Enumerated definitions for the MAC Frame Control Field in the MAC Header.
*/
typedef enum
{
    MAC_FRM_PEND_OFF                    = 4,        ///< Data pending flag offset in FCF
    MAC_ACK_REQ_OFF                     = 5,        ///< Ack request flag offset in FCF
    MAC_PAN_ID_COMPR_OFF                = 6,        ///< Pan ID compression offset in FCF
    MAC_DEST_ADDR_MODE_OFF              = 10,       ///< Destination address mode offset in FCF
    MAC_SRC_ADDR_MODE_OFF               = 14,       ///< Source address offset in FCF

    // MAC frame control field bit masks
    MAC_FRAME_TYPE_MASK                 = 0x0003,   ///< Data, command, ACK, or beacon frame indicator
    MAC_PENDING_FRAME_MASK              = 0x0010,   ///< Indirect data awaiting retrieval for dest address
    MAC_ACK_REQ_MASK                    = 0x0020,   ///< Ack required for this frame
    MAC_PAN_ID_COMPR_MASK               = 0x0040,   ///< If src and dest PAN ID are same, then include once and set this flag
    MAC_DEST_ADDR_MODE_MASK             = 0x0C00,   ///< Address mode in address field for dest - network or extended addr
    MAC_SRC_ADDR_MODE_MASK              = 0xC000,   ///< Address mode in address field for src - network or extended addr

    // frame type for FCF enums
    MAC_BEACON                          = 0x0,      ///< Beacon frame indicator
    MAC_DATA                            = 0x1,      ///< Data frame indicator
    MAC_ACK                             = 0x2,      ///< ACK frame indicator
    MAC_COMMAND                         = 0x3,      ///< Command frame indicator

    // address mode enums
    NO_PAN_ID_ADDR                      = 0x0,      ///< No address and no PAN ID in address field
    SHORT_ADDR                          = 0x2,      ///< Address field contains network (short) address
    LONG_ADDR                           = 0x3       ///< Address field contains IEEE (long) address
} mac_frm_ctrl_enums_t;

/**
  \brief  Miscellaneous enumerated definitions used by the MAC layer
*/
typedef enum
{
    // broadcast address
    MAC_BROADCAST_ADDR              = 0xFFFF,   ///< MAC broadcast address

    // 802.15.4 Frame Versions
    MAC_802_15_4_2003               = 0x0,      ///< Compliant to 802.15.4 2003 frame format
    MAC_802_15_4_2006               = 0x1,      ///< Compliant to 802.15.4 2006 frame format
    MAC_802_15_4_2015               = 0x2,      ///< Compliant to 802.15.4 frame format (2015 after)

    // scan types
    MAC_ENERGY_SCAN                 = 0x00,     ///< Energy scan identifier
    MAC_ACTIVE_SCAN                 = 0x01,     ///< Active scan identifier
    MAC_ORPHAN_SCAN                 = 0x03,     ///< Orphan scan identifier

    MAC_MAX_CHANNELS                = 16,       ///< Max number of channels supported by MAC
    MAC_PHY_CHANNEL_OFFSET          = 11,       ///< Channel offset. 802.15.4 range is Ch 11-26
    MAC_FCS_LENGTH                  = 2,        ///< Size of the MAC frame checksum
    MAC_LONG_ADDR_LENGTH            = 8,        ///< Size of the MAC frame checksum
} mac_misc_enums_t;

/**
  \brief  This struct holds the info for the MAC frame control field.
*/
typedef struct mac_frm_ctrl_s
{
    uint16_t frame_type: 3;
    uint16_t sec_en: 1;
    uint16_t frame_pending: 1;
    uint16_t ack_req: 1;
    uint16_t panid_compress: 1;
    uint16_t rsv: 1;
    uint16_t seq_num_suppress: 1;
    uint16_t ie_present: 1;
    uint16_t dst_addr_mode: 2;
    uint16_t frame_ver: 2;
    uint16_t src_addr_mode: 2;
} mac_frm_ctrl_t, *pmac_frm_ctrl_t;

typedef struct
{
    uint16_t short_addr;
    uint8_t long_addr[MAC_LONG_ADDR_LENGTH];
    uint8_t mode;
} address_t;

/**
  \brief  This struct contains the fields for the MAC header.
*/
typedef struct
{
    union
    {
        mac_frm_ctrl_t  mac_frm_ctrl;   ///< Frame control field structure
        uint16_t        mac_fcf;        ///< Frame control field hex value
    };
    uint8_t         dsn;            ///< Data sequence number
    uint16_t        dest_pan_id;    ///< Destination network ID
    address_t       dest_addr;      ///< Destination address
    uint16_t        src_pan_id;     ///< Source network ID
    address_t       src_addr;       ///< Source address
} mac_hdr_t;

#define ZIGBEE_MP_MODULE            (0x83)  // Zigbee module ID of MP test HCI command
#if !defined(UNKNOWN_HCI_COMMAND_ERROR)
#define UNKNOWN_HCI_COMMAND_ERROR   (0x01)  // copy from "bt_fw_hci_spec_defines.h"
#endif

/**
  \brief  Defines RF transceiver state
*/
enum zb_mp_test_subcmd_e
{
    ZB_MP_MAC_INIT    = 0x00,   // enable ZB MAC and init MAC driver
    ZB_MP_RESET       = 0x01,   // reset MAC HW & driver
    ZB_MP_TX          = 0x02,   // TX a packet
    ZB_MP_CONT_TX     = 0x03,   // TX packet continuously
    ZB_MP_TX_REPORT   = 0x04,   // report current TX status
    ZB_MP_RX          = 0x05,   // enable RX
    ZB_MP_RX_REPORT   = 0x06,   // report current RX status
    ZB_MP_LOOPBACK    = 0x07,   // do a packet loopback test with the remote golden device
    ZB_MP_ED_GAIN_CAL = 0x08,   // do a ED gain calibration test
    ZB_MP_REG_READ    = 0x09,   // read a ZB MAC register
    ZB_MP_REG_WRITE   = 0x0A,   // write a ZB MAC register
    ZB_PHY_TX_01      = 0x0B,   // ZB PHY Test TX-01
    ZB_PHY_RX_06      = 0x0C,   // ZB PHY Test RX-06 (ED Scan test)
    ZB_PHY_RX_08      = 0x0D,   // ZB PHY Test RX-08 (CCA test)
    ZB_PHY_RX_TO_TX   = 0x0E,   // ZB PHY Test TURN-AROUND-01 (RX packet and TX ACK)
    ZB_PHY_TX_TO_RX   = 0x0F,   // ZB PHY Test TURN-AROUND-02 (TX packet and RX ACK)
    ZB_PHY_TX_CCA     = 0x10,   // ZB PHY Test TX_CCA

    ZB_MP_CMD_MAX_NUM
};

typedef uint8_t (*zigbee_mp_cmd_handle_t)(uint8_t *param, uint8_t *send_cmd_comp_event_flag);
typedef uint8_t (*zigbee_mp_event_handle_t)(uint8_t *param, uint8_t *event_parameter);

typedef struct zb_mp_cmd_handle_entry_s
{
    uint8_t subcmd;
    zigbee_mp_cmd_handle_t cmd_handle;
} zb_mp_cmd_handle_entry_t, *pzb_mp_cmd_handle_entry_t;

enum zb_mp_tx_state_e
{
    ZB_MP_TX_STOPPED = 0,
    ZB_MP_TX_RUNNING = 1,
    ZB_MP_TX_CONT_RUNNING = 2,
    ZB_MP_DUMMY_TX_FOR_CCA = 3,
};

enum zb_mp_rx_state_e
{
    ZB_MP_RX_STOPPED = 0,
    ZB_MP_RX_RUNNING = 1,
};

enum zb_mp_loopback_test_state_e
{
    ZB_MP_LPTST_STOPPED = 0,
    ZB_MP_LPTST_W4_TXDONE = 1,
    ZB_MP_LPTST_W4_RXDONE = 2,

    ZB_MP_LPTST_FAILED = 0xEE,
};

enum zb_mp_test_state_e
{
    ZB_MP_STATE_DISABLED = 0,
    ZB_MP_STATE_READY = 1,

    ZB_MP_STATE_ERROR = 0xEE,
};

#define MP_TEST_TIMER_ID    (0)
#define MAC_REG_MAX_SIZE    (0x09C0UL)

typedef struct zb_mp_tx_state_s
{
    uint8_t tx_running_state;  // MP TX running state: 0/1/2: stopped/transmitting/continuously_tx
    uint8_t tx_retry;   // the retry count of the latest TX
    uint32_t txed_cnt;  // how many packet are transmitted since init or reset
    uint32_t remain_tx_cnt; // how many packet is going to be transmitted
} zb_mp_tx_state_t, *pzb_mp_tx_state_t;

typedef struct zb_mp_rx_state_s
{
    uint8_t rx_running_state;  // MP RX running state: 0/1: stop RX statistic / start RX statistic
    uint8_t channel;    // the channel for RX test
    uint8_t lqi; // the LQI of the latest received packet
    uint32_t rx_ok_cnt;  // how many good packet are received since RX statistic is started
    uint32_t rx_err_cnt;  // how many corrupted packet are received since RX statistic is started
    uint16_t rssi_raw;  // the RSSI raw data of received packet
} zb_mp_rx_state_t, *pzb_mp_rx_state_t;

typedef void (*zb_mp_timeout_callback_t)(void);
typedef uint8_t (*zb_mp_handle_cmd_t)(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                      uint8_t *send_cmd_comp_event_flag);
typedef uint8_t (*zb_mp_handle_event_t)(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                        uint8_t *event_parameter);
typedef uint8_t (*loopback_test_cmd_patch_t)(uint8_t *param, uint8_t *send_cmd_comp_event_flag);

typedef struct zb_mp_test_adapter_s
{
    mac_driver_t zb_mac_drv;
    volatile zb_mp_tx_state_t mp_tx_state;
    volatile zb_mp_rx_state_t mp_rx_state;
    zb_mp_timeout_callback_t test_timeout_callback;
    zb_mp_handle_cmd_t mp_handle_cmd;
    zb_mp_handle_event_t mp_handle_event;
    uint8_t tx_buf[128];
    uint8_t rx_buf[144];
    uint32_t reg_rd_addr;
    uint8_t reg_rd_len;
    uint8_t hci_cmd_status;
    uint8_t dsn; // data sequence number
    volatile uint8_t mp_lpbk_test_state;
    int8_t tx_gain_offset;
    int8_t ed_scan_gain_offset;
    loopback_test_cmd_patch_t loopback_test_cmd_patch;
    volatile uint32_t txsr;
    volatile uint8_t mp_test_state;
    uint8_t ed_scan_status; // ED scan execution result
    int8_t ed_scan_peak;
    uint8_t cca_busy_cnt;
    uint8_t cca_clear_cnt;
    uint8_t rf_mode;
} zb_mp_test_adapter_t, *pzb_mp_test_adapter_t;

// param: HCI MP command parameters
// send_cmd_comp_event_flag: to return whether send HCI event for this command
// return: Status of HCI event
uint8_t zigbee_mp_handle_cmd_rom(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                 uint8_t *send_cmd_comp_event_flag);
// param: HCI MP command parameters
// event_parameter: the buffer to fill event parameters
// return: event parameters increased (number of parameters data is filled, in bytes)
uint8_t zigbee_mp_handle_event_rom(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                   uint8_t *event_parameter);

extern uint8_t zigbee_mp_handle_cmd(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                    uint8_t *send_cmd_comp_event_flag);
extern uint8_t zigbee_mp_handle_event(uint8_t module_id, uint8_t subcmd, uint8_t *param,
                                      uint8_t *event_parameter);
void zb_mp_test_init(zb_mp_test_adapter_t *padapt);

// Start work queue based continuous TX mechanism for AT+ZIGBTEST
void zb_mp_cont_tx_kick(void);
#ifdef __cplusplus
}
#endif

#endif    // end of #ifndef _ZB_MP_TEST_H_
