/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.

*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
  * @file     ppt_simple.c
  * @brief    Source file for simple wrapper of 2.4G module common driver.
  * @details
  * @author   Bill
  * @date     2025-12-1
  * @version  v1.0
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <string.h>
#include "trace.h"
#include "ppt_simple.h"
#include "os_mem.h"

extern uint8_t (*modem_psd_get_entry_num)(void);
extern int8_t (*get_phy_rssi0_dbm)(uint8_t);
extern uint8_t(*modem_psd_get_rf_mode)(void);
uint8_t modem_psd_get_scan_mode(void);

ppt_ctx_t ppt_ctx_imp;
ppt_ctx_t *ppt_ctx = &ppt_ctx_imp;

void ppt_set_ptx_mode_ext(ppt_ptx_mode_ext_t *param)
{
    ppt_ptx_mode_t *base = &param->base;
    ppt_ctx->ack = base->ack_mode;
    ppt_ctx->oneshot = !base->periodic_mode;
    ppt_ctx->retransmit_times = ppt_ctx->oneshot ? 0 : param->retransmit_times;
    ppt_set_ptx_mode(base);
}

void ppt_set_prx_mode_ext(ppt_prx_mode_ext_t *param)
{
    ppt_prx_mode_t *base = &param->base;
    ppt_ctx->ack = base->ack_mode;
    ppt_ctx->oneshot = !base->continuous_mode;
    ppt_set_prx_mode(base);
}

bool ppt_set_psd_mode_ext(ppt_psd_mode_ext_t *param)
{
    ppt_psd_mode_t *base = &param->base;
    if (base->chann_stop >= PSD_CHANN_NUM || base->chann_step >= PSD_CHANN_NUM)
    {
        return false;
    }
    if (base->chann_start > base->chann_stop)
    {
        return false;
    }
    else if (base->chann_start < base->chann_stop)
    {
        if (base->chann_step == 0)
        {
            return false;
        }
        uint8_t chann_num = base->chann_stop - base->chann_start + 1;
        chann_num = (chann_num + base->chann_step - 1) / base->chann_step;
        if (chann_num > modem_psd_get_entry_num())
        {
            return false;
        }
    }
    ppt_ctx->psd_mode = *base;
    ppt_set_psd_mode(base);
    return true;
}

PPT_API_SECTION uint8_t *ppt_get_tx_buffer(uint16_t payload_len)
{
    if (payload_len > PPT_TX_BUFFER_SIZE)
    {
        APP_PRINT_ERROR1("ppt_get_tx_buffer: fail, payload size %d exceed limit", payload_len);
        return NULL;
    }

    return ppt_ctx->tx_buffer[0];
}

uint8_t *ppt_get_tx_buffer_by_entry(uint8_t entry, uint16_t payload_len)
{
    if (payload_len > PPT_TX_BUFFER_SIZE)
    {
        APP_PRINT_ERROR1("ppt_get_tx_buffer_entry: fail, payload size %d exceed limit", payload_len);
        return NULL;
    }

    if (entry >= PPT_TX_BUFFER_NUM)
    {
        APP_PRINT_ERROR1("ppt_get_tx_buffer_entry: fail, entry %d exceed limit", entry);
        return NULL;
    }

    return ppt_ctx->tx_buffer[entry];
}

PPT_API_SECTION uint8_t *ppt_get_rx_buffer(uint16_t pdu_len)
{
    if (pdu_len > PPT_RX_BUFFER_SIZE)
    {
        APP_PRINT_ERROR1("ppt_get_rx_buffer: fail, pdu size %d exceed limit", pdu_len);
        return NULL;
    }

    return ppt_ctx->rx_buffer;
}

PPT_API_SECTION void ppt_push_tx_data(uint16_t len, uint8_t *data)
{
    uint8_t *tx_buffer = ppt_get_tx_buffer(len);
    memcpy(tx_buffer, data, len);
    ppt_push_tx_fifo(0, tx_buffer, len);
}

void ppt_update_tx_data(uint16_t len, uint8_t *data)
{
    uint8_t *tx_buffer = ppt_get_tx_buffer(len);
    memcpy(tx_buffer, data, len);
    ppt_update_tx_fifo(0, tx_buffer, len);
}

PPT_API_SECTION void ppt_push_tx_data_by_entry(uint8_t entry, uint16_t len, uint8_t *data)
{
    uint8_t *tx_buffer = ppt_get_tx_buffer_by_entry(entry, len);
    memcpy(tx_buffer, data, len);
    ppt_push_tx_fifo(entry, tx_buffer, len);
}

PPT_API_SECTION uint8_t *ppt_pop_rx_data(uint16_t len)
{
    uint8_t *rx_buffer = ppt_get_rx_buffer(len);
    ppt_pop_rx_fifo(0, rx_buffer, len);
    return rx_buffer;
}

PPT_API_SECTION uint8_t *ppt_pop_rx_data_by_entry(uint8_t entry, uint16_t len)
{
    uint8_t *rx_buffer = ppt_get_rx_buffer(len);
    ppt_pop_rx_fifo(entry, rx_buffer, len);
    return rx_buffer;
}

void ppt_async_cb_template(void)
{
    /* make the async way easy to use */
}

void ppt_enable_ptx(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_STANDBY)
    {
        return;
    }

    ppt_ctx->fsm = PPT_FSM_PTX;
    ppt_ctx->retransmit_counter = ppt_ctx->retransmit_times;
    ppt_ctx->async_cb = async_cb;
    ppt_ctx->sync_flag = true;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PTX_ENABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
        ppt_ctx->fsm = PPT_FSM_STANDBY;
    }
}

void ppt_disable_ptx(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_PTX)
    {
        return;
    }

    ppt_ctx->async_cb = async_cb;
    ppt_ctx->sync_flag = true;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PTX_DISABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
        ppt_ctx->fsm = PPT_FSM_STANDBY;
    }
}

void ppt_enable_prx(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_STANDBY)
    {
        return;
    }

    ppt_ctx->fsm = PPT_FSM_PRX;
    ppt_ctx->async_cb = async_cb;
    ppt_ctx->sync_flag = true;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PRX_ENABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
        ppt_ctx->fsm = PPT_FSM_STANDBY;
    }
}

void ppt_disable_prx(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_PRX)
    {
        return;
    }

    ppt_ctx->async_cb = async_cb;
    ppt_ctx->sync_flag = true;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PRX_DISABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
        ppt_ctx->fsm = PPT_FSM_STANDBY;
    }
}

PPT_API_SECTION void ppt_reset_hw_fsm(void)
{
    WR_PPT_REG_FIELD(PRO_RESET_CTRL_REG, fw_rst_trigger, 1);
    ppt_clear_radio();
    PRO_MISR_TYPE reg_misr;
    do
    {
        reg_misr.d16 = RD_PPT_REG(PRO_MISR);
    }
    while (reg_misr.reset_trig_int == 0);
    WR_PPT_REG(PRO_MISR, BIT6);
    /* workaround for dma error if reset when dma start */
    ppt_clear_dma_tx_done();
    /* trx interrupts may be already pended before reset */
    ppt_flush_trx_int();
    /* the hw rx fifo shall be cleared here after reset */
    ppt_flush_rx_fifo();
}

PPT_API_SECTION bool ppt_kill_hw_fsm(ppt_fsm_t fsm)
{
    if (fsm == PPT_FSM_PTX)
    {
        ppt_execute_instruction(PPT_HW_INSTRUCTION_PTX_DISABLE);
        ppt_clear_radio();
        PRO_MISR_TYPE reg_misr;
        do
        {
            reg_misr.d16 = RD_PPT_REG(PRO_MISR);
        }
        while (reg_misr.kill_ptx_int == 0);
        WR_PPT_REG(PRO_MISR, BIT4);
    }
    else if (fsm == PPT_FSM_PRX)
    {
        ppt_execute_instruction(PPT_HW_INSTRUCTION_PRX_DISABLE);
        ppt_clear_radio();
        PRO_MISR_TYPE reg_misr;
        do
        {
            reg_misr.d16 = RD_PPT_REG(PRO_MISR);
        }
        while (reg_misr.kill_prx_int == 0);
        WR_PPT_REG(PRO_MISR, BIT5);
    }
    else
    {
        return false;
    }

    /* clear all trx interrupts */
    ppt_flush_trx_int();
    /* the hw rx fifo shall be cleared here after kill */
    ppt_flush_rx_fifo();
    return true;
}

PPT_API_SECTION void ppt_psd_isr_handler_imp(void)
{
    for (uint8_t loop = 0; loop < modem_psd_get_entry_num(); loop++)
    {
        //DBG_DIRECT("psd: 0x%08x 0x%08x 0x%08x 0x%08x", g_modem_psd_report_array[loop].d32_array[0],
        //g_modem_psd_report_array[loop].d32_array[1], g_modem_psd_report_array[loop].d32_array[2],
        //g_modem_psd_report_array[loop].d32_array[3]);
        if (g_modem_psd_report_array[loop].signature_bit == 0)
        {
            int16_t mp_gain, psd_db, base_tmp;
            base_tmp = (get_phy_rssi0_dbm(modem_psd_get_rf_mode()) << 4);
            uint8_t chann = ppt_ctx->psd_mode.chann_start + ppt_ctx->psd_mode.chann_step * loop;
            mp_gain = g_modem_psd_report_array[loop].mp_gain_idx;
            if (modem_psd_get_scan_mode() == 2)
            {
                psd_db = g_modem_psd_report_array[loop].psd_avg_neg;
                psd_db = (psd_db >= 1024) ? ((psd_db - 2048)) : psd_db;
                ppt_ctx->psd_result[chann] = (psd_db + base_tmp + (mp_gain << (1 + 4))) << 3;
                if (chann <= PSD_CHANN_NUM - 2)
                {
                    psd_db = g_modem_psd_report_array[loop].psd_avg_dc;
                    psd_db = (psd_db >= 1024) ? ((psd_db - 2048)) : psd_db;
                    ppt_ctx->psd_result[chann + 1] = (psd_db + base_tmp + (mp_gain << (1 + 4))) << 3;
                }
                if (chann <= PSD_CHANN_NUM - 3)
                {
                    psd_db = g_modem_psd_report_array[loop].psd_avg_pos;
                    psd_db = (psd_db >= 1024) ? ((psd_db - 2048)) : psd_db;
                    ppt_ctx->psd_result[chann + 2] = (psd_db + base_tmp + (mp_gain << (1 + 4))) << 3;
                }
                //APP_PRINT_INFO4("ppt: psd chan %d, rssi %d %d %d", chann, ppt_ctx->psd_result[chann], ppt_ctx->psd_result[chann + 1], ppt_ctx->psd_result[chann + 2]);
            }
            else
            {
                psd_db = g_modem_psd_report_array[loop].psd_avg_dc;
                psd_db = (psd_db >= 1024) ? ((psd_db - 2048)) : psd_db;
                ppt_ctx->psd_result[chann] = (psd_db + base_tmp + (mp_gain << (1 + 4))) << 3;
                //APP_PRINT_INFO2("ppt: psd chan %d, rssi %d %d %d",chann, ppt_ctx->psd_result[chann]);
            }
        }
        else
        {
            break;
        }
    }

    RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x14), ppt_ctx->psd_tmp_flag);

    ppt_ctx->fsm = PPT_FSM_STANDBY;
    ppt_ctx->sync_flag = false;
    ppt_dlps_mac_idle = true;
    if (ppt_ctx->async_cb)
    {
        ppt_ctx->async_cb();
    }
}

void ppt_enable_psd(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_STANDBY)
    {
        return;
    }

    /* save modem param */
    ppt_ctx->psd_tmp_flag = RTK_READ_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x14));

    for (uint8_t loop = 0; loop < modem_psd_get_entry_num(); loop++)
    {
        g_modem_psd_report_array[loop].signature_bit = 3;
    }
    ppt_reg_psd_handler(ppt_psd_isr_handler_imp);
    ppt_ctx->fsm = PPT_FSM_PSD;
    ppt_ctx->sync_flag = true;
    ppt_ctx->async_cb = async_cb;
    ppt_dlps_mac_idle = false;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PSD_ENABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
    }
}

void ppt_disable_psd(void (*async_cb)(void))
{
    if (ppt_ctx->fsm != PPT_FSM_PSD)
    {
        return;
    }

    ppt_ctx->async_cb = async_cb;
    ppt_ctx->sync_flag = true;
    ppt_execute_instruction(PPT_HW_INSTRUCTION_PSD_DISABLE);
    if (ppt_ctx->async_cb == NULL)
    {
        while (ppt_ctx->sync_flag);
        ppt_ctx->fsm = PPT_FSM_STANDBY;
    }
}

void ppt_clear_psd_result(void)
{
    memset(ppt_ctx->psd_result, 0, sizeof(ppt_ctx->psd_result));
}

int16_t ppt_get_psd_result(uint8_t chann)
{
    if (chann >= PSD_CHANN_NUM)
    {
        return 0x7fff;
    }

    return ppt_ctx->psd_result[chann] / 128;
}

static void ppt_init_sw(void)
{
    for (uint8_t loop = 0; loop < PPT_TX_BUFFER_NUM; loop++)
    {
        if (ppt_ctx->tx_buffer[loop] == NULL)
        {
            ppt_ctx->tx_buffer[loop] = os_mem_alloc(RAM_TYPE_BUFFER_ON, PPT_TX_BUFFER_SIZE);
        }
    }

    if (ppt_ctx->rx_buffer == NULL)
    {
        ppt_ctx->rx_buffer = os_mem_alloc(RAM_TYPE_BUFFER_ON, PPT_RX_BUFFER_SIZE);
    }
}

static void ppt_deinit_sw(void)
{
    for (uint8_t loop = 0; loop < PPT_TX_BUFFER_NUM; loop++)
    {
        if (ppt_ctx->tx_buffer[loop])
        {
            os_mem_free(ppt_ctx->tx_buffer[loop]);
        }
    }

    if (ppt_ctx->rx_buffer)
    {
        os_mem_free(ppt_ctx->rx_buffer);
    }
    memset(ppt_ctx, 0, sizeof(ppt_ctx_t));
}

static void ppt_init_hw(void)
{
    uint16_t pro_reg;
    WR_PPT_REG(PRO_IMR, 0xFFFF);

    pro_reg = RD_PPT_REG(PRO_PACKET_CONTROL);
    ((PRO_PACKET_CONTROL_TYPE *)(&pro_reg))->crc_kept = 0;
    WR_PPT_REG(PRO_PACKET_CONTROL, pro_reg);

    pro_reg = RD_PPT_REG(PRO_RX_CONTROL);
    ((PRO_RX_CONTROL_TYPE *)(&pro_reg))->max_length_kept = 0;
    WR_PPT_REG(PRO_RX_CONTROL, pro_reg);

    pro_reg = RD_PPT_REG(PRO_MAX_LENGTH);
    ((PRO_MAX_LENGTH_TYPE *)(&pro_reg))->max_length = PPT_TX_BUFFER_SIZE;
    WR_PPT_REG(PRO_MAX_LENGTH, pro_reg);

    for (uint8_t entry = 0; entry < PPT_ENTRY_NUM; entry++)
    {
        ppt_set_auto_no_tx(entry, false);
        ppt_set_auto_ack(entry, false);
    }

    ppt_reset_trx_pkt_num();

    pro_reg = RD_PPT_REG(PRO_TX_TURN_AROUND_DELAY);
    ((PRO_TX_TURN_AROUND_DELAY_TYPE *)(&pro_reg))->pro_txon_early_1m = 0x8;
    WR_PPT_REG(PRO_TX_TURN_AROUND_DELAY, pro_reg);

    PRO_TRX_ON_DELAY_TYPE trx_on_delay = {.d16 = RD_PPT_REG(PRO_TRX_ON_DELAY)};
    trx_on_delay.pro_txon_delay = 58;
    trx_on_delay.pro_rxon_delay = 58;
    WR_PPT_REG(PRO_TRX_ON_DELAY, trx_on_delay.d16);

    pro_reg = RD_PPT_REG(PRO_RX_TIMEOUT_DELAY);
    ((PRO_RX_TIMEOUT_DELAY_TYPE *)(&pro_reg))->pro_txon_early_2m = 0x2;
    WR_PPT_REG(PRO_RX_TIMEOUT_DELAY, pro_reg);

    PRO_2M_TRX_ON_DELAY_TYPE trx_on_delay_2m = {.d16 = RD_PPT_REG(PRO_2M_TRX_ON_DELAY)};
    trx_on_delay_2m.pro_txon_delay_2m = 58;
    trx_on_delay_2m.pro_rxon_delay_2m = 58;
    WR_PPT_REG(PRO_2M_TRX_ON_DELAY, trx_on_delay_2m.d16);

    WR_PPT_REG(PRO_IMR, 0);
}

void ppt_init(void)
{
    ppt_init_sw();
    ppt_reset_core_domain(true);
#if 0
    bt_retention_domain_reset();
    phy_init(false);
#endif
#if TIME_DEBUG
    ppt_init_time_debug();
#endif
    ppt_init_hw();
    ppt_init_dma();
    ppt_init_misc();
}

void ppt_deinit(void)
{
    ppt_reset_core_domain(false);
    ppt_deinit_misc();
    ppt_deinit_dma();

    ppt_deinit_sw();
}
