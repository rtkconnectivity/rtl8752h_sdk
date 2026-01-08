/**************************************************************************//**
 * @file      mac_private_handler.c
 * @brief     IEEE802.15.4 MAC/PHY driver private handler functions.
 * @author    JustinWu
 * @version   V1.00
 * @date      2021-09-24
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

#ifndef _MAC_PRIV_HANDLER_H_
#define _MAC_PRIV_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

#define MAC_PRIVATE_EVENT_QUEUE_SIZE    16  // define the MAC private event queue size

/**
  \brief  MAC Private primitive / event defintion
*/
typedef enum
{
    MAC_PRIV_EVENT_COEX_TIMER           = 0x01,      // Coexistence maintance timer timeout
    MAC_PRIV_EVENT_RX_NAK               = 0x02,      // RX NAK
    MAC_PRIV_EVENT_TX_FAILED            = 0x03,      // TX failed
    MAC_PRIV_EVENT_COEX_TDMA_SLOT_END   = 0x04,      // Coexistence TDMA slot timeout
} mac_priv_event_type_t;

/**
  \brief  MAC Private handler status
*/
typedef enum
{
    MAC_PRIV_HANDLER_STS_SUCCESS    = 0x00,      // success
    MAC_PRIV_HANDLER_STS_Q_FULL     = 0x01,      // queue full
} mac_priv_handler_sts_t;

/**
  \brief  Defines parameters of RX NAK event
*/
typedef struct mac_priv_event_rxnak_para_s
{
    uint8_t phy_gnt_status; // PHY grant status
} mac_priv_event_rxnak_para_t;

/**
  \brief  Defines parameters of TX NO ACK event
*/
typedef struct mac_priv_event_txnak_para_s
{
    uint8_t txn_term_sts; // TXN term status
} mac_priv_event_txnak_para_t;

/**
  \brief  Defines type for MAC 8-bits register retention list
*/
typedef struct mac_priv_event_s
{
    uint8_t event_type; // event type
    union
    {
        mac_priv_event_rxnak_para_t rxnak;
        mac_priv_event_txnak_para_t txnak;
    } param;

    void *pnext;    // for the link list maintain, point to next event
} mac_priv_event_t, *pmac_priv_event_t;

mac_priv_event_t *mac_priv_event_alloc(void);
void mac_priv_event_free(mac_priv_event_t *pevent);
void mac_priv_event_submit(mac_priv_event_t *pevent);
mac_priv_event_t *mac_priv_event_retrive(void);
void mac_priv_handler_init(void);
void mac_priv_handler_task(void);

#ifdef __cplusplus
}
#endif

#endif    // end of #ifndef _MAC_PRIV_HANDLER_H_
