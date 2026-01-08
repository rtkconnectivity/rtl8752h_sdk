/**************************************************************************//**
 * @file      mac_drv_rom_patch.h
 * @brief     Define MAC driver ROM code patch macros.
 * @author    JustinWu
 * @version   V1.00
 * @date      2022-10-03
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


#ifndef _MAC_DRV_ROM_PATCH_H_
#define _MAC_DRV_ROM_PATCH_H_

#if !defined(MAC_ROM_PATCH)
#define MAC_ROM_PATCH 1 // if enable ROM code patch
#endif

extern pmac_driver_t pmac_driver;

#define MAC_ROM_PATCH_FUNC_EXIST(func) ((pmac_driver != NULL) && (pmac_driver->rcp != NULL) && (pmac_driver->rcp->func != NULL))
#define MAC_ROM_PATCH_FUNC(func) (pmac_driver->rcp->func)

#define mac_memcpy_patch_exist()                MAC_ROM_PATCH_FUNC_EXIST(memcpy)
#define mac_porting_init_patch_exist()          MAC_ROM_PATCH_FUNC_EXIST(porting_init)
#define mac_SoftwareTimer_IntrHandler_patch_exist()\
    MAC_ROM_PATCH_FUNC_EXIST(swtimer_handler)
#define mac_EDScanProcess_patch_exist()         MAC_ROM_PATCH_FUNC_EXIST(edscan_handler)
#define mac_EnterCritical_patch_exist()         MAC_ROM_PATCH_FUNC_EXIST(enter_critical)
#define mac_ExitCritical_patch_exist()          MAC_ROM_PATCH_FUNC_EXIST(exit_critical)
#define mac_Reset_patch_exist()                 MAC_ROM_PATCH_FUNC_EXIST(reset)
#define mac_GetCurrentBTUS_patch_exist()        MAC_ROM_PATCH_FUNC_EXIST(curr_btus)
#define mac_BTClkToUS_patch_exist()             MAC_ROM_PATCH_FUNC_EXIST(btclk2us)
#define mac_SetBTClkUSInt_patch_exist()         MAC_ROM_PATCH_FUNC_EXIST(set_btus_int)
#define mac_RstRF_patch_exist()                 MAC_ROM_PATCH_FUNC_EXIST(reset_rf)
#define mac_SetRadioState_patch_exist()         MAC_ROM_PATCH_FUNC_EXIST(set_radio_state)
#define mac_GetRadioState_patch_exist()         MAC_ROM_PATCH_FUNC_EXIST(get_radio_state)
#define mac_EnTxNCsmacaCca_patch_exist()        MAC_ROM_PATCH_FUNC_EXIST(enable_txn_csmacca)
#define mac_DisTxNCsmacaCca_patch_exist()       MAC_ROM_PATCH_FUNC_EXIST(disable_txn_csmacca)
#define mac_SoftwareTimer_Init_patch_exist()    MAC_ROM_PATCH_FUNC_EXIST(swtimer_init)
#define mac_SoftwareTimer_Free_patch_exist()    MAC_ROM_PATCH_FUNC_EXIST(swtimer_free)
#define mac_SoftwareTimer_Stop_patch_exist()    MAC_ROM_PATCH_FUNC_EXIST(swtimer_stop)
#define mac_Rx_patch_exist()                    MAC_ROM_PATCH_FUNC_EXIST(rx)
#define mac_GrantPHYReq_patch_exist()           MAC_ROM_PATCH_FUNC_EXIST(grant_phy_req)
#define mac_EnterDLPS_patch_exist()             MAC_ROM_PATCH_FUNC_EXIST(enter_dlps)
#define mac_ExitDLPS_patch_exist()              MAC_ROM_PATCH_FUNC_EXIST(exit_dlps)

#define mac_memcpy_patch                        MAC_ROM_PATCH_FUNC(memcpy)
#define mac_porting_init_patch                  MAC_ROM_PATCH_FUNC(porting_init)
#define mac_SoftwareTimer_IntrHandler_patch     MAC_ROM_PATCH_FUNC(swtimer_handler)
#define mac_EDScanProcess_patch                 MAC_ROM_PATCH_FUNC(edscan_handler)
#define mac_EnterCritical_patch                 MAC_ROM_PATCH_FUNC(enter_critical)
#define mac_ExitCritical_patch                  MAC_ROM_PATCH_FUNC(exit_critical)
#define mac_Reset_patch                         MAC_ROM_PATCH_FUNC(reset)
#define mac_GetCurrentBTUS_patch                MAC_ROM_PATCH_FUNC(curr_btus)
#define mac_BTClkToUS_patch                     MAC_ROM_PATCH_FUNC(btclk2us)
#define mac_SetBTClkUSInt_patch                 MAC_ROM_PATCH_FUNC(set_btus_int)
#define mac_RstRF_patch                         MAC_ROM_PATCH_FUNC(reset_rf)
#define mac_SetRadioState_patch                 MAC_ROM_PATCH_FUNC(set_radio_state)
#define mac_GetRadioState_patch                 MAC_ROM_PATCH_FUNC(get_radio_state)
#define mac_EnTxNCsmacaCca_patch                MAC_ROM_PATCH_FUNC(enable_txn_csmacca)
#define mac_DisTxNCsmacaCca_patch               MAC_ROM_PATCH_FUNC(disable_txn_csmacca)
#define mac_SoftwareTimer_Init_patch            MAC_ROM_PATCH_FUNC(swtimer_init)
#define mac_SoftwareTimer_Free_patch            MAC_ROM_PATCH_FUNC(swtimer_free)
#define mac_SoftwareTimer_Stop_patch            MAC_ROM_PATCH_FUNC(swtimer_stop)
#define mac_Rx_patch                            MAC_ROM_PATCH_FUNC(rx)
#define mac_GrantPHYReq_patch                   MAC_ROM_PATCH_FUNC(grant_phy_req)
#define mac_EnterDLPS_patch                     MAC_ROM_PATCH_FUNC(enter_dlps)
#define mac_ExitDLPS_patch                      MAC_ROM_PATCH_FUNC(exit_dlps)

#endif  /* _MAC_DRV_ROM_PATCH_H_ */
