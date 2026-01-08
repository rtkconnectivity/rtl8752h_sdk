/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-05-23     howie      first version
 */


#ifndef __DRV_FLASH_H_
#define __DRV_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "flash_map.h"
#include "rtl_hal_peripheral.h"

int rt_hw_flash_init(void);

int realtek_flash_read(uint32_t addr, uint8_t *buf, uint32_t size);
int realtek_flash_write(uint32_t addr, const uint8_t *buf, uint32_t size);
int realtek_flash_erase(uint32_t addr, uint32_t size);

#define REALTK_FLASH_START_ADDRESS                   OTA_TMP_ADDR
#define REALTEK_FLASH_SIZE                           (OTA_TMP_SIZE)
#define FLASH_SECTOR_SIZE                            4096
#define REALTEK_FLASH_END_ADDRESS                    (OTA_TMP_ADDR + OTA_TMP_SIZE)

#ifdef __cplusplus
}
#endif

#endif /* __DRV_FLASH_H_ */

/************************** end of file ******************************/

