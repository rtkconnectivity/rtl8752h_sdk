/**************************************************************************//**
 * @file     zb_mac_ipc.h
 * @brief    Inter-Processor communication definition for IEEE802.15.4 MAC/PHY
 *           control cross platorm.
* @author    JustinWu
 * @version  V1.00
 * @date     2023-05-31
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

#ifndef _ZB_MAC_IPC_H_
#define _ZB_MAC_IPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mac_port_macro.h"
#include "private/mac_reg.h"
#include "private/mac_reg_type.h"
#include "private/mac_reg_mask.h"
#include "mac_data_type.h"
#include "frame_802154.h"

#define ZB_IPC_CMD_TYPE_BASE    0xC000
#define ZB_IPC_EVT_TYPE_BASE    0xE000
#define IPC_H2C_CMD(cmd)        (ZB_IPC_CMD_TYPE_BASE|cmd)
#define IPC_C2H_EVENT(evt)      (ZB_IPC_EVT_TYPE_BASE|evt)

#define ZB_IPC_DEV_TASK_STACK_SIZE      (1024)
#define ZB_IPC_DEV_TASK_PRIORITY        (2)
#define ZB_IPC_HOST_TASK_STACK_SIZE     (4096)
#define ZB_IPC_HOST_TASK_PRIORITY       (2)

#define RX_PKT_BUF_ZIZE                 (144)   // size of a RX packet buffer on IPC device side
#define RX_PKT_BUF_NUM                  (4)     // number of buffer for RX packet on IPC device side

#define IPC_CMD_BUF_SIZE                (144)
#define IPC_EVT_BUF_SIZE                (180)
#define IPC_CMD_COMP_EVT_BUF_SIZE       (32)

/* note: the IPC H2C IRQ priority should be equal to the ZB MAC IRQ's priority to prevent
   ISR is prempted by the other and then cause race condition  */
#define IPC_H2C_IRQ_PRIORITY            (ZB_IRQ_PRIPROTY)     // IPC IRQ on the device side
#define IPC_C2H_IRQ_PRIORITY            (2)     /* IPC IRQ priority on the host side,
                                                   0 is the highest */
typedef enum zb_ipc_sts_e
{
    ZB_IPC_STS_SUCCESS = 0,
    ZB_IPC_STS_UNKNOWN_CMD = 1,
    ZB_IPC_STS_INVALID_TID = 2,
    ZB_IPC_STS_SEND_CMD_ERR = 3,
    ZB_IPC_STS_INVALID_PARA = 4,
    ZB_IPC_STS_UNSUPPORT = 5,

} zb_ipc_sts_t, *pzb_ipc_sts_t;

/**
  \brief  Defines the Command type of the ZB MAC control over IPC
*/
typedef enum zb_ipc_cmd_e
{
    ZB_CMD_RESERVED              = 0x000,    ///< Reserved
    ZB_CMD_INIT                  = 0x001,    ///< MAC Init
    ZB_CMD_RESET                 = 0x002,    ///< MAC Reset
    ZB_CMD_REG_READ              = 0x003,    ///< Read MAC register
    ZB_CMD_REG_WRITE             = 0x004,    ///< Write MAC register
    ZB_CMD_REG_BITS_WRITE        = 0x005,    ///< Write MAC register bits
    ZB_CMD_GET_MAC_TIME          = 0x006,    ///< get current MAC time
    ZB_CMD_GET_BT_US             = 0x007,    ///< get current BT time
    ZB_CMD_GET_BT_CLK            = 0x008,    ///< get current BT clock
    ZB_CMD_SET_BT_TIMER_ABS      = 0x009,    ///< set BT time interrupt by a given BT time
    ZB_CMD_START_BT_TIMER        = 0x00A,    ///< start a BT timer with timeout offset from current time
    ZB_CMD_STOP_BT_TIMER         = 0x00B,    ///< stop a BT timer
    ZB_CMD_RESET_RF              = 0x00C,    ///< Reset RF
    ZB_CMD_SET_RADIO_STATE       = 0x00D,    ///< change Radio On/Off state
    ZB_CMD_GET_RADIO_STATE       = 0x00E,    ///< Get current Radio On/Off state
    ZB_CMD_SET_PANID             = 0x00F,    ///< Set PANID
    ZB_CMD_SET_SHORT_ADDR        = 0x010,    ///< Set 16-bits short address
    ZB_CMD_SET_LONG_ADDR         = 0x011,    ///< Set 64-bits long address
    ZB_CMD_SET_COORD_SADDR       = 0x012,    ///< Set associated coordinator 16-bits short address
    ZB_CMD_SET_COORD_LADDR       = 0x013,    ///< Set associated coordinator 64-bits long address
    ZB_CMD_SET_GROUP_ADDR        = 0x014,    ///< Set group 64-bits long address
    ZB_CMD_SET_CHANNEL           = 0x015,    ///< Set Channel
    ZB_CMD_GET_CHANNEL           = 0x016,    ///< Get current Channel
    ZB_CMD_SET_TX_GAIN           = 0x017,    ///< Set TX power
    ZB_CMD_SET_TX_CSMA           = 0x018,    ///< enable/disable TX CSMA-CA
    ZB_CMD_SET_CCA_MODE          = 0x019,    ///< set the CCA mode of CSMA-CA
    ZB_CMD_EDSCAN                = 0x01A,    ///< Start an ED scan
    ZB_CMD_EDSCAN_STOP           = 0x01B,    ///< Stop a running ED scan
    ZB_CMD_UPD_TX_PRE_TRIG_TIME  = 0x01C,    ///< Update given time TX pre-trigger time
    ZB_CMD_TRIG_TXN              = 0x01D,    ///< Trigger normal FIFO TX
    ZB_CMD_TRIG_GIVEN_TIME_TX    = 0x01E,    ///< Trigger given time TX
    ZB_CMD_LOAD_TXN              = 0x01F,    ///< Load MPDU to normal TX FIFO
    ZB_CMD_LOAD_NONCE            = 0x020,    ///< Load 13-bytes security nonce
    ZB_CMD_LOAD_KEY              = 0x021,    ///< Load security key buffer
    ZB_CMD_SET_SEC_LV            = 0x022,    ///< Set security level for specified TX/RX FIFO
    ZB_CMD_ADD_SADDR_FILTER      = 0x023,    ///< Add short source address match filter entry
    ZB_CMD_ADD_LADDR_FILTER      = 0x024,    ///< Add long source address match filter entry
    ZB_CMD_DEL_SADDR_FILTER      = 0x025,    ///< delete a short source address match filter entry
    ZB_CMD_DEL_LADDR_FILTER      = 0x026,    ///< delete a long source address match filter entry
    ZB_CMD_DEL_ALL_SADDR_FILTER  = 0x027,    ///< delete all short source address match filter entries
    ZB_CMD_DEL_ALL_LADDR_FILTER  = 0x028,    ///< delete all long source address match filter entries
    ZB_CMD_TXN_TRANSMIT          = 0x029,    ///< transmit a packet through the normal TX FIFO
    ZB_CMD_SET_CCA_CS_THRES      = 0x02A,    ///< set the CCA CS mode threshold
    ZB_CMD_SET_CCA_ED_THRES      = 0x02B,    ///< set the CCA ED mode threshold
    ZB_CMD_READ_FIFO             = 0x02C,    ///< read MAC FIFO
    ZB_CMD_UPPER_CIPHER          = 0x02D,    ///< upper layer cipher
    ZB_CMD_UPPER_DECIPHER        = 0x02E,    ///< upper layer de-cipher
    ZB_CMD_PM_INIT               = 0x02F,    ///< power manager init
    ZB_CMD_PM_SET_MODE           = 0x030,    ///< power manager set power state
    ZB_CMD_PORTING_EXT           = 0x031,    ///< extend command for upperlay stack porting

    ZB_CMD_MAX_TYPE
} zb_ipc_cmd_t, *pzb_ipc_cmd_t;

/**
  \brief  Defines the Event type of the ZB MAC control over IPC
*/
typedef enum zb_ipc_evt_e
{
    ZB_EVT_CMD_COMP              = 0x000,    ///< IPC command complete
    ZB_EVT_BT_TIMER_INT          = 0x001,    ///< BT timer interrupt event
    ZB_EVT_TXNIF                 = 0x002,    ///< TXN done interrupt event
    ZB_EVT_RX_PKT                = 0x003,    ///< packet received event
    ZB_EVT_ED_SCAN_DONE          = 0x004,    ///< ED scan done evet
    ZB_EVT_PM_ENTER_DLPS         = 0x005,    ///< entering DLPS evet
    ZB_EVT_PM_EXIT_DLPS          = 0x006,    ///< exiting DLPS evet
    ZB_EVT_PORTING_EXT           = 0x007,    ///< extend event for upperlayer stack porting

    ZB_EVT_MAX_TYPE
} zb_ipc_evt_t, *pzb_ipc_evt_t;

typedef enum zb_reg_size_e
{
    ZB_REG_8b = 0,      ///< 8 bits
    ZB_REG_16b = 1,     ///< 16 bits
    ZB_REG_32b = 2,     ///< 32 bits
} zb_reg_size_t, *pzb_reg_size_t;

typedef enum zb_radio_state_e
{
    ZB_RADIO_OFF = 0,   ///< Radio Off
    ZB_RADIO_ON = 1,    ///< Radio On
} zb_radio_state_t, *pzb_radio_state_t;

typedef enum zb_csma_state_e
{
    ZB_CSMA_OFF = 0,   ///< TXN CSMA-CA Off
    ZB_CSMA_ON = 1,    ///< TXN CSMA-CA On
} zb_csma_state_t, *pzb_csma_state_t;

typedef enum zb_ed_scan_mode_e
{
    ZB_ED_SCAN_SYNC = 0,        ///< block CPU and wait scan done
    ZB_ED_SCAN_NON_SYNC = 1,    ///< notif scan done by interrupt
} zb_ed_scan_mode_t, *pzb_ed_scan_mode_t;

typedef enum zb_key_buf_id_e
{
    ZB_KEY_BUF_ID_TXN = 0,        ///< Key buffer ID for TXN FIFO
    ZB_KEY_BUF_ID_TXG1 = 1,       ///< Key buffer ID for TXG1 FIFO
    ZB_KEY_BUF_ID_TXG2 = 2,       ///< Key buffer ID for TXG2 FIFO
    ZB_KEY_BUF_ID_RX = 3,         ///< Key buffer ID for RX FIFO
} zb_key_buf_id_t, *pzb_key_buf_id_t;

typedef enum zb_sec_id_e
{
    ZB_SEC_ID_TXN = 0,      ///< security ID for TXN FIFO
    ZB_SEC_ID_RX = 1,       ///< security ID for RX FIFO
    ZB_SEC_ID_BCN = 2,      ///< security ID for TX Beacon FIFO
    ZB_SEC_ID_TXG1 = 3,     ///< security ID for TX G1 FIFO
    ZB_SEC_ID_TXG2 = 4,     ///< security ID for TX G2 FIFO
} zb_sec_id_t, *pzb_sec_id_t;

/* Zigbee IPC Command Header */
typedef struct __attribute__((packed)) zb_ipc_cmd_hdr_s
{
    uint16_t cmd;   /*!< the command type */
    uint8_t seq_no;   /*!< the sequence number */
    uint8_t len;   /*!< the length, in byte, of command parameters */
} zb_ipc_cmd_hdr_t, *pzb_ipc_cmd_hdr_t;

/* Zigbee IPC Event Header */
typedef struct __attribute__((packed)) zb_ipc_evt_hdr_s
{
    uint16_t event;  /*!< the event type */
    uint8_t seq_no;   /*!< the sequence number */
    uint8_t len;  /*!< the length, in byte, of event parameters */
} zb_ipc_evt_hdr_t, *pzb_ipc_evt_hdr_t;

typedef struct __attribute__((packed)) zb_ipc_cmd_comp_evt_hdr_s
{
    uint16_t cmd_type;  /*!< the completed command type */
    uint8_t seq_no;   /*!< the sequence number of completed command */
    uint8_t reserved;
} zb_ipc_cmd_comp_evt_hdr_t, *pzb_ipc_cmd_comp_evt_hdr_t;

/* MAC Init command parameters */
typedef struct mac_init_cmd_s
{
    uint8_t attr_valid; /*!< is this init command with a attribute configuration */
    uint8_t reserved[3];
    mac_attribute_t mac_attr;   /*!< the MAC attribute to init the MAC driver */
} mac_init_cmd_t, *pmac_init_cmd_t;

/* Read MAC Register */
typedef struct __attribute__((packed)) mac_read_reg_cmd_s
{
    uint32_t reg_addr;  /*!< the register address  */
    uint8_t reg_sz;  /*!< 1/2/4 bytes reg to read  */
} mac_read_reg_cmd_t, *pmac_read_reg_cmd_t;

typedef struct __attribute__((packed)) mac_read_reg_evt_s
{
    uint32_t reg_addr;  /*!< the register address  */
    uint32_t reg_value;
    int8_t status;  /*!< reg read status */
} mac_read_reg_evt_t, *pmac_read_reg_evt_t;

/* Write MAC Register */
typedef struct __attribute__((packed)) mac_write_reg_cmd_s
{
    uint32_t reg_addr;  /*!< the register offset address  */
    uint32_t reg_val;   /*!< reg value to write  */
    uint8_t reg_sz;  /*!< 1/2/4 bytes reg to read  */
} mac_write_reg_cmd_t, *pmac_write_reg_cmd_t;

typedef struct __attribute__((packed)) mac_write_reg_evt_s
{
    uint32_t reg_addr;  /*!< the register address  */
    uint32_t reg_value;
    int8_t status;  /*!< reg write status */
} mac_write_reg_evt_t, *pmac_write_reg_evt_t;

/* Write MAC Register bits */
typedef struct __attribute__((packed)) mac_write_reg_bits_cmd_s
{
    uint32_t reg_addr;  /*!< the register offset address  */
    uint32_t mask;  /*!< mask for the bits to be clean  */
    uint32_t reg_val;   /*!< bits value to write  */
    uint8_t reg_sz;  /*!< 1/2/4 bytes reg to read  */
} mac_write_reg_bits_cmd_t, *pmac_write_reg_bits_cmd_t;

/* Get current MAC time */
typedef struct __attribute__((packed)) mac_get_time_evt_s
{
    uint64_t cur_time;  /*!< current MAC time is us */
} mac_get_time_evt_t, *pmac_get_time_evt_t;

/* Get current BT time */
typedef struct __attribute__((packed)) mac_get_bt_time_evt_s
{
    uint32_t bt_time;  /*!< current BT time is us */
} mac_get_bt_time_evt_t, *pmac_get_bt_time_evt_t;

/* Get current BT clock */
typedef struct __attribute__((packed)) mac_get_bt_clk_evt_s
{
    mac_bt_clk_t bt_clk;  /*!< current BT time is us */
} mac_get_bt_clk_evt_t, *pmac_get_bt_clk_evt_t;

/* Set BT timer interrupt by a given absolute BT time value */
typedef struct __attribute__((packed)) mac_set_bt_timer_abs_cmd_s
{
    uint8_t timer_id;   /*!< time timer ID to be use */
    uint8_t reserved1[3];
    uint32_t hit_time;  /*!< the time to hit the interrupt */
} mac_set_bt_timer_abs_cmd_t, *pmac_set_bt_timer_abs_cmd_t;

typedef struct __attribute__((packed)) mac_set_bt_timer_abs_evt_s
{
    int8_t status;  /*!< set BT time interrupt status */
} mac_set_bt_timer_abs_evt_t, *pmac_set_bt_timer_abs_evt_t;

/* start BT timer by an offset from current time */
typedef struct __attribute__((packed)) mac_start_bt_timer_cmd_s
{
    uint8_t timer_id;   /*!< time timer ID to be use */
    uint8_t is_periodical;  /*!< is a periodocal time */
    uint8_t reserved1[2];
    uint32_t interval;  /*!< the time interval, in us, to hit the interrupt */
} mac_start_bt_timer_cmd_t, *pmac_start_bt_timer_cmd_t;

typedef struct __attribute__((packed)) mac_set_bt_timer_evt_s
{
    int8_t status;  /*!< set BT time interrupt status */
} mac_set_bt_timer_evt_t, *pmac_set_bt_timer_evt_t;

/* stop a BT timer */
typedef struct __attribute__((packed)) mac_stop_bt_timer_cmd_s
{
    uint8_t timer_id;   /*!< time timer ID to be use */
} mac_stop_bt_timer_cmd_t, *pmac_stop_bt_timer_cmd_t;

typedef struct __attribute__((packed)) mac_stop_bt_timer_evt_s
{
    int8_t status;  /*!< stop BT time interrupt status */
} mac_stop_bt_timer_evt_t, *pmac_stop_bt_timer_evt_t;

/* Reset RF */
typedef struct __attribute__((packed)) mac_reset_rf_evt_s
{
    int8_t status;  /*!< stop BT time interrupt status */
} mac_reset_rf_evt_t, *pmac_reset_rf_evt_t;

/* Change Radio On/Off State */
typedef struct __attribute__((packed)) mac_set_radio_state_cmd_s
{
    uint8_t radio_state;   /*!< radio_on/off state */
} mac_set_radio_state_cmd_t, *pmac_set_radio_state_cmd_t;

typedef struct __attribute__((packed)) mac_set_radio_state_evt_s
{
    int8_t status;  /*!< change radio state status */
    uint8_t radio_state;    /*!< current radio state */
} mac_set_radio_state_evt_t, *pmac_set_radio_state_evt_t;

/* Get current Radio On/Off State */
typedef struct __attribute__((packed)) mac_get_radio_state_evt_s
{
    uint8_t radio_state;    /*!< current radio state */
} mac_get_radio_state_evt_t, *pmac_get_radio_state_evt_t;

/* Set PANID */
typedef struct __attribute__((packed)) mac_set_panid_cmd_s
{
    uint16_t panid;   /*!< the PAN ID to be set */
} mac_set_panid_cmd_t, *pmac_set_panid_cmd_t;

/* Set short address */
typedef struct __attribute__((packed)) mac_set_short_addr_cmd_s
{
    uint16_t addr;   /*!< the short address be set */
} mac_set_short_addr_cmd_t, *pmac_set_short_addr_cmd_t;

/* Set long address */
typedef struct __attribute__((packed)) mac_set_long_addr_cmd_s
{
    uint8_t addr[8];   /*!< the long address be set */
} mac_set_long_addr_cmd_t, *pmac_set_long_addr_cmd_t;

/* Set coordinator short address */
typedef struct __attribute__((packed)) mac_set_coord_saddr_cmd_s
{
    uint16_t addr;   /*!< the coordinator short address be set */
} mac_set_coord_saddr_cmd_t, *pmac_set_coord_saddr_cmd_t;

/* Set coordinator long address */
typedef struct __attribute__((packed)) mac_set_coord_laddr_cmd_s
{
    uint8_t addr[8];   /*!< the coordinator long address be set */
} mac_set_coord_laddr_cmd_t, *pmac_set_coord_laddr_cmd_t;

/* Set group address */
typedef struct __attribute__((packed)) mac_set_group_addr_cmd_s
{
    uint8_t addr[8];   /*!< the group address be set */
} mac_set_group_addr_cmd_t, *pmac_set_group_addr_cmd_t;

/* Set channel */
typedef struct __attribute__((packed)) mac_set_channel_cmd_s
{
    uint8_t channel;   /*!< the channel number to be set */
} mac_set_channel_cmd_t, *pmac_set_channel_cmd_t;

typedef struct __attribute__((packed)) mac_set_channel_evt_s
{
    int8_t status;  /*!< set channel status */
    uint8_t channel;    /*!< current channel number */
} mac_set_channel_evt_t, *pmac_set_channel_evt_t;

/* Get current channel */
typedef struct __attribute__((packed)) mac_get_channel_evt_s
{
    uint8_t channel;    /*!< current channel number */
} mac_get_channel_evt_t, *pmac_get_channel_evt_t;

/* Set TX Gain */
typedef struct __attribute__((packed)) mac_set_tx_gain_cmd_s
{
    uint8_t tx_power;   /*!< the TX power, in dBm, to be set */
} mac_set_tx_gain_cmd_t, *pmac_set_tx_gain_cmd_t;

typedef struct __attribute__((packed)) mac_set_tx_gain_evt_s
{
    int8_t status;  /*!< set TX power status */
    uint8_t tx_gain;    /*!< current TX gain setting */
} mac_set_tx_gain_evt_t, *pmac_set_tx_gain_evt_t;

/* Enable/Disable TXN CSMA */
typedef struct __attribute__((packed)) mac_set_tx_csma_cmd_s
{
    uint8_t csma_en;   /*!< is enable TXN CSMA-CA */
} mac_set_tx_csma_cmd_t, *pmac_set_tx_csma_cmd_t;

typedef struct __attribute__((packed)) mac_set_tx_csma_evt_s
{
    uint8_t csma_en;    /*!< current TXN CSMA-CA On/Off state */
} mac_set_tx_csma_evt_t, *pmac_set_tx_csma_evt_t;

/* Set CCA mode */
typedef struct __attribute__((packed)) mac_set_cca_mode_cmd_s
{
    uint8_t cca_mode;   /*!< the CCA mode to be set */
} mac_set_cca_mode_cmd_t, *pmac_set_cca_mode_cmd_t;

typedef struct __attribute__((packed)) mac_set_cca_mode_evt_s
{
    int8_t status;  /*!< set CCA mode status */
    uint8_t cca_mode;    /*!< current CCA mode */
} mac_set_cca_mode_evt_t, *pmac_set_cca_mode_evt_t;

/* Start an ED scan */
typedef struct __attribute__((packed)) mac_ed_scan_cmd_s
{
    uint32_t scan_duration;   /*!< the ED scan time, in us */
    uint8_t scan_mode;   /*!< blocked and wiat scan done or notify scan done by interrupt */
} mac_ed_scan_cmd_t, *pmac_ed_scan_cmd_t;

typedef struct __attribute__((packed)) mac_ed_scan_evt_s
{
    int8_t status;  /*!< ED scan status */
    int8_t ed_peak_lev; /*!< peak value of ED scan */
    int8_t ed_avrg_lev; /*!< average value of ED scan */
} mac_ed_scan_evt_t, *pmac_ed_scan_evt_t;

/* TXN Trigger */
typedef struct __attribute__((packed)) mac_trig_txn_cmd_s
{
    uint8_t ack_req;   /*!< is request for ACK */
    uint8_t sec_en;    /*!< is a secure frame TX */
} mac_trig_txn_cmd_t, *pmac_trig_txn_cmd_t;

typedef struct __attribute__((packed)) mac_trig_txn_evt_s
{
    int8_t status;  /*!< trigger TXN status */
} mac_trig_txn_evt_t, *pmac_trig_txn_evt_t;

/* Given time TX Trigger */
typedef struct __attribute__((packed)) mac_trig_give_time_tx_cmd_s
{
    uint8_t ack_req;   /*!< is request for ACK */
    uint8_t sec_en;    /*!< is a secure frame TX */
    uint8_t do_cca;    /*!< is do a CCA before TX */
    uint8_t reserved1;
    uint32_t tx_time;   /*!< the time to start TX packet to the air */
} mac_trig_give_time_tx_cmd_t, *pmac_trig_give_time_tx_cmd_t;

typedef struct __attribute__((packed)) mac_trig_give_time_tx_evt_s
{
    int8_t status;  /*!< ED scan status */
} mac_trig_give_time_tx_evt_t, *pmac_trig_give_time_tx_evt_t;

/* Load TXN FIFO */
typedef struct __attribute__((packed)) mac_load_txn_cmd_s
{
    uint8_t hdr_len;    /*!< header length */
    uint8_t frm_len;    /*!< frame length */
    uint8_t mpdu[MAC_MAX_TX_FRM_LEN];  /*!< TX MPDU data */
} mac_load_txn_cmd_t, *pmac_load_txn_cmd_t;

/* Load TXN FIFO */
typedef struct __attribute__((packed)) mac_txn_transmit_cmd_s
{
    uint8_t hdr_len;    /*!< header length */
    uint8_t frm_len;    /*!< frame length */
    uint8_t ack_req;   /*!< is request for ACK */
    uint8_t sec_en;    /*!< is a secure frame TX */
    uint8_t mpdu[MAC_MAX_TX_FRM_LEN];  /*!< TX MPDU data */
} mac_txn_transmit_cmd_t, *pmac_txn_transmit_cmd_t;

typedef struct __attribute__((packed)) mac_txn_transmit_evt_s
{
    int8_t status;  /*!< trigger TXN status */
} mac_txn_transmit_evt_t, *pmac_txn_transmit_evt_t;

typedef struct __attribute__((packed)) mac_load_txn_evt_s
{
    int8_t status;  /*!< load TXN MPDU status */
} mac_load_txn_evt_t, *pmac_load_txn_evt_t;

/* Load security Nonce */
typedef struct __attribute__((packed)) mac_load_nonce_cmd_s
{
    uint8_t nonce[MAC_NONCE_LENGTH];    /*!< nonce data to load */
} mac_load_nonce_cmd_t, *pmac_load_nonce_cmd_t;

/* Load security key */
typedef struct __attribute__((packed)) mac_load_key_cmd_s
{
    uint8_t key[MAC_KEY_LENGTH];    /*!< key data to load */
    uint8_t key_buf_id;   /*!< which key to be load */
} mac_load_key_cmd_t, *pmac_load_key_cmd_t;

/* Set security level */
typedef struct __attribute__((packed)) mac_set_sec_lv_cmd_s
{
    uint8_t sec_level;    /*!< security level */
    uint8_t sec_id;   /*!< which FIFO's security level to set */
} mac_set_sec_lv_cmd_t, *pmac_set_sec_lv_cmd_t;

/* Add short source address filter entry */
typedef struct __attribute__((packed)) mac_add_saddr_filter_cmd_s
{
    uint16_t short_addr;    /*!< source short address */
    uint16_t panid; /*!< panid */
} mac_add_saddr_filter_cmd_t, *pmac_add_saddr_filter_cmd_t;

typedef struct __attribute__((packed)) mac_add_saddr_filter_evt_s
{
    int8_t status;  /*!< add short source address match filter status */
} mac_add_saddr_filter_evt_t, *pmac_add_saddr_filter_evt_t;

/* Add long source address filter entry */
typedef struct __attribute__((packed)) mac_add_laddr_filter_cmd_s
{
    uint8_t addr[8];    /*!< source long address */
} mac_add_laddr_filter_cmd_t, *pmac_add_laddr_filter_cmd_t;

typedef struct __attribute__((packed)) mac_add_laddr_filter_evt_s
{
    int8_t status;  /*!< add short source address match filter status */
} mac_add_laddr_filter_evt_t, *pmac_add_laddr_filter_evt_t;

/* Delete short source address filter entry */
typedef struct __attribute__((packed)) mac_del_saddr_filter_cmd_s
{
    uint16_t short_addr;    /*!< source short address */
    uint16_t panid; /*!< panid */
} mac_del_saddr_filter_cmd_t, *pmac_del_saddr_filter_cmd_t;

typedef struct __attribute__((packed)) mac_del_saddr_filter_evt_s
{
    int8_t status;  /*!< delete short source address match filter status */
} mac_del_saddr_filter_evt_t, *pmac_del_saddr_filter_evt_t;

/* Add long source address filter entry */
typedef struct __attribute__((packed)) mac_del_laddr_filter_cmd_s
{
    uint8_t addr[8];    /*!< source long address */
} mac_del_laddr_filter_cmd_t, *pmac_del_laddr_filter_cmd_t;

typedef struct __attribute__((packed)) mac_del_laddr_filter_evt_s
{
    int8_t status;  /*!< delete long source address match filter status */
} mac_del_laddr_filter_evt_t, *pmac_del_laddr_filter_evt_t;

typedef struct __attribute__((packed)) mac_set_cca_cs_threshold_cmd_s
{
    uint8_t thres;  /*!< threshold of Carrier Sense */
    uint8_t times;  /*!< the "total" times of symbol scanning result > thres is over this value,
                         then mark the channel is busy */
    uint8_t cont_times; /*!< the "continuous" times of symbol scanning result > thres is over
                             this value, then mark the channel is busy */
} mac_set_cca_cs_threshold_cmd_t, *pmac_set_cca_cs_threshold_cmd_t;

typedef struct __attribute__((packed)) mac_set_cca_ed_threshold_cmd_s
{
    uint8_t thres;  /*!< threshold of Energy Detection, in unit of detection resolution */
    uint8_t res;  /*!< resolution of Energy Detection: 0/1/2/3: 0.25/0.5/1/2 dBm */
} mac_set_cca_ed_threshold_cmd_t, *pmac_set_cca_ed_threshold_cmd_t;

typedef struct __attribute__((packed)) mac_read_fifo_cmd_s
{
    uint8_t fifo_id;  /*!< the FIFO to be read */
    uint8_t len;  /*!< the length to read */
} mac_read_fifo_cmd_t, *pmac_read_fifo_cmd_t;

typedef struct __attribute__((packed)) mac_read_fifo_evt_s
{
    uint8_t fifo_id;  /*!< the read FIFO ID */
    uint8_t frame_len;  /*!< the read frame length */
    int8_t status;
} mac_read_fifo_evt_t, *pmac_read_fifo_evt_t;

typedef struct __attribute__((packed)) mac_upper_cipher_cmd_s
{
    uint8_t sec_mode;   /*!< the security cipher mode */
    uint8_t nonce[SEC_NONCE_LEN]; /*!< the cipher nonce */
    uint8_t key[SEC_KEY_LEN];  /*!< the encryption key */
} mac_upper_cipher_cmd_t, *pmac_upper_cipher_cmd_t;

typedef struct __attribute__((packed)) mac_upper_cipher_evt_s
{
    int8_t status;  /*!< the upper layer cipher status */
} mac_upper_cipher_evt_t, *pmac_upper_cipher_evt_t;

typedef struct __attribute__((packed)) mac_upper_decipher_cmd_s
{
    uint8_t sec_mode;   /*!< the security cipher mode */
    uint8_t nonce[SEC_NONCE_LEN]; /*!< the cipher nonce */
    uint8_t key[SEC_KEY_LEN];  /*!< the decryption key */
} mac_upper_decipher_cmd_t, *pmac_upper_decipher_cmd_t;

typedef struct __attribute__((packed)) mac_upper_decipher_evt_s
{
    int8_t status;  /*!< the upper layer cipher status */
} mac_upper_decipher_evt_t, *pmac_upper_decipher_evt_t;

typedef struct __attribute__((packed)) mac_pm_init_cmd_s
{
    uint8_t init_wake; // is init the wakeup time with wakeup_time_us?
    uint8_t init_wake_cur; // is init the wakeup time with current time?
    uint8_t init_wake_offset;  // is init the wakeup time with current time + wakeup_interval_us?
    uint8_t wake_interval_en;     // is wakeup periodical interval enabled?
    uint32_t wakeup_time_us;  // the time stamp of next wakeup time, in unit of us by BT clock
    uint32_t wakeup_interval_us;  // the sleep interval, in unit of us
} mac_pm_init_cmd_t, *pmac_pm_init_cmd_t;

typedef struct __attribute__((packed)) mac_pm_init_evt_s
{
    uint32_t wakeup_time_us;  // the time stamp of next wakeup time, in unit of us
    int8_t status;  /*!< the PM init status */
} mac_pm_init_evt_t, *pmac_pm_init_evt_t;

typedef struct __attribute__((packed)) mac_pm_set_pwr_mod_cmd_s
{
    uint8_t mode;  /*!< power mode to set */
    uint8_t init_wake; // is init the wakeup time with wakeup_time_us?
    uint8_t init_wake_offset;  // is init the wakeup time with current time + wakeup_time_us?
    uint8_t reserved1;
    uint32_t wakeup_time_us;  // the time stamp of next wakeup time, in unit of us by BT clock
} mac_pm_set_pwr_mod_cmd_t, *pmac_pm_set_pwr_mod_cmd_t;

typedef struct __attribute__((packed)) mac_pm_set_pwr_mod_evt_s
{
    uint8_t status;  /*!< set power mode status */
    uint8_t mode;  /*!< current power mode */
    uint8_t reserved[2];
    uint32_t wakeup_time_us;  // the time stamp of next wakeup time, in unit of us
} mac_pm_set_pwr_mod_evt_t, *pmac_pm_set_pwr_mod_evt_t;

//// Event from KM0 to KM4
/* Command complete event */
typedef struct __attribute__((packed)) mac_cmd_complete_evt_s
{
    uint16_t cmd_type;  /*!< completed command type */
} mac_cmd_complete_evt_t, *pmac_cmd_complete_evt_t;

/* BT Timer interrupt event */
typedef struct __attribute__((packed)) mac_bt_timeout_evt_s
{
    uint8_t timer_id;  /*!< the timer ID trigger this event */
} mac_bt_timeout_evt_t, *pmac_bt_timeout_evt_t;

/* TX Done event */
typedef struct __attribute__((packed)) mac_txif_evt_s
{
    uint32_t txsr;           /*!< TX Status Register                         */
    uint32_t tx_term_sts;    /*!< TX Termed (Lost PHY Grant) Status Register */
    uint32_t tx_frm_len_err; /*!< TX Frame Length Error Status Register      */
    uint32_t tx_given_time_status;  /*!< Given Time TX Status Register       */
} mac_txif_evt_t, *pmac_txif_evt_t;

/* RX packet event */
typedef struct __attribute__((packed)) mac_rx_pkt_evt_s
{
    uint32_t rxm_fch;   /*!< Received Frame Control Fields Store Register  */
    uint32_t rx_aux_sec_hdr;    /*!< RX Security Aux Header Information Register */
    uint32_t rx_aux_frmcnt; /*!< RX Security Aux Header Frame Counter */
    uint32_t src_addr_match_sts;    /*!< RX source match filter status */
    union
    {
        struct
        {
            uint16_t panid;
            uint16_t addr;
        } short_addr;
        uint8_t long_addr[8];
    } matched_addr;
    uint64_t time_stamp; /*!< the time stamp of this RX packet */
    uint8_t frame_len;      /*!< frame length  */
    int8_t rssi;    /*!< the RSSI of this received packet, in dBm  */
    uint8_t lqi;
    uint8_t channel;    /*!< current channel  */
    uint8_t rx_buf_idx;
} mac_rx_pkt_evt_t, *pmac_rx_pkt_evt_t;

/* ED scan done event */
typedef struct __attribute__((packed)) mac_ed_scan_done_evt_s
{
    int8_t status;  /*!< ED scan status */
    int8_t ed_peak_lev; /*!< peak value of ED scan */
    int8_t ed_avrg_lev; /*!< average value of ED scan */
} mac_ed_scan_done_evt_t, *pmac_ed_scan_done_evt_t;

/// @brief IPC function's type definition
/// ZB IPC buffer address descripter
typedef struct zb_ipc_buf_base_desc_s
{
    uint32_t chk_sum1;
    volatile uint8_t *ipc_event_buf;
    volatile uint8_t *ipc_cmd_comp_event_buf;
    volatile uint8_t *ipc_cmd_buf;
    volatile uint8_t *ipc_rx_pkt_buf;
    uint32_t chk_sum2;
    uint32_t reserved1;
} zb_ipc_buf_base_desc_t, *pzb_ipc_buf_base_desc_t;

/// @brief  IPC device side local event type
typedef enum ipc_dev_local_event_type_e
{
    IPC_CMD_RDY     = 0x01,
    ZB_MAC_IRQ      = 0x02,
} ipc_dev_local_event_type_t, *pipc_dev_local_event_type_t;

typedef struct event_mac_irq_s
{
    uint16_t isr;   // interrupt status
} event_mac_irq_t, *pevent_mac_irq_t;

typedef struct event_ipc_cmd_rdy_s
{
//    uint8_t h2c_status;
} event_ipc_cmd_rdy_t, *pevent_ipc_cmd_rdy_t;

typedef struct ipc_dev_local_event_s
{
    ipc_dev_local_event_type_t event_type;
    union
    {
        event_mac_irq_t mac_irq;
        event_ipc_cmd_rdy_t ipc_cmd_rdy;
    };
} ipc_dev_local_event_t, *pipc_dev_local_event_t;

typedef int8_t (*zb_ipc_cmd_handle_t)(uint8_t *pcmdbuf);
typedef void (*zb_ipc_evt_handle_t)(uint8_t *pevtbuf);
typedef void (*ipc_mac_timeout_callback_t)(uint8_t tid);
typedef void (*ipc_mac_txif_callback_t)(uint32_t txsr, uint32_t tx_term_sts,
                                        uint32_t tx_frm_len_err,
                                        uint32_t tx_given_time_status);
typedef void (*ipc_mac_rxif_callback_t)(pmac_rx_pkt_evt_t pmac_rx_pkt_evt, uint8_t *prxpkt);

typedef struct bt_timer_entry_s
{
    uint32_t hit_time;    /*!< hit timeout interrupt value  */
    uint32_t interval;   /*!< periodocal timer interval  */
} bt_timer_entry_t, *pbt_timer_entry_s;

typedef struct rx_pkt_sts_s
{
    uint32_t rxm_fch;   /*!< Received Frame Control Fields Store Register  */
    uint32_t rx_aux_sec_hdr;    /*!< RX Security Aux Header Information Register */
    uint32_t rx_aux_frmcnt; /*!< RX Security Aux Header Frame Counter */
    uint32_t src_addr_match_sts;    /*!< RX source match filter status */
    union
    {
        struct
        {
            uint16_t panid;
            uint16_t addr;
        } short_addr;
        uint8_t long_addr[8];
    } matched_addr;
} rx_pkt_sts_t, *prx_pkt_sts_t;

typedef struct zb_ipc_device_adapt_s
{
    void *pmac_driver;  // MAC driver adapter
    uint8_t *pipc_event_buf;
    uint8_t *pipc_cmd_comp_event_buf;
    uint8_t *rx_pkt_buf;
    void (*mac_init_cmd_callback)(void);
    void (*zb_ipc_h2c_irq_handler)(void);
    zb_ipc_cmd_handle_t porting_ext_cmd_handler;
    zb_ipc_cmd_handle_t *pcmd_handler;
    void *pm_adp;   // power manager adapter
    uint32_t mac_intsts; /*!< MAC interrupt status  */
    void *zb_ipc_sem;  /*!< semaphore to wakeup IPC device task  */
    void *zb_ipc_task_handle;   //!< ZB IPC device task handle
    bt_timer_entry_t bt_timer[MAC_BT_TIMER_NUM];   //!< struct to handle periodical BT timer
    mac_txif_evt_t txsts;   //!< status of packet TX
    rx_pkt_sts_t rxsts[RX_PKT_BUF_NUM];   //!< status of packet RX
    uint8_t cur_channel;    //!< current channel
    uint8_t max_cmd_type;   //!< Max IPC command type
    uint8_t next_cmd_seq_no;  //!< the expect seq_no of next command
    uint8_t cmd_cmp_seq_no;  //!< command complete event sequence number
    uint8_t ev_seq_no;  //!< event sequence number
} zb_ipc_device_adapt_t, *pzb_ipc_device_adapt_t;

typedef struct zb_ipc_host_adapt_s
{
    void *sem_evt_rx_ind;  /*!< semaphore to notify an IPC event is received */
    void *sem_cmd_comp_evt_ind;  /*!< semaphore to notify an IPC command complete event is received */
    void *sem_zb_ipc;  /*!< semaphore to wakeup IPC device task  */
    void *zb_ipc_task_handle;   //!< ZB IPC device task handle
    uint8_t is_event_pending;
    uint8_t next_ev_seq_no;  //!< the expect seq_no of next event
    ipc_mac_timeout_callback_t mac_timeout_callback;
    ipc_mac_txif_callback_t mac_txif_callback;
    ipc_mac_rxif_callback_t mac_rxif_callback;
    mac_ed_scan_callback_t mac_ed_scan_callback;
} zb_ipc_host_adapt_t, *pzb_ipc_host_adapt_t;

#ifdef __cplusplus
}
#endif

#endif  /* end of "#ifndef _ZB_MAC_IPC_H_" */
