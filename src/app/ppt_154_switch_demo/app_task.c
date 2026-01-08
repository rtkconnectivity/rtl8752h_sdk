/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      app_task.c
   * @brief     Routines to create App task and handle events & messages
   * @author    jane
   * @date      2017-06-02
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <trace.h>
#include <os_msg.h>
#include <os_task.h>
#include <os_sched.h>
#include <os_timer.h>
#include <app_task.h>
#include <app_msg.h>
#include <ppt_simple.h>

/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TASK_PRIORITY               1         //!< Task priorities
#define APP_TASK_STACK_SIZE             256 * 4   //!<  Task stack size
#define APP_TASK_IO_QUEUE_SIZE          16
#define APP_TASK_EVT_QUEUE_SIZE         16

#define TX_TIMER_PERIOD                 3000 // ms
/*============================================================================*
 *                              Variables
 *============================================================================*/
void *app_task_handle;   //!< APP Task handle
void *app_task_evt_queue_handle;
void *app_task_io_queue_handle;

void *tx_timer;
/*============================================================================*
 *                              Functions
 *============================================================================*/
extern void ppt_demo(void);
extern void ppt_deinit_demo(void);
extern void zb_demo(void);
extern void zb_task_init(void);
extern void zb_mac_disable(void);

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
 * \brief    send msg queue to app task.
 *
 * \param[in]   p_msg   The message to copy into the queue.
 *
 * \return           The status of the message queue peek.
 * \retval true      Message queue was sent successfully.
 * \retval false     Message queue was failed to send.
 */
bool app_send_msg_to_apptask(T_IO_MSG *p_msg)
{
    uint8_t event = EVENT_IO_TO_APP;

    if (os_msg_send(app_task_io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail");
        return false;
    }
    if (os_msg_send(app_task_evt_queue_handle, &event, 0) == false)
    {
        APP_PRINT_ERROR0("send_evt_msg_to_app fail");
        return false;
    }
    return true;
}


/**
 * @brief       handle io msg posted to app task
 *
 * @param[in]   msg
 */
void app_handle_io_msg(T_IO_MSG msg)
{
    static bool flag = false;
    switch (msg.type)
    {
    case IO_MSG_TYPE_MULTIPROTOCOL:
        {
            if (msg.subtype == IO_MSG_MPM_PPT)
            {
                ppt_deinit_demo();
                APP_PRINT_INFO0("IO_MSG_TYPE_PPT");
            }
            else if (msg.subtype == IO_MSG_MPM_154)
            {
                zb_mac_disable();
                APP_PRINT_INFO0("IO_MSG_TYPE_154");
            }
            break;
        }
    case IO_MSG_TYPE_TIMER:
        {
            if (flag)
            {
                ppt_demo();
            }
            else
            {
                zb_demo();
            }
            flag = !flag;
            break;
        }
    default:
        break;
    }
}

/**
 * @brief trigger tx operation
 *
 * @param[in] p_handle
 */
static void tx_timer_callback(void *p_handle)
{
    APP_PRINT_INFO0("tx_timer_callback");
    T_IO_MSG msg = {0};
    msg.type = IO_MSG_TYPE_TIMER;
    if (app_send_msg_to_apptask(&msg) == false)
    {
        APP_PRINT_INFO0("send msg to app task failed");
    }
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
    os_msg_queue_create(&app_task_evt_queue_handle, APP_TASK_EVT_QUEUE_SIZE,
                        sizeof(uint8_t)); // T_EVENT_TYPE
    os_msg_queue_create(&app_task_io_queue_handle, APP_TASK_IO_QUEUE_SIZE, sizeof(T_IO_MSG));
    zb_task_init();
    os_timer_create(&tx_timer, "tx timer", 1, TX_TIMER_PERIOD, true, tx_timer_callback);
    os_timer_start(&tx_timer);
    tx_timer_callback(NULL);
    T_EVENT_TYPE event;
    while (1)
    {
        if (os_msg_recv(app_task_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG msg = {0};
                if (os_msg_recv(app_task_io_queue_handle, &msg, 0) == true)
                {
                    app_handle_io_msg(msg);
                }
            }
        }
    }
}

/** @} */ /* End of group PERIPH_APP_TASK */
