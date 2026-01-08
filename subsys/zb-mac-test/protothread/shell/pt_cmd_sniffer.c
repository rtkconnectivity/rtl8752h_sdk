/**
*****************************************************************************************
*     Copyright(c) 2024 - 2025, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      pt_cmd_sniffer.c
   * @brief     Wireshark sniffer implementation
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2024 - 2025 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#include "protothread.h"
#include "dbg_printf.h"
#include "shell.h"
#include "strproc.h"
#include "pt_cmd.h"
#include "mac_test_common.h"
#include "mac_driver_interface.h"
#include "services/usb_cdc_acm.h"

// ============================================================
//  Simple 802.15.4 Sniffer FW Output (ASCII + CRLF)
//  Mode 0 : NOFCS   (no FCS, + timestamp)
//  Mode 1 : FULL    (with FCS, + timestamp)
//  Mode 2 : TAP     (no FCS, + RSSI/LQI/CH + timestamp)
//  Mode 255 : STOP
// ============================================================

typedef enum
{
    SNIFFER_MODE_NOFCS = 0,
    SNIFFER_MODE_FULL  = 1,
    SNIFFER_MODE_TAP   = 2,
    SNIFFER_MODE_STOP  = 3,
} sniffer_mode_t;

static sniffer_mode_t g_sniffer_mode = SNIFFER_MODE_STOP;

// 共用輸出 buffer：127 bytes PSDU + FCS + ASCII + meta 足夠
static char snf_out[320];
static const char hex_lut[] = "0123456789ABCDEF";

// === small integer → ascii helper（無 printf） ===
static char *i32_to_str(int v, char *p)
{
    char *s = p;
    int n = v;

    if (n < 0)
    {
        n = -n;
    }
    do
    {
        *p++ = (char)('0' + (n % 10));
        n /= 10;
    }
    while (n);

    if (v < 0)
    {
        *p++ = '-';
    }

    for (char *a = s, *b = p - 1; a < b; ++a, --b)
    {
        char t = *a;
        *a = *b;
        *b = t;
    }
    return p;
}

static char *u32_to_str(uint32_t v, char *p)
{
    char *s = p;
    do
    {
        *p++ = (char)('0' + (v % 10));
        v /= 10;
    }
    while (v);

    for (char *a = s, *b = p - 1; a < b; ++a, --b)
    {
        char t = *a;
        *a = *b;
        *b = t;
    }
    return p;
}

static char *u64_to_str(uint64_t v, char *p)
{
    char *s = p;
    do
    {
        *p++ = (char)('0' + (int)(v % 10));
        v /= 10;
    }
    while (v);

    for (char *a = s, *b = p - 1; a < b; ++a, --b)
    {
        char t = *a;
        *a = *b;
        *b = t;
    }
    return p;
}

void sniffer_uart_send(const uint8_t *buf, uint16_t len)
{
    if (dbg_output_interface == DBG_OUT_IF_UART)
    {
        uint8_t count = len / 16;
        uint8_t remainder = len % 16;
        uint8_t i;

        for (i = 0; i < count; i++)
        {
            UART_SendData(ZB_DBG_UART, &buf[16 * i], 16);
            while (UART_GetFlagState(ZB_DBG_UART, UART_FLAG_THR_TSR_EMPTY) != SET);
        }

        if (remainder > 0)
        {
            UART_SendData(ZB_DBG_UART, &buf[16 * i], remainder);
            while (UART_GetFlagState(ZB_DBG_UART, UART_FLAG_THR_TSR_EMPTY) != SET);
        }
    }
    else if (dbg_output_interface == DBG_OUT_IF_USB)
    {
        usb_cdc_acm_send(buf, len);
    }
}

// === Mode 0: NOFCS (MHR+payload, no FCS) + timestamp ===
static void sniffer_mode_nofcs(uint8_t *rx_data)
{
    uint8_t  *buf = MAC_RX_PKT(rx_data);
    uint8_t   len = MAC_RX_PKT_LEN(rx_data);  // MUST be without FCS
    uint64_t  ts  = MAC_RX_PKT_TIMESTAMP(rx_data); // us from boot

    char *p = snf_out;

    // prefix
    // "NOFCS "
    *p++ = 'N'; *p++ = 'O'; *p++ = 'F'; *p++ = 'C'; *p++ = 'S'; *p++ = ' ';

    // hex payload (no FCS)
    for (uint8_t i = 0; i < len; ++i)
    {
        uint8_t b = buf[i];
        *p++ = hex_lut[b >> 4];
        *p++ = hex_lut[b & 0x0F];
    }

    // space + timestamp
    *p++ = ' ';
    p = u64_to_str(ts, p);

    // CRLF
    *p++ = '\r';
    *p++ = '\n';

    sniffer_uart_send((const uint8_t *)snf_out, (uint16_t)(p - snf_out));
}

// === Mode 1: FULL (MHR+payload+FCS) + timestamp ===
static void sniffer_mode_full(uint8_t *rx_data)
{
    uint8_t  *buf = MAC_RX_PKT(rx_data);
    uint8_t   len = MAC_RX_PKT_LEN(rx_data);  // without FCS in buffer
    uint16_t  fcs = MAC_RX_PKT_CRC(rx_data);  // 16-bit FCS
    uint64_t  ts  = MAC_RX_PKT_TIMESTAMP(rx_data);

    char *p = snf_out;

    // "FULL "
    *p++ = 'F'; *p++ = 'U'; *p++ = 'L'; *p++ = 'L'; *p++ = ' ';

    // hex payload
    for (uint8_t i = 0; i < len; ++i)
    {
        uint8_t b = buf[i];
        *p++ = hex_lut[b >> 4];
        *p++ = hex_lut[b & 0x0F];
    }

    // append FCS (LSB first)
    uint8_t fcs_lo = (uint8_t)(fcs & 0xFFU);
    uint8_t fcs_hi = (uint8_t)(fcs >> 8);
    *p++ = hex_lut[fcs_lo >> 4];
    *p++ = hex_lut[fcs_lo & 0x0F];
    *p++ = hex_lut[fcs_hi >> 4];
    *p++ = hex_lut[fcs_hi & 0x0F];

    // space + timestamp
    *p++ = ' ';
    p = u64_to_str(ts, p);

    *p++ = '\r';
    *p++ = '\n';

    sniffer_uart_send((const uint8_t *)snf_out, (uint16_t)(p - snf_out));
}

// === Mode 2: TAP (MHR+payload+FCS + RSSI/LQI/CH + timestamp) ===
static void sniffer_mode_tap(uint8_t *rx_data)
{
    uint8_t  *buf = MAC_RX_PKT(rx_data);
    uint8_t   len = MAC_RX_PKT_LEN(rx_data);  // without FCS
    int8_t    rssi = MAC_RX_PKT_RSSI(rx_data);
    uint8_t   lqi  = MAC_RX_PKT_LQI(rx_data);
    uint8_t   ch   = mac_channel_get();       // 11..26
    uint64_t  ts   = MAC_RX_PKT_TIMESTAMP(rx_data);

    char *p = snf_out;

    // "TAP "
    *p++ = 'T'; *p++ = 'A'; *p++ = 'P'; *p++ = ' ';

    // hex payload
    for (uint8_t i = 0; i < len; ++i)
    {
        uint8_t b = buf[i];
        *p++ = hex_lut[b >> 4];
        *p++ = hex_lut[b & 0x0F];
    }

    // NOTE:
    // Wireshark assumes IEEE802_15_4_TAP frames are NO-FCS.
    // Do NOT append FCS in TAP mode.

    // " " RSSI " " LQI " " CH " " TS
    *p++ = ' ';
    p = i32_to_str(rssi, p);

    *p++ = ' ';
    p = u32_to_str(lqi, p);

    *p++ = ' ';
    p = u32_to_str(ch, p);

    *p++ = ' ';
    p = u64_to_str(ts, p);

    *p++ = '\r';
    *p++ = '\n';

    sniffer_uart_send((const uint8_t *)snf_out, (uint16_t)(p - snf_out));
}

// === RX dispatcher：radio 收到一筆 frame 時呼叫 ===
static void sniffer_radio_rx(uint8_t *rx_data, uint8_t pan_idx)
{
    switch (g_sniffer_mode)
    {
    case SNIFFER_MODE_NOFCS:
        sniffer_mode_nofcs(rx_data);
        break;
    case SNIFFER_MODE_FULL:
        sniffer_mode_full(rx_data);
        break;
    case SNIFFER_MODE_TAP:
        sniffer_mode_tap(rx_data);
        break;
    case SNIFFER_MODE_STOP:
    default:
        break;
    }
}

MAC_INPUT_HANDLER(sniffer_handler, 0, MAC_CMD_ANY, sniffer_radio_rx);

static int cmd_sniffer(int argc, char *argv[])
{
    uint32_t mode, ch = 0;

    switch (argc)
    {
    case 2:
        ch = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
        if (ch < 11 || ch > 26)
        {
            return FALSE; // invalid channel, ignore
        }
    case 1:
        mode = _strtoul((const char *)(argv[0]), (char **)NULL, 10);

        if (mode > SNIFFER_MODE_STOP)
        {
            return TRUE;
        }

        if (mode != g_sniffer_mode)
        {
            if (mode == SNIFFER_MODE_STOP)
            {
                mac_promiscuous_set(0);
                NETSTACK_MAC.register_handler(&sniffer_handler, 0);
                process_start(&ping_reply_process, NULL);
            }
            else if (g_sniffer_mode == SNIFFER_MODE_STOP)
            {
                process_exit(&ping_reply_process);
                mac_promiscuous_set(1);
                NETSTACK_MAC.register_handler(&sniffer_handler, 1);
            }
        }

        if (ch)
        {
            mac_channel_set((uint8_t)ch);
        }

        g_sniffer_mode = (sniffer_mode_t)mode;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void shell_register_cmd_sniffer(void)
{
    shell_register_pt((shell_program_t)cmd_sniffer, "sniffer",
                      BRIEF("Support Wireshark 802.15.4 Sniffer")
                      SYNOPSIS("sniffer <mode> [<ch>]")
                      DESCRIPTION("mode: 0 ~ 3")
                      DESCRIPTION("   0: NOFCS")
                      DESCRIPTION("   1: FULL")
                      DESCRIPTION("   2: TAP")
                      DESCRIPTION("   3: STOP"));
}
