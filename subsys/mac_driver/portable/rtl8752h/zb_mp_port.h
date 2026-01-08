/**************************************************************************//**
 * @file      zb_mp_port.h
 * @brief     Header file for IEEE802.15.4 MP test porting macros.
 * @author    JustinWu
 * @version   V1.00
 * @date      2025-05-16
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


#ifndef _ZB_MP_PORT_H_
#define _ZB_MP_PORT_H_

#include "mac_port_macro.h"
#include "modem.h"

#ifndef ZB_PHY_ID
#define ZB_PHY_ID (0)
#endif

#define ZB_POUTER_OFF(phy_id)           new_pouter_off()
#define ZB_POUTER_ON(phy_id, arg1)      new_pouter_on(arg1)

#endif  /* end of _ZB_MP_PORT_H_ */
