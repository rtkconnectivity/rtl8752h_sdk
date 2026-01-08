/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <os_msg.h>
#include <os_task.h>
#include <os_sched.h>
#include <app_task.h>

/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TASK_PRIORITY             1         //!< Task priorities
#define APP_TASK_STACK_SIZE           256 * 4   //!<  Task stack size

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *app_task_handle;   //!< APP Task handle

/*============================================================================*
 *                              Functions
 *============================================================================*/
void app_main_task(void *p_param);

/**
 * @brief  Initialize App task
 * @return void
 */
void app_task_init()
{
    os_task_create(&app_task_handle, "app", app_main_task, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}

/**
 * @brief        App task to handle events & messages
 * @param[in]    p_param    Parameters sending to the task
 * @return       void
 */
void app_main_task(void *p_param)
{
    /* avoid confliction with ble psd procedure */
    os_delay(1000);

    while (1)
    {
        void ppt_demo(void);
        ppt_demo();
    }
}

/** @} */ /* End of group PERIPH_APP_TASK */


