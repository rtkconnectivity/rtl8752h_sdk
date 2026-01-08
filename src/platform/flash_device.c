/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/* For compatibility reasons, support for the old style API has been implemented.
Not recommend for new application. Please use flash_nor_device.h and flash_nor_basic.h
for developing new application. */

#include "../../inc/platform/flash_device.h"
#include "trace.h"

#define FLASH_RDID_INFO_BASE        0x201394

typedef enum
{
    FLASH_NOR_IDX_SPIC0,            /* NOR Flash controlled by SPIC0 */
    FLASH_NOR_IDX_SPIC1,
    FLASH_NOR_IDX_MAX
} FLASH_NOR_IDX_TYPE;

typedef enum
{
    FLASH_NOR_RET_UNKNOWN,
    FLASH_NOR_RET_NOT_EXIST_BASIC_CMD,
    FLASH_NOR_RET_NOT_EXIST_ADV_CMD,
    FLASH_NOR_RET_NOT_EXIST_QUERY_INFO,
    FLASH_NOR_RET_CMD_NOT_SUPPORT,
    FLASH_NOR_RET_DEV_NOT_SUPPORT,
    FLASH_NOR_RET_VENDOR_NOT_SUPPORT,
    FLASH_NOR_RET_LOCK_FAILED,
    FLASH_NOR_RET_UNLOCK_FAILED,
    FLASH_NOR_RET_BIT_MODE_SET_FAILED,
    FLASH_NOR_RET_BIT_MODE_NOT_SUPPORT,
    FLASH_NOR_RET_ILLEGAL_OPERATION,
    FLASH_NOR_RET_PARAM_INVALID,
    FLASH_NOR_RET_WAIT_BUSY_FAILED,
    FLASH_NOR_RET_IDX_OUT_OF_RANGE,
    FLASH_NOR_RET_ADDR_OUT_OF_RANGE,
    FLASH_NOR_RET_ADDR_LARGER_THAN_FLASH_SIZE,
    FLASH_NOR_RET_CAL_IN_PHYSICAL_CYC_NOT_FOUND,
    FLASH_NOR_RET_CAL_RD_DUMMY_LENGTH_NOT_FOUND,
    FLASH_NOR_RET_CAL_FAILED,
    FLASH_NOR_RET_MALLOC_FAILED,
    FLASH_NOR_RET_HOOK_FUNC,
    FLASH_NOR_RET_SUSPEND_UNNECCESSARY,
    FLASH_NOR_RET_SET_4_BYTE_ADDRESS_MODE_FAILED,
    FLASH_NOR_RET_SUCCESS
} FLASH_NOR_RET_TYPE;

typedef enum
{
    FLASH_NOR_ERASE_SECTOR  = 1,
    FLASH_NOR_ERASE_BLOCK   = 2,
    FLASH_NOR_ERASE_CHIP    = 4,
} FLASH_NOR_ERASE_MODE;

typedef enum _FLASH_NOR_IOCTL_TYPE
{
    FLASH_NOR_GET_BASE                          = 0x0000,
    FLASH_NOR_GET_ADDR_BASE,
    FLASH_NOR_GET_RDID,
    FLASH_NOR_GET_SIZE,
    FLASH_NOR_GET_BP,
    FLASH_NOR_GET_BP_TOP_BOTTOM,
    FLASH_NOR_GET_WAIT_BUSY_CTR,
    FLASH_NOR_GET_BIT_MODE,

    FLASH_NOR_SET_BASE                          = 0x1000,
    FLASH_NOR_SET_BP,
    FLASH_NOR_SET_BP_TOP_BOTTOM,
    FLASH_NOR_SET_BP_UNLOCK_BY_ADDR,
    FLASH_NOR_SET_WAIT_BUSY_CTR,
    FLASH_NOR_SET_SPIC_BAUD,
    FLASH_NOR_SET_LOG_BITMAP,

    FLASH_NOR_EXEC_BASE                         = 0x2000,
    FLASH_NOR_EXEC_FLASH_INIT,
    FLASH_NOR_EXEC_DP,
    FLASH_NOR_EXEC_FLASH_SW_RESET,
    FLASH_NOR_EXEC_QUERY_INFO_LOADING,
    FLASH_NOR_EXEC_HIGH_SPEED_MODE,
    FLASH_NOR_EXEC_FLASH_CAL,
    FLASH_NOR_EXEC_FLASH_READ,
    FLASH_NOR_EXEC_FLASH_WRITE,
    FLASH_NOR_EXEC_FLASH_ERASE,

    FLASH_NOR_BASE_MASK                         = 0xF000,
} FLASH_NOR_IOCTL_TYPE;

/** ioctl results **/
typedef enum _IOCTL_RESULT
{
    RESULT_FAILURE = 0,
    RESULT_SUCCESS = 1
} IOCTL_RESULT;
/*** flash ioctl end. ***/


void flash_dump_flash_info(void)
{
    extern void flash_nor_dump_flash_info(void);
    flash_nor_dump_flash_info();
}

uint32_t flash_get_bank_addr(T_FLASH_LAYOUT_NAME name)
{
    /* The new enum name is FLASH_LAYOUT_NAME, and it is same to T_FLASH_LAYOUT_NAME. */
    extern uint32_t flash_nor_get_bank_addr(T_FLASH_LAYOUT_NAME name);
    return flash_nor_get_bank_addr(name);
}

uint32_t flash_get_bank_size(T_FLASH_LAYOUT_NAME name)
{
    extern uint32_t flash_nor_get_bank_size(T_FLASH_LAYOUT_NAME name);
    return flash_nor_get_bank_size(name);
}

uint32_t flash_ioctl(uint32_t cmd, uint32_t param_1, uint32_t param_2)
{
    uint32_t ret = RESULT_SUCCESS;

    extern FLASH_NOR_RET_TYPE flash_nor_ioctl(uint16_t cmd, uint16_t idx, uint32_t p1, uint32_t p2,
                                              uint32_t p3);

    switch (cmd)
    {
    case flash_ioctl_get_size_main:
        ret = flash_nor_ioctl(FLASH_NOR_GET_SIZE, FLASH_NOR_IDX_SPIC0, 0, 0, 0);
        break;
    default:
        APP_PRINT_ERROR0("Unsupported legacy Flash ioctl CMD.");
        ret = RESULT_FAILURE;
        break;
    }

    return ret;
}

T_FLASH_EXIST_LV flash_get_flash_exist(void)
{
    extern T_FLASH_EXIST_LV flash_nor_get_exist(FLASH_NOR_IDX_TYPE idx);
    return flash_nor_get_exist(FLASH_NOR_IDX_SPIC0);
}

bool flash_erase_locked(T_ERASE_TYPE type, uint32_t addr)
{
    FLASH_NOR_ERASE_MODE mode;

    if (type == FLASH_ERASE_CHIP)
    {
        mode = FLASH_NOR_ERASE_CHIP;
    }
    else if (type == FLASH_ERASE_BLOCK)
    {
        mode = FLASH_NOR_ERASE_BLOCK;
    }
    else
    {
        mode = FLASH_NOR_ERASE_SECTOR;
    }

    extern FLASH_NOR_RET_TYPE(*flash_nor_erase_locked)(uint32_t addr, FLASH_NOR_ERASE_MODE mode);
    return flash_nor_erase_locked(addr, mode) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_write_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_write_locked)(uint32_t addr, uint8_t *data, uint32_t byte_len);
    return flash_nor_write_locked(start_addr, data, data_len) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_auto_write_locked(uint32_t start_addr, uint32_t data)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_write_locked)(uint32_t addr, uint8_t *data, uint32_t byte_len);
    return flash_nor_write_locked(start_addr, (uint8_t *)&data,
                                  4) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_auto_write_buffer_locked(uint32_t start_addr, uint32_t *data, uint32_t len)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_write_locked)(uint32_t addr, uint8_t *data, uint32_t byte_len);
    return flash_nor_write_locked(start_addr, (uint8_t *)data,
                                  len) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_read_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_read_locked)(uint32_t addr, uint8_t *data, uint32_t byte_len);
    return flash_nor_read_locked(start_addr, data, data_len) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_auto_read_locked(uint32_t addr, uint32_t *data)
{
    extern bool flash_nor_auto_read_locked(uint32_t addr, uint32_t *data);
    return flash_nor_auto_read_locked(addr, data);
}

bool flash_split_read_locked(uint32_t start_addr, uint32_t data_len, uint8_t *data, uint32_t *cnt)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_read_locked)(uint32_t addr, uint8_t *data, uint32_t byte_len);
    return flash_nor_read_locked(start_addr, data, data_len) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_get_block_protect_locked(uint8_t *bp_lv)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_get_bp_lv_locked)(FLASH_NOR_IDX_TYPE idx, uint8_t *bp_lv);
    return flash_nor_get_bp_lv_locked(FLASH_NOR_IDX_SPIC0,
                                      bp_lv) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_set_block_protect_locked(uint8_t bp_lv)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_set_bp_lv_locked)(FLASH_NOR_IDX_TYPE idx, uint8_t bp_lv);
    return flash_nor_set_bp_lv_locked(FLASH_NOR_IDX_SPIC0,
                                      bp_lv) == FLASH_NOR_RET_SUCCESS ? true : false;
}

bool flash_sw_protect_unlock_by_addr_locked(uint32_t unlock_addr, uint8_t *old_bp_lv)
{
    extern FLASH_NOR_RET_TYPE(*flash_nor_unlock_bp_by_addr_locked)(uint32_t unlock_addr,
                                                                   uint8_t *old_bp_lv);
    return flash_nor_unlock_bp_by_addr_locked(unlock_addr,
                                              old_bp_lv) == FLASH_NOR_RET_SUCCESS ? true : false;
}

T_FLASH_RET flash_try_high_speed(T_FLASH_MODE bit_mode)
{
    T_FLASH_RET ret;
    FLASH_NOR_RET_TYPE int_ret;

    extern FLASH_NOR_RET_TYPE(*flash_nor_try_high_speed_mode)(FLASH_NOR_IDX_TYPE idx,
                                                              T_FLASH_MODE bit_mode);
    int_ret = flash_nor_try_high_speed_mode(FLASH_NOR_IDX_SPIC0, bit_mode);

    switch (int_ret)
    {
    case FLASH_NOR_RET_SUCCESS:
        ret = FLASH_SUCCESS;
        break;
    case FLASH_NOR_RET_CMD_NOT_SUPPORT:
    case FLASH_NOR_RET_DEV_NOT_SUPPORT:
        ret = FLASH_NOT_SUPPORT;
        break;
    case FLASH_NOR_RET_UNKNOWN:
        ret = FLASH_ERROR_NOT_DEF;
        break;
    case FLASH_NOR_RET_ILLEGAL_OPERATION:
        ret = FLASH_ERROR_INVALID_INPUT;
        break;
    case FLASH_NOR_RET_IDX_OUT_OF_RANGE:
    case FLASH_NOR_RET_ADDR_OUT_OF_RANGE:
    case FLASH_NOR_RET_ADDR_LARGER_THAN_FLASH_SIZE:
        ret = FLASH_ERROR_OUT_OF_RANGE;
        break;
    case FLASH_NOR_RET_WAIT_BUSY_FAILED:
        ret = FLASH_ERROR_TIMEOUT;
        break;
    case FLASH_NOR_RET_NOT_EXIST_BASIC_CMD:
        ret = FLASH_NOT_EXIST;
        break;
    case FLASH_NOR_RET_NOT_EXIST_ADV_CMD:
    case FLASH_NOR_RET_NOT_EXIST_QUERY_INFO:
    case FLASH_NOR_RET_LOCK_FAILED:
    case FLASH_NOR_RET_UNLOCK_FAILED:
    case FLASH_NOR_RET_BIT_MODE_SET_FAILED:
    case FLASH_NOR_RET_BIT_MODE_NOT_SUPPORT:
    case FLASH_NOR_RET_CAL_IN_PHYSICAL_CYC_NOT_FOUND:
    case FLASH_NOR_RET_CAL_RD_DUMMY_LENGTH_NOT_FOUND:
    case FLASH_NOR_RET_CAL_FAILED:
    case FLASH_NOR_RET_MALLOC_FAILED:
    case FLASH_NOR_RET_HOOK_FUNC:
    case FLASH_NOR_RET_SUSPEND_UNNECCESSARY:
    case FLASH_NOR_RET_SET_4_BYTE_ADDRESS_MODE_FAILED:
        ret = FLASH_CONFIG_FAILED;
        break;
    case FLASH_NOR_RET_VENDOR_NOT_SUPPORT:
    case FLASH_NOR_RET_PARAM_INVALID:
    default:
        ret = FLASH_NOT_SUPPORT;
        break;
    }

    return ret;
}

uint32_t flash_auto_read(uint32_t addr)
{
    extern uint32_t flash_nor_auto_read(uint32_t addr);
    return flash_nor_auto_read(addr);
}

uint32_t flash_get_id(void)
{
    uint8_t *rdid = (uint8_t *)FLASH_RDID_INFO_BASE;
    return (rdid[0] << 16) | (rdid[2] << 8) | rdid[1];
}
