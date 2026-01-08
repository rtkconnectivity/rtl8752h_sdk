/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include "trace.h"
#include "ppt_simple.h"
#include "ppt_cfg.h"
#include "os_sched.h"

volatile uint32_t rx_int_num;
volatile uint32_t rx_num;
volatile uint32_t tx_num;

static PPT_ISR_SECTION void ppt_isr_handler(void)
{
    PRO_MISR_TYPE reg_misr;
    reg_misr.d16 = RD_PPT_REG(PRO_MISR);
    APP_PRINT_INFO1("ppt_isr_handler: 0x%04x", reg_misr.d16);

    if (reg_misr.rx_int)
    {
        rx_int_num += 1;
        /* read concerned rx stack registers before pop rx stack */
        PRO_RSSI_TYPE rssi = {.d16 = RD_PPT_REG(PRO_RSSI)};
        PRO_LENGTH_TYPE rx_length = {.d16 = RD_PPT_REG(PRO_LENGTH)};
        PRO_RX_STACK_TYPE rx_stack = {.d16 = RD_PPT_REG(PRO_RX_STACK)};
        PRO_RX_HP_TYPE rx_hp = {.d16 = RD_PPT_REG(PRO_RX_HP)};
        PRO_ACCHIT_CLK_LOWER_TYPE acchit_clk_lower = {.d16 = RD_PPT_REG(PRO_ACCHIT_CLK_LOWER)};
        PRO_ACCHIT_CLK_UPPER_TYPE acchit_clk_upper = {.d16 = RD_PPT_REG(PRO_ACCHIT_CLK_UPPER)};
        PRO_RX_CRC_LOWER_TYPE rx_crc_lower = {.d16 = RD_PPT_REG(PRO_RX_CRC_LOWER)};
        PRO_RX_CRC_UPPER_TYPE rx_crc_upper = {.d16 = RD_PPT_REG(PRO_RX_CRC_UPPER)};
        PRO_RX_HS_LOWER_TYPE rx_hs_lower = {.d16 = RD_PPT_REG(PRO_RX_HS_LOWER)};
        /* pop rx stack */
        PRO_RX_HS_UPPER_TYPE rx_hs_upper = {.d16 = RD_PPT_REG(PRO_RX_HS_UPPER)};
        uint32_t rx_hs = rx_hs_lower.hs_15_0 + (rx_hs_upper.hs_30_16 << 16);
        if (rx_stack.rx_abort_rd)
        {
            APP_PRINT_ERROR0("prx: rx abort");
        }
        else if (rx_stack.rx_time_out || rx_stack.rx_hit == false)
        {
            APP_PRINT_ERROR0("prx: rx timeout");
        }
        else if (rx_stack.is_crc_error)
        {
            APP_PRINT_ERROR0("prx: rx crc error");
        }
        else
        {
            rx_num += 1;
            uint8_t rx_entry = PPT_RX_STACK_ENTRY(rx_stack);
            uint16_t rx_len = rx_length.d16 + PDU_HEADER_LEN;
            uint8_t *rx_buffer = ppt_pop_rx_data_by_entry(rx_entry, rx_len);
            APP_PRINT_INFO8("prx: rx count %d, int count %d, rx stack 0x%04x, entry %d, hp 0x%02x, len %d, hs 0x%08x, payload %b",
                            rx_num, rx_int_num, rx_stack.d16, rx_entry,
                            rx_hp.hp, rx_length.d16, rx_hs, TRACE_BINARY(rx_length.d16, rx_buffer + PDU_HEADER_LEN));
        }

        if (ppt_ctx->oneshot)
        {
            if (ppt_ctx->ack == false || rx_stack.rx_time_out)
            {
                ppt_ctx->fsm = PPT_FSM_STANDBY;
                ppt_ctx->sync_flag = false;
            }
        }
    }

    if (reg_misr.tx_int)
    {
        tx_num += 1;
        /* read concerned tx stack registers before pop tx stack */
        PRO_TX_STACK_TYPE tx_stack = {.d16 = RD_PPT_REG(PRO_TX_STACK)};
        PRO_TX_HS_LOWER_TYPE tx_hs_lower = {.d16 = RD_PPT_REG(PRO_TX_HS_LOWER)};
        PRO_TX_HS_UPPER_TYPE tx_hs_upper = {.d16 = RD_PPT_REG(PRO_TX_HS_UPPER)};
        uint32_t tx_hs = tx_hs_lower.tx_hs_15_0 + (tx_hs_upper.tx_hs_30_16 << 16);
        PRO_TX_CLK_LOWER_TYPE tx_clk_lower = {.d16 = RD_PPT_REG(PRO_TX_CLK_LOWER)};
        PRO_TX_CLK_UPPER_TYPE tx_clk_upper = {.d16 = RD_PPT_REG(PRO_TX_CLK_UPPER)};
        /* pop tx stack */
        PRO_TX_LENGTH_TYPE tx_length = {.d16 = RD_PPT_REG(PRO_TX_LENGTH)};
        uint8_t tx_entry = PPT_TX_STACK_ENTRY(tx_stack, tx_length);
        APP_PRINT_INFO8("prx: tx count %d, stack no tx %d empty %d tptr %d, entry %d, hp 0x%02x, len %d, hs 0x%08x",
                        tx_num, tx_stack.is_no_tx, tx_stack.is_empty, tx_stack.tx_ptr, tx_entry,
                        tx_stack.tx_hp, tx_length.tx_length, tx_hs);

        if (ppt_ctx->oneshot)
        {
            ppt_ctx->fsm = PPT_FSM_STANDBY;
            ppt_ctx->sync_flag = false;
        }
    }

    if (reg_misr.kill_prx_int)
    {
        WR_PPT_REG(PRO_MISR, BIT5);
        ppt_flush_rx_fifo();
        ppt_ctx->fsm = PPT_FSM_STANDBY;
        ppt_ctx->sync_flag = false;
    }
}

void ppt_demo(void)
{
    ppt_cfg();
    ppt_reg_handler(ppt_isr_handler);

    /*
     * psd sample code
     * get the channel rssi of different frequencies from 2402 to 2480 MHz
     */
    for (uint8_t loop = 0; loop < 79; loop++)
    {
        ppt_psd_mode_ext_t param =
        {
            {
                .chann_start = loop,
                .chann_stop = loop,
                .chann_step = 1,
                .mode = 0,
                .timeout = PSD_TIMEOUT_DEFAULT
            }
        };
        ppt_set_psd_mode_ext(&param);
        ppt_enable_psd(NULL);
    }
    for (uint8_t loop = 0; loop < 79; loop++)
    {
        int16_t rssi = ppt_get_psd_result(loop);
        DBG_DIRECT("PSD: freq %dMHz rssi = %ddBm", 2402 + loop, rssi);
    }

    /* prx setting */
    {
        ppt_prx_mode_ext_t param =
        {
            {
                .ack_mode = ACK_MODE,
                .continuous_mode = !ONESHOT_MODE //!< oneshot & continuous mode switch
            }
        };
        ppt_set_prx_mode_ext(&param);
    }

    {
        ppt_header_t param =
        {
            .hp = 3 //!< BLE PDU type: SCAN_REQ
        };
        ppt_set_tx_header(0, &param);
    }

    uint8_t data[] =
    {
        0x02, 0x02, 0x01, 0x02, 0x1, 0x1, //!< ScanA
        0x01, 0x02, 0x01, 0x02, 0x1, 0x1 //!< AdvA
    };
    ppt_push_tx_data(sizeof(data), data);

    if (ppt_ctx->oneshot)
    {
        while (1)
        {
            ppt_enable_prx(0);
            /* delay for print log */
            os_delay(10);
        }
    }
    else
    {
        /* use async mode to exit while loop and print log */
        ppt_enable_prx(ppt_async_cb_template);
        while (1)
        {
            os_delay(10);
        }
    }
}
