/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include <stdio.h>
#include "trace.h"
#include "ppt_simple.h"
#include "ppt_cfg.h"
#include "os_sched.h"

volatile uint32_t tx_num;
volatile uint32_t rx_num;

static PPT_ISR_SECTION void ppt_isr_handler(void)
{
    PRO_MISR_TYPE reg_misr;
    reg_misr.d16 = RD_PPT_REG(PRO_MISR);
    APP_PRINT_INFO1("ppt_isr_handler: 0x%04x", reg_misr.d16);

    if (reg_misr.tx_early_int)
    {
        WR_PPT_REG(PRO_MISR, BIT2);
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
        APP_PRINT_INFO8("ptx: tx count %d, stack no tx %d empty %d tptr %d, entry %d, hp 0x%02x, len %d, hs 0x%08x",
                        tx_num, tx_stack.is_no_tx, tx_stack.is_empty, tx_stack.tx_ptr, tx_entry,
                        tx_stack.tx_hp, tx_length.tx_length, tx_hs);

        if (ppt_ctx->ack == false)
        {
            if (ppt_ctx->retransmit_counter == 0)
            {
                ppt_trigger_fw_ack(tx_stack.tx_entry_1_0);
                if (ppt_ctx->oneshot)
                {
                    ppt_ctx->fsm = PPT_FSM_STANDBY;
                    ppt_ctx->sync_flag = false;
                }
                else
                {
                    ppt_execute_instruction(PPT_HW_INSTRUCTION_PTX_DISABLE);
                }
            }
            else
            {
                ppt_ctx->retransmit_counter -= 1;
            }
        }
    }

    if (reg_misr.rx_int)
    {
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
            APP_PRINT_ERROR0("ptx: rx abort");
        }
        else if (rx_stack.rx_time_out || rx_stack.rx_hit == false)
        {
            APP_PRINT_ERROR0("ptx: rx timeout");
        }
        else if (rx_stack.is_crc_error)
        {
            APP_PRINT_ERROR0("ptx: rx crc error");
        }
        else
        {
            rx_num += 1;
            uint8_t rx_entry = PPT_RX_STACK_ENTRY(rx_stack);
            uint16_t rx_len = rx_length.d16 + PDU_HEADER_LEN;
            uint8_t *rx_buffer = ppt_pop_rx_data_by_entry(rx_entry, rx_len);
            APP_PRINT_INFO7("ptx: rx count %d, rx stack 0x%04x, entry %d, hp 0x%02x, len %d, hs 0x%08x, payload %b",
                            rx_num, rx_stack.d16, rx_entry, rx_hp.hp, rx_length.d16, rx_hs, TRACE_BINARY(rx_length.d16,
                                    rx_buffer + PDU_HEADER_LEN));
        }

        if (ppt_ctx->retransmit_counter == 0)
        {
            ppt_trigger_fw_ack(rx_stack.entry_0);
            if (ppt_ctx->oneshot)
            {
                ppt_ctx->fsm = PPT_FSM_STANDBY;
                ppt_ctx->sync_flag = false;
            }
            else
            {
                ppt_execute_instruction(PPT_HW_INSTRUCTION_PTX_DISABLE);
            }
        }
        else
        {
            ppt_ctx->retransmit_counter -= 1;
        }

        /* statistics of response miss rate */
        char rate[10];
        sprintf(rate, "%u.%03u%%", (tx_num - rx_num) * 100 / tx_num,
                (tx_num - rx_num) * 100000 / tx_num % 1000);
        APP_PRINT_INFO4("ptx: statistics tx %d, rx %d, miss %d(%s)", tx_num, rx_num, tx_num - rx_num,
                        TRACE_STRING(rate));
    }

    if (reg_misr.kill_ptx_int)
    {
        WR_PPT_REG(PRO_MISR, BIT4);
        ppt_flush_rx_fifo();
        ppt_ctx->fsm = PPT_FSM_STANDBY;
        ppt_ctx->sync_flag = false;
    }
}

void ppt_demo(void)
{
    ppt_cfg();
    ppt_reg_handler(ppt_isr_handler);

    {
        ppt_ptx_mode_ext_t param =
        {
            {
                .ack_mode = ACK_MODE,
                .periodic_mode = !ONESHOT_MODE, //!< oneshot & periodic mode switch
                .periodic_interval = 7
            },
            .retransmit_times = 1
        };
        ppt_set_ptx_mode_ext(&param);
    }

    {
        ppt_header_t param =
        {
            .hp = 0 //!< BLE PDU type: ADV_IND
        };
        ppt_set_tx_header(0, &param);
    }

    uint8_t data[] =
    {
        0x01, 0x02, 0x01, 0x02, 0x1, 0x1, //!< AdvA
        /* advertising data */
        0x2, 0x1, 0x5, //!< flags
        0x5, 0x9, '2', '.', '4', 'g' //!< complete local name
    };
    uint8_t len = sizeof(data);

    while (1)
    {
        ppt_push_tx_data(len, data);
        ppt_enable_ptx(0);

        /* modify data content and data length for test */
        data[sizeof(data) - 1] += 1;
        len += 1;
        if (len > sizeof(data))
        {
            len = 6; // reserved 6 bytes for the AdvA (Advertiser Address) field in the BLE Adv PDU
        }

        /* delay for periodic mode */
        if (ppt_ctx->oneshot == false)
        {
            os_delay(100);
        }
    }
}

