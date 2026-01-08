/*
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include "app_task.h"
#include "board.h"
#include "trace.h"
#include "os_sync.h"

#define configMINIMAL_SECURE_STACK_SIZE  1024

#define APP_TASK_PRIORITY               1   /* Task priorities. */
#define APP_TASK_STACK_SIZE             (512 * 8)

#define MAX_NUMBER_OF_GAP_MESSAGE       0x20
#define MAX_NUMBER_OF_IO_MESSAGE        0x20
#define MAX_NUMBER_OF_EVENT_MESSAGE     (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)

#if (USE_OSIF == 1)
#include "os_task.h"
#include "os_timer.h"
#include "os_mem.h"
#include "os_msg.h"

#define TEST_FTL_IO_MSG_VAL      0xAA
#define TEST_OS_IO_MSG_VAL       0xAB

void *app_task_handle;
void *evt_queue_handle;
void *io_queue_handle;
void *xTestTimerHandle = NULL;
uint32_t timers = 0;

/**
 * \brief    send msg queue to app task.
 *
 * \param[in]   p_handle   The handle to the message queue being peeked.
 *
 * \return           The status of the message queue peek.
 * \retval true      Message queue was peeked successfully.
 * \retval false     Message queue was failed to peek.
 */
bool app_send_msg_to_apptask(uint32_t *p_msg)
{
    uint32_t event = EVENT_IO_TO_APP;

    if (os_msg_send(io_queue_handle, p_msg, 0) == false)
    {
        APP_PRINT_INFO0("send_io_msg_to_app fail");
        return false;
    }
    if (os_msg_send(evt_queue_handle, &event, 0) == false)
    {
        APP_PRINT_INFO0("send_evt_msg_to_app fail");
        return false;
    }
    return true;
}

void test_timer_cb(void *xTimer)
{
    timers ++;
    APP_PRINT_INFO1("Test Timer timeout %d times", timers);

    if (timers <= 1)
    {
        uint32_t send_val = TEST_FTL_IO_MSG_VAL;   //test FTL
        app_send_msg_to_apptask(&send_val);
    }
    else if (timers <= 11)
    {
        uint32_t send_val = TEST_OS_IO_MSG_VAL;    //test OS
        app_send_msg_to_apptask(&send_val);
    }
    else
    {
        os_timer_stop(&xTestTimerHandle);
    }

}

void sw_timer_init(void)
{
    APP_PRINT_INFO0("sw_timer_init");

    bool retval = false;
    retval = os_timer_create(&xTestTimerHandle, "Test Timer",  1, \
                             100, true, test_timer_cb);
    if (!retval)
    {
        APP_PRINT_INFO1("create xTimerPeriodWakeupDlps retval=%d", retval);
    }
    else
    {
        os_timer_start(&xTestTimerHandle);
        APP_PRINT_INFO0("Start auto reload Test Timer: Period 1s");
    }
}

void test_heap(void)
{
    for (RAM_TYPE ram_type = RAM_TYPE_DATA_ON; ram_type < RAM_TYPE_NUM; ram_type ++)
    {
        void *p = os_mem_alloc(ram_type, 100);
        DBG_DIRECT("malloc: type %d, addr 0x%x", ram_type, (uint32_t) p);
    }
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
    uint32_t event, io_msg;

    APP_PRINT_INFO0("app task");

    os_msg_queue_create(&io_queue_handle, MAX_NUMBER_OF_IO_MESSAGE,  sizeof(uint32_t));
    os_msg_queue_create(&evt_queue_handle, MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint32_t));

//    gap_start_bt_stack(evt_queue_handle, io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

    while (true)
    {
        // Task code goes here.
        if (os_msg_recv(evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {
            APP_PRINT_INFO1("app task: recv event type %u", event);
            if (event == EVENT_IO_TO_APP)
            {
                if (os_msg_recv(io_queue_handle, &io_msg, 0) == true)
                {
                    if (TEST_OS_IO_MSG_VAL == io_msg)
                    {
                        APP_PRINT_INFO0("app task: recv test os io msg success!");
                    }
                    else
                    {
                        APP_PRINT_INFO1("app task: recv io msg 0x%x", io_msg);
                    }

                }
            }
        }
    }
}

void task_init(void)
{
    os_task_create(&app_task_handle, "app", app_main_task, 0, APP_TASK_STACK_SIZE,
                   APP_TASK_PRIORITY);
}

#else
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

QueueHandle_t evt_queue_handle;
QueueHandle_t io_queue_handle;
TaskHandle_t app_task_handle;
TimerHandle_t xTestTimerHandle = NULL;
uint32_t timers = 0;

bool app_send_msg_to_apptask(uint32_t *p_send)
{
    uint32_t event = EVENT_IO_TO_APP;

    if (xQueueGenericSend(io_queue_handle, (void *)p_send, (TickType_t)0, queueSEND_TO_BACK) != pdPASS)
    {
        DBG_DIRECT("send io msg fail");
        return false;
    }

    if (xQueueGenericSend(evt_queue_handle, (void *)&event, (TickType_t)0, queueSEND_TO_BACK) != pdPASS)
    {
        DBG_DIRECT("send event msg fail");
        return false;
    }
    return true;
}

void test_timer_cb(TimerHandle_t xTimer)
{
    timers ++;
    DBG_DIRECT("Test Timer timeout %u times", timers);

    uint32_t send_val = 100;   //test value
    app_send_msg_to_apptask(&send_val);
}

void sw_timer_init(void)
{
    uint32_t timer_id;
    DBG_DIRECT("sw_timer_init");
    xTestTimerHandle = xTimerCreate("Test Timer", 100, pdTRUE, &timer_id, test_timer_cb);
    DBG_DIRECT("Test Timer handle 0x%x", (uint32_t)xTestTimerHandle);
    if (xTestTimerHandle)
    {
        xTimerStart(xTestTimerHandle, 0);
        DBG_DIRECT("Start auto reload Test Timer: Period 1s");
    }
}

void app_main_task(void *pvParameters)
{
    uint32_t event, uIoReceivedMsg;

    /* This task calls secure side functions. So allocate a secure context for
     * it. */
    portALLOCATE_SECURE_CONTEXT(configMINIMAL_SECURE_STACK_SIZE);

    DBG_DIRECT("app task");


    evt_queue_handle = xQueueCreate(MAX_NUMBER_OF_EVENT_MESSAGE, sizeof(uint32_t));
    io_queue_handle = xQueueCreate(MAX_NUMBER_OF_IO_MESSAGE, sizeof(uint32_t));

    for (;;)
    {
        // Task code goes here.
        if (pdPASS == xQueueReceive(evt_queue_handle, &event, 0xFFFFFFFF))
        {
            DBG_DIRECT("app task: recv event type %u", event);
            if (event == EVENT_IO_TO_APP)
            {
                if (pdPASS == xQueueReceive(io_queue_handle, &uIoReceivedMsg, 0))
                {
                    DBG_DIRECT("app task: recv io msg 0x%x", uIoReceivedMsg);
                }
            }
        }
    }
}

// Function that creates a task.
void task_init(void)
{
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;

    // Create the task, storing the handle.  Note that the passed parameter ucParameterToPass
    // must exist for the lifetime of the task, so in this case is declared static.  If it was just an
    // an automatic stack variable it might no longer exist, or at least have been corrupted, by the time
    // the new task attempts to access it.
    xTaskCreate(app_main_task, "app", APP_TASK_STACK_SIZE, &ucParameterToPass, APP_TASK_PRIORITY,
                &app_task_handle);

}
#endif
