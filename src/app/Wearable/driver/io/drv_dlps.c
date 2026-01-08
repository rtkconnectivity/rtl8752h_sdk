/*
 * Copyright (c) 2022
 *
 *
 *
 *
 *
 *
 */

#include "drv_dlps.h"
#if defined RTL87x2G
#include "pm.h"
#else
#include "dlps.h"
#endif
#include "trace.h"
#include "app_section.h"
#include "stdlib.h"
#ifdef SENSOR_MODE
#include "pm.h"
#endif


bool dlps_flag = false;

static dlps_slist_t drv_dlps_exit_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_enter_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_check_slist =
{
    .next = NULL,
};
static dlps_slist_t drv_dlps_wakeup_slist =
{
    .next = NULL,
};


/**
 * @brief    System_Handler
 * @note     system handle to judge which pin is wake source
 * @return   void
 */
APP_RAM_TEXT_SECTION
void System_Handler(void)
{
#ifdef RTL8772F
    DBG_DIRECT("SYSTEM_HANDLER 0x%x", get_aon_wakeup_int());
#else
    DBG_DIRECT("DLPS WAKE UP , System_Handler");
#endif

    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_wakeup_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        p_item->dlps_cb();
    }
#ifdef RTL8772F
    clear_aon_wakeup_int();
#endif
    DBG_DIRECT("DLPS WAKE UP , System_Handler END");
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
APP_RAM_TEXT_SECTION
static void app_enter_dlps_config(void)
{
    DBG_DIRECT("DLPS ENTER");
    extern void Pad_ClearAllWakeupINT(void);
    Pad_ClearAllWakeupINT();
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_enter_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        // DBG_DIRECT("DLPS ENTER CB %s", p_item->name);
        p_item->dlps_cb();
    }

    dlps_flag = true;
    // DBG_DIRECT("DLPS ENTER END");
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
APP_RAM_TEXT_SECTION
static void app_exit_dlps_config(void)
{
#ifdef RTL87x2G
    DBG_DIRECT("DLPS EXIT, wake up reason 0x%x", power_get_wakeup_reason());
#else
    DBG_DIRECT("DLPS EXIT");
#endif
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_exit_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        // DBG_DIRECT("DLPS EXIT CB %s", p_item->name);
        p_item->dlps_cb();
    }

    dlps_flag = false;
    // DBG_DIRECT("DLPS EXIT END");
#ifdef SENSOR_MODE
    POWERScenarioMode mode = power_scenario_mode_get(POWER_SCENARIO_OPERATION_MODE);
    if (mode == SCENARIO_OPERATION_SENSOR_MODE)
    {
        disable_xip_access(true);
        DBG_DIRECT("disable_xip_access");
    }
#endif
}

/**
 * @brief DLPS CallBack function
 * @param none
* @return true : allow enter dlps
 * @retval void
*/
#if defined RTL8772F
APP_RAM_TEXT_SECTION
static PMCheckResult app_dlps_check_cb(void)
{
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_check_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        //DBG_DIRECT("%s check fail! Module[%s]", __func__, p_item->name);
        if (p_item->dlps_cb() == false)
        {
            return PM_CHECK_FAIL;
        }
    }

    return PM_CHECK_PASS;
}
#elif defined RTL87x2G
APP_RAM_TEXT_SECTION
static POWER_CheckResult app_dlps_check_cb(void)
{
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_check_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        if (p_item->dlps_cb() == false)
        {
            // DBG_DIRECT("%s check fail! Module[%s]", __func__, p_item->name);
            return POWER_CHECK_FAIL;
        }
    }

    return POWER_CHECK_PASS;
}
#elif defined RTL8762D
APP_RAM_TEXT_SECTION
static bool app_dlps_check_cb(void)
{
    dlps_slist_t *node;
    for (node = dlps_slist_first(&(drv_dlps_check_slist)); node; node = dlps_slist_next(node))
    {
        drv_dlps_cb_item_t *p_item = dlps_container_of(node, drv_dlps_cb_item_t, slist);
        //DBG_DIRECT("%s check fail! Module[%s]", __func__, p_item->name);
        if (p_item->dlps_cb() == false)
        {
            return false;
        }
    }

    return true;
}
#endif


/******************************************************************
 * @brief  pwr_mgr_init() contains the setting about power mode.
 * @param  none
 * @return none
 * @retval void
 */
void pwr_mgr_init(void)
{
#if defined RTL8772F || defined RTL87x2G
    if (false == power_check_cb_register(app_dlps_check_cb))
    {
        // APP_PRINT_ERROR0("Error: dlps_check_cb_reg(app_dlps_check_cb) failed!");
    }
#endif
#ifdef RTL8762D
    if (false == dlps_check_cb_reg(app_dlps_check_cb))
    {
        // APP_PRINT_ERROR0("Error: dlps_check_cb_reg(app_dlps_check_cb) failed!");
    }
#endif
    DLPS_IORegUserDlpsEnterCb(app_enter_dlps_config);
    DLPS_IORegUserDlpsExitCb(app_exit_dlps_config);
    DLPS_IORegister();
#ifdef RTL87x2G
// #if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
    bt_power_mode_set(BTPOWER_DEEP_SLEEP);
//#endif
    power_mode_set(POWER_DLPS_MODE);
#endif
#ifdef RTL8772F
    lps_mode_set(PLATFORM_DLPS);
#endif
#ifdef RTL8762D
    lps_mode_set(LPM_DLPS_MODE);
#endif
}

void drv_dlps_exit_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;


    dlps_slist_append(&drv_dlps_exit_slist, &(p_item->slist));
}

void drv_dlps_enter_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_enter_slist, &(p_item->slist));
}

void drv_dlps_check_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_check_slist, &(p_item->slist));
}

void drv_dlps_wakeup_cbacks_register(const char *name, bool (*cbacks)(void))
{
    drv_dlps_cb_item_t *p_item = malloc(sizeof(drv_dlps_cb_item_t));
    if (p_item == NULL)
    {
        return;
    }
    p_item->name = name;
    p_item->dlps_cb = cbacks;

    dlps_slist_append(&drv_dlps_wakeup_slist, &(p_item->slist));
}




/************** end of file ********************/
