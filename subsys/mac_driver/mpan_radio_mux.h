/**************************************************************************//**
 * @file      mpan_radio_mux.h
 * @brief     MAC functions header for mux for Multi-PAN.
 * @author    JustinWu
 * @version   V1.00
 * @date      2024-04-29
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

#ifndef _MPAN_RADIO_MUX_H_
#define _MPAN_RADIO_MUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mac_driver.h"

/* MAC control access lock to prevent conflict of MAC access between multi-PAN */
void mpan_mac_lock(uint8_t pan_idx);
uint8_t mpan_mac_owner(void);
void mpan_mac_unlock(void);

/* MAC packet TX lock to prevent conflict of packet TX between multi-PAN */
/* Make sure using TX lock in this sequence:
   lock TX -> Fill TX FIFO  -> lock MAC -> Trigger TX ->
   unlock MAC -> unlock TX (in TX done callback) */
void mpan_tx_lock(uint8_t pan_idx);
void mpan_tx_unlock(void);

void mpan_MACInitialize(uint8_t pan_idx);
void mpan_MACTimerCallBackReg(uint8_t pan_idx, mac_bt_timer_id_t tmr_id,
                              mac_irq_callback_t cbfun, uint32_t arg);


#ifdef __cplusplus
}
#endif

#endif    // end of #ifndef _MPAN_RADIO_MUX_H_
