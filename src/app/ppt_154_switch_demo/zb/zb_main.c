/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      main.c
   * @brief     Source file for BLE peripheral project, mainly used for initialize modules
   * @author    jane
   * @date      2017-06-12
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
#include "zb_main.h"
#include "shell.h"
#include "mac_test_common.h"
/** added for switch demo test */
#include "app_msg.h"
#include "mac_driver.h"
#include "strproc.h"
#include "mac_802154_frame_parser.h"
#include "dbg_printf.h"
#include "protothread.h"
#include "osif.h"

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
typedef void (*zbpm_callback_t)(void);
void *zb_task_handle;
extern bool app_send_msg_to_apptask(T_IO_MSG *p_msg);
extern int zbmac_power_manager_set(uint32_t next, uint32_t period);
uint8_t rxbuf[RX_BUF_SIZE];
struct ringbuf rxbuf_ring;

BOOL stub_zb_mac_data(int argc, char *argv[])
{
    uint16_t panid = mac_GetPANId();
    uint16_t saddr = mac_GetShortAddress();
    uint16_t daddr = 0;
    uint16_t data_len = 0;
    uint32_t delay_us = 0;
    uint32_t loop_cnt = 1;
    fc_t fc = {0};

    if (argc < 3)
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

    daddr = _strtoul((const char *)(argv[0]), (char **)NULL, 16);       // param 1: dest address
    data_len = _strtoul((const char *)(argv[1]), (char **)NULL, 10);    // param 2: data payload len
    loop_cnt = _strtoul((const char *)(argv[2]), (char **)NULL, 10);    // param 3: loop count

    if (argc > 3)
    {
        delay_us = _strtoul((const char *)(argv[3]), (char **)NULL, 10);    // param 4: TX at given time
    }

    if (!daddr || !data_len)
    {
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }

    fc.type = FRAME_TYPE_DATA;
    fc.sec_en = 0;
    fc.pending = 0;
    fc.ack_req = ((daddr == 0xffff) ? 0 : 1);
    fc.panid_compress = 1;
    fc.dst_addr_mode = ADDR_MODE_SHORT;
    fc.ver = FRAME_VER_2006;
    fc.src_addr_mode = ADDR_MODE_SHORT;
    g_tx_buf.len = generate_ieee_frame(FRAME_TYPE_DATA, g_tx_buf.buf, 125, fc, 0,
                                       panid, (uint8_t *)&saddr, panid, (uint8_t *)&daddr,
                                       NV_FIELD, NV_FIELD, NV_FIELD, NV_FIELD);
    GEN_SEQ_DATA_MV_PTR(g_tx_buf.buf, 0, data_len, g_tx_buf.len);
    txl_ctrl_info_t info = {0};
    info.prt_tx_mask = 0xff;
    if (delay_us)
    {
        info.tx_mode = TX_MODE_SCHEDULE;
        info.schedule_mode = TX_SCHEDULE_DEFER;
        info.schedule_time = delay_us;
    }
    loop_ctrl(loop_cnt, txl_check, txl_exec, NULL,
              LOOP_REPORT_NONE, 0, (uint32_t)&info);
    dbg_printf("Done\r\n");
    return TRUE;
}

void zb_mac_disable(void)
{
    mac_RadioOff();
    mac_Disable();
}

void zb_mac_enable(void)
{
    zb_mac_drv_init();
}

void zb_demo(void)
{
    zb_mac_enable();
    mac_SetPANId(0x5);
    mac_SetChannel(12);
    mac_SetShortAddress(0x1);

    char *cmds[] = {"1", "7", "7"};
    bool ret;
    uint8_t counter = 0;
    while (1)
    {
        ret = stub_zb_mac_data(3, cmds);
        uint32_t next = 500 * 1000;
        uint32_t period = 0;
        zbmac_power_manager_set(next, period);
        if (++counter > 2)
        {
            break;
        }
    }

    if (ret)
    {
        T_IO_MSG bee_msg = {0};
        bee_msg.type = IO_MSG_TYPE_MULTIPROTOCOL;
        bee_msg.subtype = IO_MSG_MPM_154;
        if (app_send_msg_to_apptask(&bee_msg) == false)
        {
            APP_PRINT_ERROR0("send message to app queue failed");
        }
    }
    else
    {
        APP_PRINT_ERROR1("[zb_demo] zb_mac_data send fail, ret: %d", ret);
    }
}


/**
 * @brief    Entry of APP code
 * @return   int (To avoid compile warning)
 */
extern void mac_Initialize_Patch(void);
void zb_task_init(void)
{
    mac_Initialize_Patch();

    DBG_DIRECT("zb_task_init");
    zb_mac_interrupt_enable();
    DBG_DIRECT("zb_mac_interrupt_enable");
    zb_mac_drv_enable();
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
    DBG_DIRECT("os_task_create");
}
/** @} */ /* End of group PERIPH_DEMO_MAIN */
