/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      ppt_cfg.h
   * @brief     ppt demo common setting
   * @author    bill
   * @date      2021-11-16
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _PPT_CFG_H_
#define _PPT_CFG_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup PPT_CFG ppt configuration
  * @brief
  * @{
  */

#define FREQUENCY               2422 //!< unit of MHz
#define PHY_TYPE                PPT_PHY_TYPE_BLE_1M
#define PREAMBLE_LEN            1
#define ACCESS_ADDRESS_LEN      4
#define HEADER_PREFIX_LEN       1
#define HEADER_LENGTH_LEN       1
#define HEADER_SUFFIX_LEN       0
#define PDU_HEADER_LEN          PPT_RX_FIFO_HEADER_SIZE_BYTE(HEADER_PREFIX_LEN, HEADER_LENGTH_LEN, HEADER_SUFFIX_LEN)
#define CRC_LEN                 3
#define BIT_ORDER               PPT_FIELD_BIT_ORDER_LSB
#define ACCESS_ADDRESS          0x87, 0x65, 0x43, 0x21

#define ACK_MODE                1
#define ONESHOT_MODE            1
#define TURNAROUND_DELAY        0

/**
 * @brief  Configure the radio basic parameters
 * @return none
 */
void ppt_cfg(void);

/** End of PPT_CFG
* @}
*/

#ifdef __cplusplus
}
#endif

#endif

