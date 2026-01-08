/**************************************************************************//**
 * @file      pt_mac.h
 * @brief     Header file for protothread mac function implementation
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

#ifndef _PT_MAC_H_
#define _PT_MAC_H_

#define MAC_CMD_ANY     0
#define MAC_CMD_VENDOR  0x24
#define MAC_CMD_PING    0x25
#define MAC_CMD_CSL     0x26 // proprietary CSL application

#define PT_MAC_RX_BUF_MAX 8 // The size must be a power of two
typedef struct
{
    uint8_t raw[144];
    uint32_t pan_idx : 8;
    uint32_t rsvd : 24; // For 4-byte alignment
} rx_item_t;

extern struct ringbufindex pt_mac_rx_ring;
extern rx_item_t pt_mac_rx_buf[PT_MAC_RX_BUF_MAX];

typedef void (* pt_mac_callback_t)(void *ptr, int status, int transmissions, uint32_t trig_ts,
                                   uint32_t tx_ts);

/* Generic MAC input handler */
typedef struct mac_input_handler
{
    struct mac_input_handler *next;
    void (*handler)(uint8_t *rx_data, uint8_t pan_idx);
    uint8_t pan_idx; // pan index
    uint8_t type; // mac command type
} mac_input_handler_t;

#define MAC_INPUT_HANDLER(name, pan_idx, type, func) \
    static mac_input_handler_t name = {NULL, func, pan_idx, type}

struct mac_driver
{
    char *name;

    /** Initialize the MAC driver */
    void (* init)(void);

    /** Send a packet from the packetbuf  */
    int (* send)(pt_mac_callback_t sent_callback, void *ptr, void *ctrl_info);

    /** Callback for getting notified of packet sent */
    void (* sent)(void);

    /** Callback for getting notified of incoming packet. */
    void (* input)(void);

    /** Register receiving handler */
    void (* register_handler)(mac_input_handler_t *handler, bool add);

    /** Turn the MAC layer on. */
    int (* on)(void);

    /** Turn the MAC layer off. */
    int (* off)(void);

    /** Read out estimated max payload size based on payload in packetbuf */
    int (* max_payload)(void);
};

/* Generic MAC return values. */
enum
{
    /**< The MAC layer transmission was OK. */
    PT_MAC_TX_OK,

    /**< The MAC layer transmission could not be performed due to a
     collision. */
    PT_MAC_TX_COLLISION,

    /**< The MAC layer did not get an acknowledgement for the packet. */
    PT_MAC_TX_NOACK,

    /**< The MAC layer can not transmit the packet on time. */
    PT_MAC_TX_NOTONTIME,

    /**< The MAC layer deferred the transmission for a later time. */
    PT_MAC_TX_DEFERRED,

    /**< The MAC layer transmission could not be performed because of an
     error. The upper layer may try again later. */
    PT_MAC_TX_ERR,

    /**< The MAC layer transmission could not be performed because of a
     fatal error. The upper layer does not need to try again, as the
     error will be fatal then as well. */
    PT_MAC_TX_ERR_FATAL,

    /**< The MAC layer transmission could not be performed because of
     insufficient queue space, failure to allocate a neighbor,
     or insufficient packet memory space. The upper layer may try again later. */
    PT_MAC_TX_QUEUE_FULL,

    /**< Max. reason */
    PT_MAC_TX_MAX_REASON,
};

/* MAC state */
enum
{
    PT_MAC_OFF,
    PT_MAC_ON,
};

extern bool ptmac_dump_rx;
extern bool ptmac_dump_tx;
extern const struct mac_driver ieee802154_driver;
#define NETSTACK_MAC ieee802154_driver

#endif /* _PT_MAC_H_ */
