/**************************************************************************//**
* @file     mac_test_config.h
* @brief    Config file for IEEE 802.15.4 MAC test
* @author   felix
* @version  V1.00
* @date     2025-01-17
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

#ifndef _MAC_TEST_CONFIG_H_
#define _MAC_TEST_CONFIG_H_

/*
 * User Configuration: active by unmark comments.
 * Please note that this is just a wish, the actual setting needs to go through
 * the following dependency check.
 */


//#define CFG_BT_ADV_EN 0       // default enable
//#define CFG_GPIO_DBG_EN 0     // default disable
//#define CFG_AUTO_TEST_EN 0    // default enable
//#define CFG_MPAN_EN 0         // default enable if IC support
//#define CFG_DUAL_CHNL_EN 0    // default enable if IC support
//#define CFG_MAX_PAN_NUM 0     // default 1
//#define CFG_ZB_PM_EN 0        // default enable
//#define CFG_BT_PM_EN 0        // default enable
//#define CFG_GET_IC_EUID 0     // default enable
//#define CFG_ADV_SWTIMER 0     // default disable (advanced mac sw timer test)
//#define CFG_PROTOTHREAD_EN 0  // default enable
//#define CFG_AUTO_INIT_EN 0    // default enable
//#define CFG_PHY_ARBI_PRIO_EN 0
//#define CFG_FPGA_DBG_PORT_EN 0
//#define CFG_ATCMD_ZIGBTEST 0   // default disable
//#define CFG_USB_CDC_ACM 1      // default disable

/* Backward Compatible */
#if defined(CONFIG_SOC_SERIES_RTL87X2H)
#define CFG_BOARD_RTL8752H
#endif

#if defined(CONFIG_SOC_SERIES_RTL87X2G)
#define CFG_BOARD_BEE4
#endif

#if defined(CONFIG_SOC_SERIES_RTL87x3G)
#define CFG_BOARD_BB2ULTRA
#endif

#if defined(CONFIG_SOC_SERIES_RTL87X3J)
#define CFG_BOARD_BB3
#endif

/* Dependence Check. Do not modify!! */
/* Rule for BT_ADV_EN */
#if defined(CFG_BOARD_8922D) || defined(CFG_BOARD_8730F) || defined(CFG_BOARD_BB3)
#define TEST_BT_ADV_EN 0 // RCP without upper stack
#else
#if defined(CFG_BT_ADV_EN)
#define TEST_BT_ADV_EN CFG_BT_ADV_EN
#else
#define TEST_BT_ADV_EN 1 // Default
#endif
#endif

/* Rule for MPAN_EN */
#if defined(CFG_BOARD_RTL8752H) || defined(CFG_BOARD_BEE4)
#define TEST_MPAN_EN 0 // NOT support multi-pan feature
#else
#if defined(CFG_MPAN_EN)
#define TEST_MPAN_EN CFG_MPAN_EN
#else
#define TEST_MPAN_EN 1
#endif
#endif

/* Rule for DUAL_CHNL_EN */
#if (TEST_MPAN_EN == 0)
#define TEST_DUAL_CHNL_EN 0
#else
#if defined(CFG_BOARD_BB2ULTRA)
#define TEST_DUAL_CHNL_EN 0 // bb2ultra not support dual channel feature
#else
#if defined(CFG_DUAL_CHNL_EN)
#define TEST_DUAL_CHNL_EN CFG_DUAL_CHNL_EN
#else
#define TEST_DUAL_CHNL_EN 1 // Default
#endif
#endif
#endif

/* Rule for MAX_PAN_NUM */
#if (TEST_MPAN_EN == 0)
#define TEST_MAX_PAN_NUM 1
#else
#if defined(CFG_MAX_PAN_NUM)
#define TEST_MAX_PAN_NUM CFG_MAX_PAN_NUM
#else
#define TEST_MAX_PAN_NUM 2 // Default
#endif
#endif

/* Rule for ZB_PM_EN */
#if defined(CFG_BOARD_8922D) || defined(CFG_BOARD_8730F)
#define TEST_ZB_PM_EN 0 // Default for NIC Combo
#else
#if defined(CFG_ZB_PM_EN)
#define TEST_ZB_PM_EN CFG_ZB_PM_EN
#else
#define TEST_ZB_PM_EN 1 // Default
#endif
#endif

/* Rule for BT_PM_EN */
#if (F_BT_DLPS_EN == 1 || DLPS_EN == 1 || F_DLPS_EN == 1)
#if defined(CFG_BT_PM_EN)
#define TEST_BT_PM_EN CFG_BT_PM_EN
#else
#define TEST_BT_PM_EN 1 // Default
#endif
#else
#define TEST_BT_PM_EN 0
#endif

/* Rule for ZB_IRQ_MANUAL_EN */
#if defined(CFG_BOARD_8922D)  || defined(CFG_BOARD_8730F)
#define TEST_ZB_IRQ_MANUAL_EN 0 // Default for 8922d, 8730f
#else
#define TEST_ZB_IRQ_MANUAL_EN 1 // Default
#endif

/* Rule for GET_IC_EUID */
#if defined(CFG_BOARD_8922D)  || defined(CFG_BOARD_8730F) || defined(CFG_BOARD_BB3)
#define TEST_GET_IC_EUID 0 // Default for 8922d
#else
#if defined(CFG_GET_IC_EUID)
#define TEST_GET_IC_EUID CFG_GET_IC_EUID
#else
#define TEST_GET_IC_EUID 1 // Default
#endif
#endif

/* Rule for ADV_SWTIMER */
#if defined(CFG_ADV_SWTIMER)
#define TEST_ADV_SWTIMER CFG_ADV_SWTIMER
#else
#define TEST_ADV_SWTIMER 0 // Default
#endif

/* Rule for GPIO_DBG_EN */
#if defined(CFG_GPIO_DBG_EN)
#define TEST_GPIO_DBG_EN CFG_GPIO_DBG_EN
#else
#define TEST_GPIO_DBG_EN 0 // Default
#endif

/* Rule for PROTOTHREAD_EN */
#if defined(CFG_PROTOTHREAD_EN)
#define TEST_PROTOTHREAD_EN CFG_PROTOTHREAD_EN
#else
#define TEST_PROTOTHREAD_EN 1 // Default
#endif

/* Rule for AUTO_TEST_EN */
#if (TEST_PROTOTHREAD_EN == 1)
#if defined(CFG_AUTO_TEST_EN)
#define TEST_AUTO_TEST_EN CFG_AUTO_TEST_EN
#else
#define TEST_AUTO_TEST_EN 1 // Default
#endif
#else
#define TEST_AUTO_TEST_EN 0 // Auto Test feature is depended on protothread
#endif

/* Rule for AUTO_INIT_EN */
#if defined(CFG_AUTO_INIT_EN)
#define TEST_AUTO_INIT_EN CFG_AUTO_INIT_EN
#else
#define TEST_AUTO_INIT_EN 1 // Default
#endif

/* Rule for PHY_ARBI_PRIO_EN */
#if defined(CFG_BOARD_8730F)
#if defined(CFG_PHY_ARBI_PRIO_EN)
#define TEST_PHY_ARBI_PRIO_EN CFG_PHY_ARBI_PRIO_EN
#else
#define TEST_PHY_ARBI_PRIO_EN 1 // Default for 8730f
#endif
#else
#define TEST_PHY_ARBI_PRIO_EN 0 // Default
#endif

/* Rule for FPGA_DBG_PORT_EN */
#if defined(CFG_FPGA_DBG_PORT_EN)
#define TEST_FPGA_DBG_PORT_EN CFG_FPGA_DBG_PORT_EN
#else
#define TEST_FPGA_DBG_PORT_EN 0 // Default
#endif

#if defined(CFG_ATCMD_ZIGBTEST)
#define TEST_ATCMD_ZIGBTEST CFG_ATCMD_ZIGBTEST
#else
#define TEST_ATCMD_ZIGBTEST 0 // Default
#endif

/* Rule for USB_CDC_ACM */
#if defined(CFG_BOARD_BEE4)
#if defined(CFG_USB_CDC_ACM)
#define TEST_USB_CDC_ACM CFG_USB_CDC_ACM
#else
#define TEST_USB_CDC_ACM 0
#endif
#else
#define TEST_USB_CDC_ACM 0 // Default
#endif

#endif /* _MAC_TEST_CONFIG_H_ */
