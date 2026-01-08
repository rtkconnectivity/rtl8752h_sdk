/**************************************************************************//**
 * @file     bitmap.c
 * @brief    Source file for bitmap function implementation
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


#include <string.h>
#include <osif.h>
#include <sys/cc.h>
#include "lib/bitmap.h"

bitmap_t *bitmap_alloc(uint16_t total_blocks)
{
    bitmap_t *bitmap = (bitmap_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(bitmap_t));
    if (!bitmap)
    {
        return NULL;
    }
    bitmap->total_blocks = total_blocks;
    bitmap->received_blocks = 0;
    // calculate bytes
    uint16_t bytes_needed = (total_blocks + 7) / 8;
    bitmap->bits = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, bytes_needed);
    if (bitmap->bits)
    {
        memset(bitmap->bits, 0, bytes_needed); // init
    }
    else
    {
        osif_mem_free(bitmap);
        return NULL;
    }
    return bitmap;
}

void bitmap_free(bitmap_t *bitmap)
{
    if (bitmap)
    {
        if (bitmap->bits)
        {
            osif_mem_free(bitmap->bits);
        }
        osif_mem_free(bitmap);
    }
}

int bitmap_get(const bitmap_t *bitmap, uint16_t id)
{
    if (!bitmap || id >= bitmap->total_blocks)
    {
        return -1;
    }
    uint16_t byte_index = id / 8;
    uint16_t bit_index = id % 8;

    return (bitmap->bits[byte_index] & (1 << bit_index)) ? 1 : 0;
}

void bitmap_set(bitmap_t *bitmap, uint16_t id)
{
    if (!bitmap || id >= bitmap->total_blocks)
    {
        return;
    }
    uint16_t byte_index = id / 8;
    uint16_t bit_index = id % 8;
    if ((bitmap->bits[byte_index] & (1 << bit_index)) == 0)
    {
        bitmap->bits[byte_index] |= (1 << bit_index);
        bitmap->received_blocks++;
    }
}
