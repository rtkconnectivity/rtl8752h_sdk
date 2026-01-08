/**
*****************************************************************************************
*     Copyright(c) 2024 - 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      auto_test.c
   * @brief     Source file for auto test function implementation
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2024 - 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#include "protothread.h"
#include "platform_port.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "mac_driver_interface.h"
#include "net/pt_mac.h"
#include "mac_test_common.h"
#include "auto_test.h"

#if TEST_AUTO_TEST_EN
static bool auto_test_en = 0;
void at_send_more(uint8_t len, void *pvalue)
{
    if (auto_test_en == 0 || pvalue == NULL || len == 0)
    {
        return;
    }

    CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], pvalue, len, g_tx_buf.len);
    g_tx_buf.buf[2] += len;
}

void at_send_first(uint8_t len, void *pvalue)
{
    if (auto_test_en == 0)
    {
        return;
    }

    if (pvalue == NULL)
    {
        len = 0;
    }

    unsigned char bytes[] = {   0x04, // HCI event
                                0x0e, // HCI event completed
                                len + 4, // data length
                                0x02, // retry count
                                0x00, 0xfc, // zigbee command
                                0x01, // success
                            };

    BUF_RESET(g_tx_buf);
    CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], bytes, sizeof(bytes), g_tx_buf.len);
    CPY_MV_PTR(&g_tx_buf.buf[g_tx_buf.len], pvalue, len, g_tx_buf.len);
}

void at_send_last(uint8_t len, void *pvalue)
{
    if (auto_test_en == 0)
    {
        return;
    }
    at_send_more(len, pvalue);

    uint8_t *pBuffer = g_tx_buf.buf;
    uint8_t totalLength = g_tx_buf.len;
    uint8_t count = totalLength / 16;
    uint8_t remainder = totalLength % 16;
    uint8_t i;

    for (i = 0; i < count; i++)
    {
        UART_SendData(ZB_DBG_UART, &pBuffer[16 * i], 16);
        while (UART_GetFlagState(ZB_DBG_UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    }

    if (remainder > 0)
    {
        UART_SendData(ZB_DBG_UART, &pBuffer[16 * i], remainder);
        while (UART_GetFlagState(ZB_DBG_UART, UART_FLAG_THR_TSR_EMPTY) != SET);
    }
}

void at_send(uint8_t len, void *pvalue)
{
    if (auto_test_en == 0)
    {
        return;
    }
    at_send_first(len, pvalue);
    at_send_last(0, NULL);
}

void at_send_enh_ack_timeout(void)
{
    if (auto_test_en == 0)
    {
        return;
    }
    unsigned char bytes[] = {   0x04, // HCI event
                                0xff, // notify event
                                0x02, // data length
                                AT_NOTIFY_EV_ACK_TO, // sub event code: ack timeout
                                0x01, // success
                            };
    UART_SendData(ZB_DBG_UART, bytes, sizeof(bytes));
}

static void default_input(uint8_t *rx_data, uint8_t pan_idx)
{
    mac_rxfifo_tail_t *info = MAC_RX_PKT_INFO(rx_data);
    int8_t rssi = MAC_RX_PKT_RSSI(rx_data);
    unsigned char bytes[] = {0x04, 0xff, 0x07, AT_NOTIFY_EV_RX, rssi, info->mac_time >> 24, info->mac_time >> 16, info->mac_time >> 8, info->mac_time, pan_idx};
    UART_SendData(ZB_DBG_UART, bytes, sizeof(bytes));
}

MAC_INPUT_HANDLER(default_handler0, 0, MAC_CMD_ANY, default_input);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
MAC_INPUT_HANDLER(default_handler1, 1, MAC_CMD_ANY, default_input);
#endif
#if (TEST_MAX_PAN_NUM > 2)
MAC_INPUT_HANDLER(default_handler2, 2, MAC_CMD_ANY, default_input);
#endif
#if (TEST_MAX_PAN_NUM > 3)
MAC_INPUT_HANDLER(default_handler3, 3, MAC_CMD_ANY, default_input);
#endif
#endif

void auto_test_enable(void)
{
    if (auto_test_en == 0)
    {
        NETSTACK_MAC.register_handler(&default_handler0, 1);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
        NETSTACK_MAC.register_handler(&default_handler1, 1);
#endif
#if (TEST_MAX_PAN_NUM > 2)
        NETSTACK_MAC.register_handler(&default_handler2, 1);
#endif
#if (TEST_MAX_PAN_NUM > 3)
        NETSTACK_MAC.register_handler(&default_handler3, 1);
#endif
#endif
        auto_test_en = 1;
    }
}

void auto_test_disable(void)
{
    if (auto_test_en == 1)
    {
        NETSTACK_MAC.register_handler(&default_handler0, 0);
#if TEST_MPAN_EN
#if (TEST_MAX_PAN_NUM > 1)
        NETSTACK_MAC.register_handler(&default_handler1, 0);
#endif
#if (TEST_MAX_PAN_NUM > 2)
        NETSTACK_MAC.register_handler(&default_handler2, 0);
#endif
#if (TEST_MAX_PAN_NUM > 3)
        NETSTACK_MAC.register_handler(&default_handler3, 0);
#endif
#endif
        auto_test_en = 0;
    }
}

bool auto_test_is_enable(void)
{
    return auto_test_en;
}

static int cmd_autotest(int argc, char *argv[])
{
    uint8_t enable;
    switch (argc)
    {
    case 1:
        enable = _strtoul((const char *)(argv[0]), (char **)NULL, 10);
        if (enable)
        {
            auto_test_enable();
            at_send(0, NULL);
            break;
        }
        else
        {
            auto_test_disable();
        }
    case 0:
        dbg_printf("autotest %u\r\n", auto_test_en);
        break;
    default:
        dbg_printf("Error: InvalidArgs\r\n");
        return FALSE;
    }
    return TRUE;
}

void shell_register_cmd_autotest(void)
{
    shell_register_pt((shell_program_t)cmd_autotest, "autotest",
                      BRIEF("enable/disable auto test")
                      SYNOPSIS("autotest [<enable/disable>]")
                      DESCRIPTION("enable/disable: 1/0"));
}

void auto_test_init(void)
{
    auto_test_en = 0;
    ptmac_dump_rx = 0;
    ptmac_dump_tx = 0;
    //auto_test_enable();
    shell_register_cmd_autotest();
}
#endif
