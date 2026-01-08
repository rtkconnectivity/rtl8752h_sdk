/**************************************************************************//**
 * @file      zb_mp_port.h
 * @brief     IEEE802.15.4 Mass production test functions porting macros
 *            and definition.
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

#include "stdint.h"
#include "zb_mp_port.h"

const PACKET_PARAMETER zb_packet_parameter =
{
    .rate_mode = 1,
    .num_data_header = 1,
    .en_fec = 0,
    .packet_header = 0,
    .rsvd = 0x100,  // rsvd[8] = zigbee modulation

    .payload_header = 7936,
    .payload_length = 127,
    .payload_std_format = 0,
    .payload_num_crc = 2,

    .payload_crc = 3967,
    .bt4 = 1,
    .le2m = 1,
    .lelr = 0,
    .lr_ci = 0,
    .rxif_sel = 2,
};
