/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    otp_config.h
  * @brief   Update Configuration in APP
  * @date    2017.6.6
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef OTP_CONFIG_H
#define OTP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rtl876x_wdg.h"
#include "rtl876x_uart.h"
/*============================================================================*
 *                        debug configuration
 *============================================================================*/
/** @brief just for debug */
#define SYSTEM_TRACE_ENABLE                        0


/*============================================================================*
 *                        flash configuration
 *============================================================================*/
/** @brief support for puran flash*/
#define FTL_APP_CALLBACK_ENABLE                    0
/** @brief modify ftl logic space size to adjust the RAM footprint of the ftl Mapping Table
    * PAGE_ELEMENT_DATA_NUM = ((FMC_PAGE_SIZE / 8) - 1).
    * MAX_LOGICAL_ADDR_SIZE = (((PAGE_ELEMENT_DATA_NUM * (g_page_num - 1)) - 1) << 2)
    * if sector size is 4KB, PAGE_ELEMENT_DATA_NUM equal 511.
    * g_page_num = ftl physical size / FMC_PAGE_SIZE. so if ftl size is 16KB, g_page_num is 4.
    * example: default ftl size in flash layout is 16KB, MAX_LOGICAL_ADDR_SIZE = 0x17F0 = 6128
    * FTL_REAL_LOGIC_ADDR_SIZE must be less or equal MAX_LOGICAL_ADDR_SIZE, otherwise will init ftl fail.
*/
#define FTL_REAL_LOGIC_ADDR_SIZE                   (3 * 1024)
/** @brief modify FTL BT storage space size, unit KB. Default 1 means 1KB. Logic addr range: [0, FTL_BT_STORAGE_SPACE_SIZE * 1024 -1].
    * It can not be greater than FTL_REAL_LOGIC_ADDR_SIZE.
*/
#define FTL_BT_STORAGE_SPACE_SIZE                  1

/** @brief enable BP, set lock level depend on flash layout and selected flash id */
#define FLASH_BLOCK_PROTECT_ENABLE                 0
/** @brief modify delay time for wakeup flash from power down mode to standby mode*/
#define AFTER_TOGGLE_CS_DELAY                      6
/*============================================================================*
 *                        os configuration
 *============================================================================*/
#define OS_TICK_TIME                              OS_TICK_10MS


/*============================================================================*
 *                        platform configuration
 *============================================================================*/
/** @brief default enable swd pinmux */
#define SWD_PINMUX_ENABLE                          1

/** @brief default disable watch dog in rom */
#define ROM_WATCH_DOG_ENABLE                       0
/** @brief set wdg mode, default reset all */
#define ROM_WATCH_DOG_MODE                         RESET_ALL
/** @brief Watch Dog Timer Config, default 4s timeout
   * div_factor: 16Bit: 32.768k/(1+divfactor).
   * cnt_limit: 2^(cnt_limit+1) - 1 ; max 11~15 = 0xFFF.
   * wdg_mode:
   *            1: RESET_ALL_EXCEPT_AON
   *            3: RESET_ALL
**/
#define ROM_WATCH_DOG_CFG_DIV_FACTOR               31
#define ROM_WATCH_DOG_CFG_CNT_LIMIT                15

/** @brief enable aon wdg which continue work in dlps state */
#define AON_WDG_ENABLE                             0
/** @brief set aon wdg timeout period in seconds, max value is 65s */
#define AON_WDG_TIME_OUT_PERIOD_SECOND             10

/** @brief modify log baud rate, refer to UartBaudRate_TypeDef in rtl876x_uart.h */
#define LOG_BAUD_RATE                              UART_BAUD_RATE_2000000

/** @brief modify log ram type, refer to RAM_TYPE in mem_types.h */
#define LOG_RAM_TYPE                               RAM_TYPE_DATA_ON

/** @brief *default sw timer number is 32 */
#define TIMER_MAX_NUMBER                            32

/** @brief *default sw timer task stack size is 1KB */
#define TIMER_TASK_STACK_SIZE                       (256 * 4)

/**************************************************/
/** @brief config enable write hardfault record to flash example */
//config enable write hardfault record to flash
//if enable, must define 1; if disable, define 0 or not define
#define WRITE_HARDFAULT_RECORD_TO_FLASH_ENABLE        0
//if enable must define hardfault record begin and end flash addr, and record cfg depend on user flash usage
#define HARDFAULT_RECORD_BEG_ADDR                  0x0085c000  //change to user unused space
#define HARDFAULT_RECORD_END_ADDR                 (HARDFAULT_RECORD_BEG_ADDR + 2 * FLASH_NOR_SECTOR_SIZE)
#define HARDFAULT_RECORD_CFG                      ( BIT_ENABLE_SAVE_HARDFAULT )

/*before wdg system reset, write reset reason to specific flash addr if enable*/
#define WRITE_REASON_TO_FLASH_BEFORE_RESET_ENABLE     0
#if (WRITE_REASON_TO_FLASH_BEFORE_RESET_ENABLE > 0)
#undef ROM_WATCH_DOG_MODE
#define ROM_WATCH_DOG_MODE                            INTERRUPT_CPU
/*write reset reason to specific flash address*/
#define REBOOT_REASON_RECORD_ADDRESS                  0x0085c000
/** @brief The maximum number of reboot records is 2^(n-1). Each reset reason record requires 8 bytes, occupied flash size is 2^(n+2) bytes. */
#define REBOOT_REASON_RECORD_LIMIT_POWERT2            10  //reserve 4KB
#endif
/**************************************************/


/*============================================================================*
 *                        upperstack configuration
 *============================================================================*/
//add more here


/*============================================================================*
*                        OTA configuration
*============================================================================*/
/*If support unsafe single bank ota user data, must define the following macros */
#define SUPPORT_SINGLE_BANK_OTA_USER_DATA
#ifdef SUPPORT_SINGLE_BANK_OTA_USER_DATA
#define USER_DATA_START_ADDR                       (0x00880000)
#define USER_DATA_MAX_SIZE                         (400 * 1024)  //400K
#define DISABLE_AES_OTA                             0
#endif

/*normal ota timeout settings*/
#define OTA_TIMEOUT_TOTAL                          240
#define OTA_TIMEOUT_WAIT4_CONN                     60
#define OTA_TIMEOUT_WAIT4_IMAGE_TRANS              200
#define OTA_TIMEOUT_CTITTV                         0xFF


/*============================================================================*
 *                        app configuration
 *============================================================================*/
//add more here



#ifdef __cplusplus
}
#endif


/** @} */ /* End of group OTP_CONFIG */
#endif
