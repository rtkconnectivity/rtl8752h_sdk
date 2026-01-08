
#include "guidef.h"
#include "gui_port.h"

#include "os_timer.h"
#include <os_msg.h>
#include <os_task.h>
#include "platform_utils.h"
#include "trace.h"
#include "stdarg.h"
#include "os_sched.h"
#include "gui_server.h"


void *port_thread_create(const char *name, void (*entry)(void *param), void *param,
                         uint32_t stack_size, uint8_t priority)
{
    void *handle = NULL;
    if (os_task_create(&handle, name, entry, 0, stack_size, 1))
    {
        return handle;
    }
    else
    {
        return NULL;
    }
}
bool port_thread_delete(void *handle)
{
    return os_task_delete(handle);
}


bool port_mq_create(void *handle, const char *name, uint32_t msg_size, uint32_t max_msgs)
{
    return os_msg_queue_create(handle, max_msgs, msg_size);
}

bool port_mq_send(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    if (os_msg_send(handle, buffer, timeout) == false)
    {
        APP_PRINT_INFO2("gui_mq_send fail io queue line = %d,0x%x", __LINE__, handle);

        return false;
    }
    return true;
}

bool port_mq_recv(void *handle, void *buffer, uint32_t size, uint32_t timeout)
{
    return os_msg_recv(handle, buffer, timeout);
}

bool port_thread_mdelay(uint32_t ms)
{
//    platform_delay_ms(ms);
    os_delay(ms);  //if open, when tab was slid, IDLE stack will overflow
    return true;
}

uint32_t port_thread_ms_get(void)
{
    return os_sys_time_get();
}


#include "stdlib.h"
void *port_malloc(uint32_t n)
{
    return malloc(n);
}


void port_free(void *rmem)
{
    free(rmem);
}

void *port_realloc(void *ptr, uint32_t n)
{
    return realloc(ptr, n);
}
#include "gui_api.h"


#include "trace.h"

static void port_log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    char buf[256];
    vsnprintf(buf, sizeof(buf), format, args);

    //APP_PRINT_INFO1("[GUI MODULE]%s", TRACE_STRING(buf));
    DBG_DIRECT("[GUI MODULE]%s", buf);

    va_end(args);
}


static struct gui_os_api os_api =
{
    .name = "rtk_osif",
    .thread_create = port_thread_create,
    .thread_delete = port_thread_delete,
    .thread_mdelay = port_thread_mdelay,
    .thread_ms_get = port_thread_ms_get,
    .mq_create = port_mq_create,
    .mq_send = port_mq_send,
    .mq_recv = port_mq_recv,

    .f_malloc = port_malloc,
    .f_free = port_free,
    .f_realloc = port_realloc,

    .log = port_log,
};

void *rtk_gui_timer;
//extern void drv_dlps_check_cbacks_register(const char *name, bool (*cbacks)(void));
void gui_port_os_init(void)
{
    gui_os_api_register(&os_api);
//    drv_dlps_check_cbacks_register("gui", gui_server_dlps_check);
}

