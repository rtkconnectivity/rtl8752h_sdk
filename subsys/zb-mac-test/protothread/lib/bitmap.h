/**************************************************************************//**
 * @file     bitmap.h
 * @brief    Header file for bitmap function implementation
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


#ifndef _BITMAP_H_
#define _BITMAP_H_

#include "protothread.h"

typedef struct
{
    uint8_t *bits;
    uint16_t total_blocks;
    uint16_t received_blocks;
} bitmap_t;

bitmap_t *bitmap_alloc(uint16_t total_blocks);
void bitmap_free(bitmap_t *bitmap);
int bitmap_get(const bitmap_t *bitmap, uint16_t id);
void bitmap_set(bitmap_t *bitmap, uint16_t id);
#endif /*_BITMAP_H_*/
