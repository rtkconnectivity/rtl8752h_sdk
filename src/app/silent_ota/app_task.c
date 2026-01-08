/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include "gap.h"
#include "gap_le.h"
#include "app_msg.h"
#include "os_msg.h"
#include "os_task.h"
#include "trace.h"
#include "app_task.h"
#include "silent_ota_application.h"
#include "otp_config.h"
#if (AON_WDG_ENABLE == 1)
#include "rtl876x_aon_wdg.h"
#endif

#define APP_TASK_PRIORITY               1   /* Task priorities. */
#define APP_TASK_STACK_SIZE             512 * 4

#define MAX_NUMBER_OF_GAP_MESSAGE       0x20
#define MAX_NUMBER_OF_IO_MESSAGE        0x20
#define MAX_NUMBER_OF_EVENT_MESSAGE     (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)

void *app_task_handle;
void *evt_queue_handle;
void *io_queue_handle;

void app_main_task(void *p_param);
void driver_init(void);
/**
 * \brief    send msg queue to app task.
 *
 * \param[in]   p_handle   The handle to the message queue being peeked.
 *
 * \return           The status of the message queue peek.
 * \retval true      Message queue was peeked successfully.
 * \retval false     Message queue was failed to peek.
 */
bool app_send_msg_to_apptask(T_IO_MSG *p_msg)
{
    uint8_t event = EVENT_IO_TO_APP;

    if (os_msg_send(io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_ERROR0("send_io_msg_to_app fail");
        return false;
    }
    if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        APP_PRINT_ERROR0("send_evt_msg_to_app fail");
        return false;
    }
    return true;
}

void app_task_init()
{
    os_task_create(&app_task_handle, "app", app_main_task, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}

/**
* @brief
*
*
* @param   pvParameters
* @return  void
*/
void app_main_task(void *p_param)
{
    uint8_t event;

    os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint8_t));

    gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

//    driver_init();

#if (ROM_WATCH_DOG_ENABLE == 1)
    extern void reset_watch_dog_timer_enable(void);
    reset_watch_dog_timer_enable();
#endif

#if (AON_WDG_ENABLE == 1)
    aon_wdg_init(1, AON_WDG_TIME_OUT_PERIOD_SECOND);
    aon_wdg_enable();
    AON_WDG_Restart();
#endif

    while (true)
    {
        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            //DBG_DIRECT("***os_msg_recv***");
            if (event == EVENT_IO_TO_APP)
            {
                T_IO_MSG io_msg;
                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
                {
                    app_handle_io_msg(io_msg);
                }
            }
            else
            {
                gap_handle_msg(event);
            }
        }
    }
}

