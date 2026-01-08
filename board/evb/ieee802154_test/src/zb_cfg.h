/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      zb_cfg.h
   * @brief     IEEE802.15.4 config file header
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
  **************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  **************************************************************************************
  */
#ifndef _ZB_CFG_H_
#define _ZB_CFG_H_

void zb_cfg_euid_get(uint8_t *p_mac_addr);
void parse_zigbee_cfg_from_occd(void);
bool cfg_update_zb_mac(uint8_t *p_mac_addr);

#endif /* _ZB_CFG_H_ */
