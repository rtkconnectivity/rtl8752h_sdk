/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_sched.h>
#include <string.h>
#include <stdlib.h>
#include <trace.h>
#include <app_task.h>
#include "board.h"
#include "ppt_driver.h"

#if DLPS_EN
#include <dlps.h>
#include <rtl876x_io_dlps.h>
#endif

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

}

#if DLPS_EN
PMCheckResult io_dlps_check(void)
{
    DBG_DIRECT("CHECK");
    return PM_CHECK_PASS;
}

void io_dlps_enter_cb(void)
{
    DBG_DIRECT("ENTER");
}

void io_dlps_exit_cb(void)
{
    DBG_DIRECT("EXIT");
}
#endif

/**
 * @brief    Contains the power mode settings
 * @return   void
 */
void pwr_mgr_init(void)
{
#if DLPS_EN
    /* ppt dlps init */
    ppt_dlps_init();

    /* platform dlps init */
    if (false == dlps_check_cb_reg((DLPSEnterCheckFunc)io_dlps_check))
    {
        APP_PRINT_ERROR0("dlps_check_cb_reg(io_dlps_check) failed!!");
    }
    DLPS_IORegUserDlpsEnterCb(io_dlps_enter_cb);
    DLPS_IORegUserDlpsExitCb(io_dlps_exit_cb);
    DLPS_IORegister();
    lps_mode_set(PLATFORM_DLPS_PFM);
#endif
}

/**
 * @brief    Contains the initialization of all tasks
 * @note     There is only one task in BLE Peripheral APP, thus only one APP task is init here
 * @return   void
 */
void task_init(void)
{
    app_task_init();
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
    pwr_mgr_init();
    task_init();
    os_sched_start();

    return 0;
}
/** @} */ /* End of group PERIPH_DEMO_MAIN */


