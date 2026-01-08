/**************************************************************************//**
* @file      main.c
* @brief    Source file for BLE peripheral project, mainly used for initialize modules
* @author   jane
* @version  V1.0
* @date     2017-06-12
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

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <trace.h>
#include "osif.h"
#include "platform_port.h"
#include "zb_main.h"
#include "stdio_port.h"
#include "shell.h"
#include "mac_test_common.h"
#include "protothread.h"


/** @defgroup  PERIPH_DEMO_MAIN Peripheral Main
    * @brief Main file to initialize hardware and BT stack and start task scheduling
    * @{
    */

/*============================================================================*
 *                              Constants
 *============================================================================*/

/*============================================================================*
 *                              Variables
 *============================================================================*/
void *zb_sem;
void *zb_task_handle;   //!< ZB MAC Task handle
extern void shell_cmd_init(void);
uint8_t rxbuf[RX_BUF_SIZE];
struct ringbuf rxbuf_ring;

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

#if 1
int stdio_getc_wrap(void *adapter, char *data)
{
    int c = ringbuf_get(&rxbuf_ring);
    if (c != -1)
    {
        //DBG_DIRECT("getc %c", c);
        *data = (uint8_t)c;
        return 1;
    }
    else
    {
        return 0;
    }
}

extern void shell_register_test_cmd(void);
__WEAK void shell_register_priv_cmd(void)
{
}
__WEAK void shell_register_user_cmd(void)
{
}

extern void stdio_putc_wrap(void *adapter, const char data);

void zb_test_task(void *p_param)
{
    uint32_t notify = 0;
    dbg_init();
    _stdio_port_init(ZB_DBG_UART, (stdio_putc_t)&stdio_putc_wrap, (stdio_getc_t)&stdio_getc_wrap);
    shell_cmd_init();
    protothread_init();
    shell_register_test_cmd();
    shell_register_priv_cmd();
    shell_register_user_cmd();
    dbg_printf("start\r\n");
    while (1)
    {
        //DBG_DIRECT("zb idle %u", notify);
        osif_task_notify_take(1, 0xffffffff, &notify);
        //DBG_DIRECT("zb run %u", notify);
        while (ringbuf_elements(&rxbuf_ring))
        {
            shell_task();
        }
    }
}

#endif

/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
extern void mac_Initialize_Patch(void);
extern void zb_pin_mux_init(void);
extern void zb_periheral_drv_init(void);
void zb_task_init(void)
{
    mac_Initialize_Patch();

    DBG_DIRECT("zb_task_init");
    zb_pin_mux_init();
    DBG_DIRECT("zb_pin_mux_init");
    zb_periheral_drv_init();
    DBG_DIRECT("zb_periheral_drv_init");
    zb_mac_interrupt_enable();
    DBG_DIRECT("zb_mac_interrupt_enable");
    zb_mac_drv_enable();
    DBG_DIRECT("zb_mac_drv_enable");
    bool zb_sem_create = false;
    zb_sem_create = osif_sem_create(&zb_sem, "zb_sem", 0, 1);
    if (zb_sem_create == true)
    {
        DBG_DIRECT("osif_sem_create ok");
    }
    else
    {
        DBG_DIRECT("osif_sem_create fail");
    }
    osif_task_create(&zb_task_handle, "zb_test", zb_test_task, NULL, ZB_TASK_STACK_SIZE,
                     ZB_TASK_PRIORITY);
    DBG_DIRECT("osif_task_create");
}
/** @} */ /* End of group PERIPH_DEMO_MAIN */
