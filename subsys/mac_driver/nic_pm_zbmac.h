/**************************************************************************//**
 * @file      nic_pm_zbmac.h
 * @brief     NIC Combo platform Power Manager for ZB MAC Functions header file.
 * @author    JustinWu
 * @version   V1.00
 * @date      2022-12-16
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

#ifndef NIC_PWR_MANAG_ZBMAC_H
#define NIC_PWR_MANAG_ZBMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

#define ZB_PM_SECTION_BSS   __attribute__ ((__section__(".zb_pm.bss")))
#define ZB_PM_SECTION_DATA  __attribute__ ((__section__(".zb_pm.data")))

typedef enum zbmac_power_mode_e
{
    ZBMAC_DEEP_SLEEP        = 0,   /**< Deep sleep */
    ZBMAC_ACTIVE            = 1,   /**< Active     */
} zbmac_power_mode_t, *pzbmac_power_mode_t;

/* power manager state(stage) transition cycle:
  Check -> Store & Enter -> Exit & Restore  */
typedef enum zbmac_pm_state_e
{
    ZBMAC_PM_CHECK          = 0,
    ZBMAC_PM_STORE_ENTER    = 1,
    ZBMAC_PM_EXIT_RESTOR    = 2,
    ZBMAC_PM_STAGE_MAX      = 3
} zbmac_pm_state_t;

typedef enum
{
    ZBMAC_PM_ERROR_UNKNOWN                  = 0x00,
    ZBMAC_PM_ERROR_POWER_MODE               = 0x01,
    ZBMAC_PM_ERROR_RADIO_BUSY               = 0x02,
    ZBMAC_PM_ERROR_RX_BUSY                  = 0x03,
    ZBMAC_PM_ERROR_SCANNING                 = 0x04,
    ZBMAC_PM_ERROR_INTERRUPT_PENDING        = 0x05,
    ZBMAC_PM_ERROR_WAKEUP_TIME              = 0x06,
    ZBMAC_PM_ERROR_32K_CHECK_LOCK           = 0x07,
    ZBMAC_PM_ERROR_HW_TIMER_RUNNING         = 0x08,
    ZBMAC_PM_ERROR_OCCUPIED_TIME            = 0x09,
    ZBMAC_PM_ERROR_NOT_INITED               = 0x0A,
} zbmac_power_mode_err_code_t;

typedef enum
{
    CANNOT_ENTER_LPS_MODE,
    CAN_ENTER_LPS_MODE_WITH_WAKEUP_INSTANT,
    CAN_ENTER_LPS_MODE_WITHOUT_WAKEUP_INSTANT
} LPS_CHECK_STATE;

typedef void (*zbpm_callback_t)(void);

typedef struct zbpm_adapter_s
{
    zbmac_power_mode_t power_mode;  /* Current power_mode,
                                       App set power_mode = ZBMAC_DEEP_SLEEP to enter DLPS */
    zbmac_power_mode_err_code_t error_code; // the reason cannot enter sleep mode
    uint32_t stage_time[ZBMAC_PM_STAGE_MAX];  /* the time, in unit of micro-sec, will take for
                                                 each stage */
    uint32_t minimum_sleep_time;    /* minimum time sleep time, in unit of micro-sec. the duration
                                       from current time to wakeup time cannot smaller than this value */
    uint32_t learning_guard_time;   /* guard time, in unit of micro-sec time, for learned stage time */
    struct
    {
        uint16_t wake_interval_en : 1;     // is wakeup periodical interval enabled
        uint16_t stage_time_learned : 1;   // is to learn (measure) PM stages consumed time
        uint16_t timer_clk_reten : 1;      // is reten the timer clock durring sleep mode
        uint16_t wakeup_time_disable : 1;  // is disable ZB MAC wake up by RTC timer
        uint16_t reserved : 12;
    } cfg;

    uint32_t wakeup_time_us;  // the time stamp of next wakeup time, in unit of us by BT clock
    uint32_t wakeup_interval_us;  // the sleep interval, in unit of us
    uint32_t scheduled_wakeup_time;
    uint32_t enter_time_us;  /* the time stamp when entering ZBMAC_PM_STORE_ENTER stage,
                                in unit of us by BT clock */
    void *pretain_buf;  // the MAC registers retainion buffer
    zbpm_callback_t enter_callback; // call back function when in enter stage
    zbpm_callback_t exit_callback;  // call back function when in exit stage
} zbpm_adapter_t, *pzbpm_adapter_t;

extern void (*zbmac_pm_init)(zbpm_adapter_t *padapter);
extern LPS_CHECK_STATE(*is_zb_can_enter_lps)(uint32_t *wakeup_instant);
extern uint32_t (*get_closest_zb_wakeup_instant)(void);
extern void (*lps_zb_backup_procedure)(void);
extern void (*lps_zb_wakeup_procedure)(void);

#ifdef __cplusplus
}
#endif

#endif /* NIC_PWR_MANAG_ZBMAC_H */
