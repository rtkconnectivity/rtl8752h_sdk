/**
*********************************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
**********************************************************************************************************
* @file     main.c
* @brief    This file provides demo code to realize RTC comparator function.
            RTC is a 32-bit counter.
* @details
* @author   yuan
* @date     2018-05-25
* @version  v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "rtl876x_io_dlps.h"
#include "dlps.h"
#include "io_rtc.h"
#include "os_sched.h"
#include "app_section.h"

/*============================================================================*
 *                              Constants
 *============================================================================*/


/*============================================================================*
 *                              Variables
 *============================================================================*/

PMCheckResult dlps_allow = PM_CHECK_PASS;

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * @brief    Contains the initialization of pinmux settings and pad settings
 * @note     All the pinmux settings and pad settings shall be initiated in this function,
 *           but if legacy driver is used, the initialization of pinmux setting and pad setting
 *           should be peformed with the IO initializing.
 * @return   void
 */
void board_init(void)
{
}

/**
 * @brief    Contains the initialization of peripherals
 * @note     Both new architecture driver and legacy driver initialization method can be used
 * @return   void
 */
void driver_init(void)
{
    extern void driver_rtc_init(void);
    driver_rtc_init();
}

/**
 * @brief this function will be called before enter DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
RAM_DATAON_DATA_SECTION
void app_enter_dlps_config(void)
{
    DBG_DIRECT("DLPS ENTER");
}

/**
 * @brief this function will be called after exit DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
uint32_t allow_count = 0;

RAM_DATAON_DATA_SECTION
void app_exit_dlps_config(void)
{
    allow_count ++;
    DBG_DIRECT("DLPS EXIT, wake up reason 0x%x", platform_pm_get_wakeup_reason());
}

/**
 * @brief DLPS CallBack function
 * @param none
* @return true : allow enter dlps
 * @retval void
*/
RAM_DATAON_DATA_SECTION
PMCheckResult app_dlps_check_cb(void)
{
    return dlps_allow;
}


/******************************************************************
 * @brief  pwr_mgr_init() contains the setting about power mode.
 * @param  none
 * @return none
 * @retval void
 */
void pwr_mgr_init(void)
{
    dlps_check_cb_reg(app_dlps_check_cb);
    DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);
    DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);
    DLPS_IORegister();
    lps_mode_set(PLATFORM_DLPS_PFM);
}

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
int main(void)
{
    extern uint32_t random_seed_value;
    srand(random_seed_value);

    board_init();
    driver_init();
    pwr_mgr_init();
    os_sched_start();

    return 0;
}
/** @} */ /* End of group PERIPH_DEMO_MAIN */

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor Corporation *****END OF FILE****/

