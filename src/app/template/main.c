/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/*============================================================================*
  *                                 Header Files
  *============================================================================*/
#include "stdlib.h"
#include "app_section.h"
#include "os_mem.h"
#include "os_sched.h"
#include "trace.h"
#include "rtl876x_wdg.h"
#include "system_rtl876x.h"
#include "platform_utils.h"
#if (FEATURE_OVERLAY == 1)
#include "overlay_mgr.h"
#endif
#include "ftl.h"
#include "app_task.h"
#include "dlps.h"
#include "rtl876x_io_dlps.h"
#include "rtl876x_pinmux.h"

/*============================================================================*
  *                                 external variable
  *============================================================================*/

PMCheckResult dlps_allow = PM_CHECK_PASS;

/*============================================================================*
  *                                 Functions
  *============================================================================*/

#if (FEATURE_OVERLAY == 1)
OVERLAY_A_SECTION
void overlay_a_func(void)
{
    static int a = 5;
    static int b;

    DBG_DIRECT("%s(addr: %x) runs!\n", __func__, overlay_a_func);
    DBG_DIRECT("a = %d, b = %d\n", a--, b--);
}

OVERLAY_B_SECTION
void overlay_b_func(void)
{
    static int a = 3;
    static int b;

    DBG_DIRECT("%s(addr: %x) runs!\n", __func__, overlay_b_func);
    DBG_DIRECT("a = %d, b = %d\n", a++, b++);
}
#endif


void my_pre_main()
{
    DBG_DIRECT("my_pre_main");
}

bool my_wdg_cb(T_WDG_MODE wdg_mode, T_SW_RESET_REASON reset_reason)
{
    DBG_DIRECT("my_wdg_cb");
    return false;
}

BOOL_WDG_CB user_wdg_cb = my_wdg_cb;
USER_CALL_BACK app_pre_main_cb = my_pre_main;

#if (DLPS_EN == 1)
/**
 * @brief this function will be called before enter DLPS
 *
 *  set PAD and wakeup pin config for enterring DLPS
 *
 * @param none
 * @return none
 * @retval void
*/
DATA_RAM_FUNCTION
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

DATA_RAM_FUNCTION
void app_exit_dlps_config(void)
{
    DBG_DIRECT("DLPS EXIT, wake up reason 0x%x", platform_pm_get_wakeup_reason());
}

/**
 * @brief DLPS CallBack function
 * @param none
* @return true : allow enter dlps
 * @retval void
*/
DATA_RAM_FUNCTION
PMCheckResult app_dlps_check_cb(void)
{
    return dlps_allow;
}
#endif

void pwr_mgr_init(void)
{
#if (DLPS_EN == 1)
    if (false == dlps_check_cb_reg(app_dlps_check_cb))
    {
        APP_PRINT_ERROR0("Error: dlps_check_cb_reg(app_dlps_check_cb) failed!");
    }
    DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);
    DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);
    DLPS_IORegister();
    lps_mode_set(PLATFORM_DLPS_PFM);
#endif
}

/**
 *@brief main entry for user application.
*/
int main(void)
{

    extern uint32_t random_seed_value;
    srand(random_seed_value);

    DBG_DIRECT("APP main: upperstack_entry=0x%x", (uint32_t) upperstack_entry);

    pwr_mgr_init();
    task_init();
    sw_timer_init();
    os_sched_start();

    return 0;
}
