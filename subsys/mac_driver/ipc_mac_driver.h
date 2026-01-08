/**************************************************************************//**
 * @file      ipc_mac_driver.h
 * @brief     IEEE802.15.4 MAC/PHY driver over IPC header file.
 * @author    JustinWu
 * @version   V1.00
 * @date      2023-06-01
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
#ifndef _IPC_MAC_DRIVER_H_
#define _IPC_MAC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "zb_mac_ipc.h"

int8_t ipc_mac_Initialize(void);
int8_t ipc_mac_Reset(void);
int8_t ipc_mac_ReadReg(uint32_t addr, volatile uint32_t *val, zb_reg_size_t size);
int8_t ipc_mac_WriteReg(uint32_t addr, volatile uint32_t *val, zb_reg_size_t size);
int8_t ipc_mac_WriteRegBits(uint32_t addr, uint32_t mask, volatile uint32_t *val,
                            zb_reg_size_t size);
int8_t ipc_mac_GetCurrentMACTime(uint64_t *mac_time);
int8_t ipc_mac_GetCurrentBTUS(uint32_t *cur_time);
int8_t ipc_mac_GetCurrentBTClk(mac_bt_clk_t *cur_btclk);
int8_t ipc_mac_SetBTClkUSInt(uint8_t tid, uint32_t time_us);
int8_t ipc_mac_StartBTTimer(uint8_t tid, uint32_t interval_us, uint8_t periodocal);
int8_t ipc_mac_StopBTTimer(uint8_t tid);
int8_t ipc_mac_ResetRF(uint8_t tid);
int8_t ipc_mac_SetRadioState(uint8_t radio_state);
int8_t ipc_mac_SetPANID(uint16_t panid);
int8_t ipc_mac_SetShortAddress(uint16_t saddr);
int8_t ipc_mac_SetLongAddress(uint8_t *laddr);
int8_t ipc_mac_SetCoordShortAddress(uint16_t saddr);
int8_t ipc_mac_SetCoordLongAddress(uint8_t *laddr);
int8_t ipc_mac_SetGroupAddress(uint8_t *gaddr);
int8_t ipc_mac_SetChannel(uint8_t channel);
int8_t ipc_mac_GetChannel(uint8_t *channel);
int8_t ipc_mac_SetTXPower(int8_t tx_dbm);
int8_t ipc_mac_SetTxNCsmacaCca(uint8_t enable_csma);
int8_t ipc_mac_SetCcaMode(mac_cca_mode_t cca_mode);
int8_t ipc_mac_EDScan(uint32_t scan_duration, int8_t *ed_peak_lev, int8_t *ed_avrg_lev);
int8_t ipc_mac_EDScanSchedule(uint32_t scan_duration, mac_ed_scan_callback_t callback);
int8_t ipc_mac_EDScanScheduleCancel(void);
int8_t ipc_mac_TrigTxN(uint8_t ackreq, uint8_t secreq);
int8_t ipc_mac_TrigTxNAtTime(uint8_t ackreq, uint8_t secreq, uint8_t docca, uint32_t txtime);
int8_t ipc_mac_LoadTxNPayload(uint8_t HdrL, uint8_t FrmL, uint8_t *mpdu);
int8_t ipc_mac_TransmitTXN(uint8_t HdrL, uint8_t FrmL, uint8_t *mpdu, uint8_t ackreq,
                           uint8_t secreq);
int8_t ipc_mac_SetNonce(uint8_t *nonce);
int8_t ipc_mac_LoadKey(zb_key_buf_id_t key_fifo_id, uint8_t *key);
int8_t ipc_mac_SetSecurityLevel(zb_sec_id_t sec_id, uint8_t level);
int8_t ipc_mac_AddSrcShortAddrMatch(uint16_t short_addr, uint16_t panid);
int8_t ipc_mac_AddSrcExtAddrMatch(uint8_t *pext_addr);
int8_t ipc_mac_DelSrcShortAddrMatch(uint16_t short_addr, uint16_t panid);
int8_t ipc_mac_DelSrcExtAddrMatch(uint8_t *pext_addr);
int8_t ipc_mac_DelAllSrcShortAddrMatch(void);
int8_t ipc_mac_DelAllSrcExtAddrMatch(void);
int8_t ipc_mac_SetCCACSThreshold(uint8_t thres, uint8_t times, uint8_t cont_times);
int8_t ipc_mac_SetCCAEDThreshold(uint8_t thres, uint8_t res);
int8_t ipc_mac_ReadFIFO(uint8_t fifo_id, uint8_t len, uint8_t *pbuf);
int8_t ipc_mac_UpperCipher(uint8_t sec_mode, const uint8_t *const sec_key,
                           const uint8_t *const nonce);
int8_t ipc_mac_UpperDecipher(uint8_t sec_mode, const uint8_t *const sec_key,
                             const uint8_t *const nonce);
int8_t ipc_mac_PMInit(void);
int8_t ipc_mac_PMSetPowerMode(uint8_t power_mode, uint8_t wake_time_en,
                              uint8_t sleep_time_en, uint32_t *time_us);

void zb_mac_ipc_host_init(void);
void zb_mac_ipc_reg_timeout_callback(ipc_mac_timeout_callback_t pfunc);
void zb_mac_ipc_reg_txnif_callback(ipc_mac_txif_callback_t pfunc);
void zb_mac_ipc_reg_rxif_callback(ipc_mac_rxif_callback_t pfunc);

#ifdef __cplusplus
}
#endif

#endif    // end of #ifndef _IPC_MAC_DRIVER_H_
