/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.

*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
  * @file     ppt_driver.c
  * @brief    Source file for 2.4G module common driver.
  * @details
  * @author   Bill
  * @date     2025-12-1
  * @version  v1.0
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

#include <stdbool.h>
#include "trace.h"
#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "platform_utils.h"
#include "os_sync.h"
#include "ppt_driver.h"

#include "dlps.h"
//#include "power_manager_interface.h"
//#include "power_manager_slave.h"
//#include "power_manager_unit_platform.h"
#include "os_mem.h"

#define PPT_LOG_METHOD                      0 //!< 0: no log, 1: buffer log, 2: direct log
#define PPT_REG_ACCESS_TIM_DBG              (TIME_DEBUG && 0)
#define PPT_WR_WI_RD_BACK                   0

#define PPT_IS_ASIC                         0
#define PATCH_BTMAC_ISR                     1

#define BLUEWIZ_REG_BASE                    0x40050000UL
#define PPT_REG_ADDR_BASE                   (BLUEWIZ_REG_BASE + 0x1c00)

#ifndef UNUSED
#define UNUSED(expr) do { (void)(expr); } while (0)
#endif

void bt_retention_domain_reset(void);

bool ppt_enable = false;
volatile bool ppt_dlps_mac_idle = true;
bool (*ppt_dlps_mac_check_cb)(uint32_t *wakeup_time_diff);
void (*ppt_dlps_mac_enter_cb)(void);
void (*ppt_dlps_mac_exit_cb)(void);
void (*ppt_dlps_pf_enter_cb)(void);
void (*ppt_dlps_pf_exit_cb)(void);
bool ppt_tx_fixed_rptr[PPT_ENTRY_NUM];

struct
{
    uint32_t modem_acc[PPT_ENTRY_RX_NUM];
    uint8_t modem_acc_enable;
    uint8_t acc_byte3_flag; //!< 0: not set, 1: set for 1M, 2: set for 2M
    bool acc_similar_flag;
    bool mod_index_2m_flag;
} ppt_fw_ret;

#if PATCH_BTMAC_ISR
void *btmac_handler_temp;
#endif
void *bb_psd_handler_temp;

static void (*ppt_isr_handler)(void);
static void (*ppt_psd_isr_handler)(void);

#if TIME_DEBUG
uint8_t test_pin[] = {TEST_PIN};
uint32_t gpio_test_pin[sizeof(test_pin)];
bool level_test_pin[sizeof(test_pin)];
#endif

static PPT_DAT_SECTION ppt_reg_field_t ppt_lut_prefix[PPT_ENTRY_RX_NUM] =
{
    {PRO_PREFIX0_1, 0}, {PRO_PREFIX0_1, 8},
    {PRO_PREFIX2_3, 0}, {PRO_PREFIX2_3, 8},
    {PRO_PREFIX4_5, 0}, {PRO_PREFIX4_5, 8},
    {PRO_PREFIX6_7, 0}, {PRO_TX_ADDRESS_UPPER, 8}
};

static PPT_DAT_SECTION uint16_t ppt_lut_tx_addr[PPT_ENTRY_RX_NUM] =
{
    PRO_TX_ADDRESS_LOWER, PRO_TX_ADDRESS1_LOWER,
    PRO_TX_ADDRESS2_LOWER, PRO_TX_ADDRESS3_LOWER,
    PRO_TX_ADDRESS4_LOWER, PRO_TX_ADDRESS5_LOWER,
    PRO_TX_ADDRESS6_LOWER, PRO_TX_ADDRESS7_LOWER,
};

PPT_API_SECTION uint8_t ppt_swap_bits8(uint8_t inp)
{
    uint8_t retval = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        retval |= ((inp >> i) & 0x01) << (7 - i);
    }
    return retval;
}

PPT_API_SECTION uint16_t ppt_swap_bits16(uint16_t inp)
{
    uint16_t retval = 0;
    for (uint8_t i = 0; i < 16; i++)
    {
        retval |= ((inp >> i) & 0x01) << (15 - i);
    }
    return retval;
}

PPT_API_SECTION uint32_t ppt_swap_bits24(uint32_t inp)
{
    uint32_t retval = 0;
    for (uint8_t i = 0; i < 24; i++)
    {
        retval |= ((inp >> i) & 0x01) << (23 - i);
    }
    return retval;
}

PPT_API_SECTION uint32_t ppt_swap_bits32(uint32_t inp)
{
    uint32_t retval = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        retval |= ((inp >> i) & 0x01) << (31 - i);
    }
    return retval;
}

bool btmac_pm_check_inactive(void);
bool btmac_pm_check_active(void);
void btmac_pm_initiate_wakeup(void);
bool ppt_dlps_check_active(void);
bool ppt_dlps_check_inactive(void);
void ppt_dlps_initiate_wakeup(void);

PPT_API_SECTION uint16_t ppt_read_reg_dlps(uint16_t offset)
{
    uint16_t reg;

    /* wake up btmac */
    if (btmac_pm_check_inactive())
    {
        btmac_pm_initiate_wakeup();
    }
    /* wake up ppt */
    if (ppt_dlps_check_inactive())
    {
        ppt_dlps_initiate_wakeup();
    }

#if PPT_REG_ACCESS_TIM_DBG
    time_debug_level_high(TIME_DEBUG_CHAN_THREAD);
#endif
    reg = HAL_READ16(PPT_REG_ADDR_BASE, offset);
#if PPT_REG_ACCESS_TIM_DBG
    time_debug_level_low(TIME_DEBUG_CHAN_THREAD);
#endif

#if PPT_LOG_METHOD == 2
    DBG_DIRECT("PPTR %04x=%04x", offset, reg);
#elif PPT_LOG_METHOD == 1
    APP_PRINT_INFO2("ppt_read_reg: offset 0x%04x, value 0x%04x", offset, reg);
#endif
    return reg;
}

PPT_API_SECTION uint16_t ppt_read_reg_active(uint16_t offset)
{
    return HAL_READ16(PPT_REG_ADDR_BASE, offset);
}

PPT_API_SECTION void ppt_write_reg_dlps(uint16_t offset, uint16_t data)
{
    /* wake up btmac */
    if (btmac_pm_check_inactive())
    {
        btmac_pm_initiate_wakeup();
    }
    /* wake up ppt */
    if (ppt_dlps_check_inactive())
    {
        ppt_dlps_initiate_wakeup();
    }

#if PPT_REG_ACCESS_TIM_DBG
    time_debug_level_high(TIME_DEBUG_CHAN_THREAD);
#endif
    HAL_WRITE16(PPT_REG_ADDR_BASE, offset, data);
#if PPT_REG_ACCESS_TIM_DBG
    time_debug_level_low(TIME_DEBUG_CHAN_THREAD);
#endif

#if PPT_LOG_METHOD
    uint16_t read_data = 0;
#if PPT_WR_WI_RD_BACK
    read_data = ppt_read_reg(offset);
#endif
#endif
#if PPT_LOG_METHOD == 2
    DBG_DIRECT("PPTW %04x=%04x(r%04x)", offset, data, read_data);
#elif PPT_LOG_METHOD == 1
    APP_PRINT_INFO3("ppt_write_reg: offset 0x%04x, value 0x%04x(r%04x)", offset, data, read_data);
#endif
}

PPT_API_SECTION void ppt_write_reg_active(uint16_t offset, uint16_t data)
{
    HAL_WRITE16(PPT_REG_ADDR_BASE, offset, data);
}

uint16_t ppt_read_reg_def(uint16_t offset)
{
    ppt_read_reg = ppt_read_reg_active;
    ppt_write_reg = ppt_write_reg_active;
    return ppt_read_reg_dlps(offset);
}

void ppt_write_reg_def(uint16_t offset, uint16_t data)
{
    ppt_read_reg = ppt_read_reg_active;
    ppt_write_reg = ppt_write_reg_active;
    ppt_write_reg_dlps(offset, data);
}

uint16_t (*ppt_read_reg)(uint16_t offset) = ppt_read_reg_def;
void (*ppt_write_reg)(uint16_t offset, uint16_t data) = ppt_write_reg_def;

PPT_API_SECTION void ppt_reset_core_domain(bool ppt_mode)
{
    {
        /* proprietary reset */
        BT_SYS_CTRL_REG_S_TYPE bt_sys_reg;
        bt_sys_reg.d32 = VENDOR_READ(BT_SYS_CTRL_REG);
        bt_sys_reg.b.pro_rst_n = 0;
        VENDOR_WRITE(BT_SYS_CTRL_REG, bt_sys_reg.d32);

        bt_sys_reg.b.pro_rst_n = 1;
        VENDOR_WRITE(BT_SYS_CTRL_REG, bt_sys_reg.d32);
    }

    {
        /* clock enable */
        BT_SYS_CTRL_REG_S_TYPE bt_sys_reg;
        bt_sys_reg.d32 = VENDOR_READ(BT_SYS_CTRL_REG);
        bt_sys_reg.b.pro_clk_en = ppt_mode;
        VENDOR_WRITE(BT_SYS_CTRL_REG, bt_sys_reg.d32);
        platform_delay_us(2);
    }
    ppt_enable = ppt_mode;
}

PPT_API_SECTION void ppt_lock(uint8_t entry, bool lock)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry <= 1)
    {
        PRO_TX_CONTROL_TYPE tx_ctrl = {.d16 = RD_PPT_REG(PRO_TX_CONTROL)};
        if (entry == 0)
        {
            tx_ctrl.lock0 = lock;
        }
        else if (entry == 1)
        {
            tx_ctrl.lock1 = lock;
        }
        WR_PPT_REG(PRO_TX_CONTROL, tx_ctrl.d16);
    }
    else if (entry == 2)
    {
        PRO_GPIO_CONTROL_TYPE gpio_ctrl = {.d16 = RD_PPT_REG(PRO_GPIO_CONTROL)};
        gpio_ctrl.lock2 = lock;
        WR_PPT_REG(PRO_GPIO_CONTROL, gpio_ctrl.d16);
    }
    else
    {
        uint16_t offset = PRO_WHITE_FIELD_LENGTH_0 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_WHITE_FIELD_LENGTH_0_TYPE len0 = {.d16 = RD_PPT_REG(offset)};
        len0.lock3 = lock;
        WR_PPT_REG(offset, len0.d16);
    }
}

void ppt_set_auto_no_tx(uint8_t entry, bool enable)
{
    assert_param(entry < PPT_ENTRY_NUM);
    uint16_t pro_reg;
    if (entry == 0)
    {
        pro_reg = RD_PPT_REG(PRO_TX_CONTROL);
        ((PRO_TX_CONTROL_TYPE *)(&pro_reg))->auto_no_tx_option0 = enable;
        WR_PPT_REG(PRO_TX_CONTROL, pro_reg);
    }
    else if (entry == 1)
    {
        pro_reg = RD_PPT_REG(PRO_TRX_CONTROL);
        ((PRO_TRX_CONTROL_TYPE *)(&pro_reg))->auto_no_tx_option1 = enable;
        WR_PPT_REG(PRO_TRX_CONTROL, pro_reg);
    }
    else if (entry == 2)
    {
        pro_reg = RD_PPT_REG(PRO_GPIO_CONTROL);
        ((PRO_GPIO_CONTROL_TYPE *)(&pro_reg))->auto_no_tx_option2 = enable;
        WR_PPT_REG(PRO_GPIO_CONTROL, pro_reg);
    }
    else
    {
        uint16_t offset = PRO_WHITE_FIELD_LENGTH_0 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_WHITE_FIELD_LENGTH_0_TYPE len0 = {.d16 = RD_PPT_REG(offset)};
        len0.auto_no_tx_option3 = enable;
        WR_PPT_REG(offset, len0.d16);
    }
}

void ppt_clear_hw_no_tx(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    if (entry <= 1)
    {
        PRO_TX_CONTROL_TYPE tx_ctrl = {.d16 = RD_PPT_REG(PRO_TX_CONTROL)};
        if (entry == 0)
        {
            tx_ctrl.clear_hw_no_tx0 = 1;
        }
        else if (entry == 1)
        {
            tx_ctrl.clear_hw_no_tx1 = 1;
        }
        WR_PPT_REG(PRO_TX_CONTROL, tx_ctrl.d16);
    }
    else if (entry == 2)
    {
        PRO_GPIO_CONTROL_TYPE gpio_ctrl = {.d16 = RD_PPT_REG(PRO_GPIO_CONTROL)};
        gpio_ctrl.clear_hw_no_tx2 = 1;
        WR_PPT_REG(PRO_GPIO_CONTROL, gpio_ctrl.d16);
    }
    else
    {
        uint16_t offset = PRO_WHITE_FIELD_LENGTH_0 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_WHITE_FIELD_LENGTH_0_TYPE len0 = {.d16 = RD_PPT_REG(offset)};
        len0.clear_hw_no_tx3 = 1;
        WR_PPT_REG(offset, len0.d16);
    }
}

void ppt_set_auto_ack(uint8_t entry, bool enable)
{
    assert_param(entry < PPT_ENTRY_NUM);
    if (entry == 0)
    {
        WR_PPT_REG_FIELD(PRO_FIELD_LENGTH, auto_no_rdptr_option0, enable);
    }
    else if (entry == 1)
    {
        WR_PPT_REG_FIELD(PRO_TRX_CONTROL, auto_no_rdptr_option1, enable);
    }
    else if (entry == 2)
    {
        WR_PPT_REG_FIELD(PRO_GPIO_CONTROL, auto_no_rdptr_option2, enable);
    }
    else
    {
        uint16_t offset = PRO_FIELD_LENGTH3 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_FIELD_LENGTH3_TYPE len3 = {.d16 = RD_PPT_REG(offset)};
        len3.auto_no_rdptr_option3 = enable;
        WR_PPT_REG(offset, len3.d16);
    }
}

PPT_API_SECTION uint8_t ppt_trigger_fw_ack_lock(uint8_t entry, bool lock)
{
    assert_param(entry < PPT_ENTRY_NUM);
    if (ppt_tx_fixed_rptr[entry])
    {
        ppt_tx_fixed_rptr[entry] = false;
        if (entry < PPT_ENTRY_NUM_ORI)
        {
            PRO_PACKET_RELATED_CONTROL0_TYPE pkt_related_ctrl = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0)};
            pkt_related_ctrl.auto_add_rptr_option0 = 0;
            WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0,
                       pkt_related_ctrl.d16);
        }
        else
        {
            uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) << 1) + PRO_FIELD_LENGTH3;
            PRO_FIELD_LENGTH3_TYPE field_len = {.d16 = RD_PPT_REG(offset)};
            field_len.auto_add_rptr_option3 = 0;
            WR_PPT_REG(offset, field_len.d16);
        }
    }
    /* Fw trigger rptr ++ */
#if PPT_LOG_METHOD == 1
    UINT32 cur_bm = RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry));
#endif
    if (lock)
    {
        ppt_lock(entry, true);
    }
    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_PACKET_RELATED_CONTROL0_TYPE pkt_related_ctrl = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0)};
        pkt_related_ctrl.fw_ack0 = TRUE; // w1 ack, hw auto pull down in next T
        WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0, pkt_related_ctrl.d16);
    }
    else
    {
        uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) << 1) + PRO_WHITE_FIELD_LENGTH_0;
        PRO_WHITE_FIELD_LENGTH_0_TYPE white_field_len = {.d16 = RD_PPT_REG(offset)};
        white_field_len.fw_ack3 = 0;
        WR_PPT_REG(offset, white_field_len.d16);
    }
    //bzdma_update_fw_rptr_of_ble_data_ring_fifo(entry, 1, FALSE);
    uint8_t rd_ptr = Bzdma_Manager.BleTxEntSta[entry].seg_rptr;
    rd_ptr = (rd_ptr + 1) & (bzdma_supported_le_max_seg_num - 1);
    Bzdma_Manager.BleTxEntSta[entry].seg_rptr = rd_ptr;
    Bzdma_Manager.BleTxEntSta[entry].free_segs += 1;
    if (lock)
    {
        ppt_lock(entry, false);
    }
#if PPT_LOG_METHOD == 1
    APP_PRINT_INFO4("ppt_trigger_fw_ack: BZDMA bitmap 0x%x -> 0x%x, entry %d, rd ptr %d", cur_bm,
                    RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry)), entry,
                    rd_ptr);
#endif
    return rd_ptr;
}

PPT_API_SECTION uint8_t ppt_force_fw_ack(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    uint8_t rd_ptr = Bzdma_Manager.BleTxEntSta[entry].seg_rptr;
    uint32_t cur_bm = RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry));
    uint32_t cur_bm_n = cur_bm & (~(1 << rd_ptr));
    WR_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry), cur_bm_n);

    rd_ptr = (rd_ptr + 1) & (bzdma_supported_le_max_seg_num - 1);
    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_PACKET_RELATED_CONTROL0_TYPE pkt_related_ctrl = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0)};
        pkt_related_ctrl.auto_add_rptr_option0 = 1;
        pkt_related_ctrl.fw_rd_ptr0 = rd_ptr;
        WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0, pkt_related_ctrl.d16);
    }
    else
    {
        uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) << 1) + PRO_FIELD_LENGTH3;
        PRO_FIELD_LENGTH3_TYPE field_len = {.d16 = RD_PPT_REG(offset)};
        field_len.auto_add_rptr_option3 = 1;
        field_len.fw_rd_ptr3 = rd_ptr;
        WR_PPT_REG(offset, field_len.d16);
    }

    Bzdma_Manager.BleTxEntSta[entry].seg_rptr = rd_ptr;
    Bzdma_Manager.BleTxEntSta[entry].free_segs += 1;

    ppt_tx_fixed_rptr[entry] = true;
#if PPT_LOG_METHOD == 1
    APP_PRINT_INFO4("ppt_force_fw_ack: BZDMA bitmap 0x%x -> 0x%x, entry %d, rd ptr %d", cur_bm,
                    RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry)), entry,
                    rd_ptr);
#endif
    //pkt_related_ctrl.auto_add_rptr_option0 = 0;
    //WR_PPT_REG(PRO_PACKET_RELATED_CONTROL0, pkt_related_ctrl.d16);
    //bzdma_update_fw_rptr_of_ble_data_ring_fifo(entry, 1, FALSE);
    return rd_ptr;
}

void ppt_flush_rx_fifo(void)
{
    BT_SYS_CTRL_REG_S_TYPE bt_sys_reg;
    bt_sys_reg.d32 = VENDOR_READ(BT_SYS_CTRL_REG);
    bt_sys_reg.b.bt_br_edr_clk_en = 1;
    VENDOR_WRITE(BT_SYS_CTRL_REG, bt_sys_reg.d32);
    BB_write_baseband_register(INSTRUCTION_REGISTER, BB_FLUSH);
    bt_sys_reg.b.bt_br_edr_clk_en = 0;
    VENDOR_WRITE(BT_SYS_CTRL_REG, bt_sys_reg.d32);
}

void ppt_flush_trx_int(void)
{
    while (1)
    {
        PRO_MISR_TYPE reg_misr;
        reg_misr.d16 = RD_PPT_REG(PRO_MISR);
        if (reg_misr.tx_int)
        {
            PRO_TX_LENGTH_TYPE length = {.d16 = RD_PPT_REG(PRO_TX_LENGTH)}; // Read 0x6a to pop stack
        }
        else if (reg_misr.rx_int)
        {
            PRO_RX_HS_UPPER_TYPE rx_hs_upper = {.d16 = RD_PPT_REG(PRO_RX_HS_UPPER)}; //read 0x62 to pop rx stack
        }
        else
        {
            break;
        }
    }
}

void ppt_reset_trx_pkt_num(void)
{
    PRO_CBK_CTRL_TYPE cbk_ctrl = {.d16 = RD_PPT_REG(PRO_CBK_CTRL)};
    cbk_ctrl.reset_rx_pktnum0 = 1;
    cbk_ctrl.reset_rx_pktnum1 = 1;
    cbk_ctrl.reset_tx_pktnum0 = 1;
    cbk_ctrl.reset_tx_pktnum1 = 1;
    WR_PPT_REG(PRO_CBK_CTRL, cbk_ctrl.d16);
    cbk_ctrl.reset_rx_pktnum0 = 0;
    cbk_ctrl.reset_rx_pktnum1 = 0;
    cbk_ctrl.reset_tx_pktnum0 = 0;
    cbk_ctrl.reset_tx_pktnum1 = 0;
    WR_PPT_REG(PRO_CBK_CTRL, cbk_ctrl.d16);

    PRO_WHITE_FIELD_LENGTH_0_TYPE white0 = {.d16 = RD_PPT_REG(PRO_WHITE_FIELD_LENGTH_0)};
    white0.reset_rx_pktnum2 = 1;
    WR_PPT_REG(PRO_WHITE_FIELD_LENGTH_0, white0.d16);
    white0.reset_rx_pktnum2 = 0;
    WR_PPT_REG(PRO_WHITE_FIELD_LENGTH_0, white0.d16);

    for (uint8_t entry = PPT_ENTRY_NUM_ORI; entry < PPT_ENTRY_NUM; entry++)
    {
        uint8_t offset = PRO_WHITE_FIELD_LENGTH_0 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_WHITE_FIELD_LENGTH_0_TYPE white0 = {.d16 = RD_PPT_REG(offset)};
        white0.reset_rx_pktnum3 = 1;
        WR_PPT_REG(offset, white0.d16);
        white0.reset_rx_pktnum3 = 0;
        WR_PPT_REG(offset, white0.d16);
    }

    PRO_GPIO_CONTROL_TYPE gpio_ctrl = {.d16 = RD_PPT_REG(PRO_GPIO_CONTROL)};
    gpio_ctrl.reset_tx_pktnum2 = 1;
    WR_PPT_REG(PRO_GPIO_CONTROL, gpio_ctrl.d16);
    gpio_ctrl.reset_tx_pktnum2 = 0;
    WR_PPT_REG(PRO_GPIO_CONTROL, gpio_ctrl.d16);

    for (uint8_t entry = PPT_ENTRY_NUM_ORI; entry < PPT_ENTRY_NUM; entry++)
    {
        uint8_t offset = PRO_FIELD_LENGTH3 + ((entry - PPT_ENTRY_NUM_ORI) << 1);
        PRO_FIELD_LENGTH3_TYPE len3 = {.d16 = RD_PPT_REG(offset)};
        len3.reset_tx_pktnum3 = 1;
        WR_PPT_REG(offset, len3.d16);
        len3.reset_tx_pktnum3 = 0;
        WR_PPT_REG(offset, len3.d16);
    }
}

PPT_API_SECTION void ppt_execute_instruction(uint8_t instruction_code)
{
    if (instruction_code >= PPT_HW_INSTRUCTION_MAX)
    {
#if PPT_LOG_METHOD == 1
        APP_PRINT_ERROR1("ppt_execute_instruction: unknown instruction %d", instruction_code);
#endif
        return;
    }

#if PPT_LOG_METHOD == 1
    APP_PRINT_INFO1("ppt_execute_instruction: execute %d", instruction_code);
#endif
    PRO_INSTRUCTION_TYPE inst;// = {.d16 = RD_PPT_REG(PRO_INSTRUCTION)};
    inst.instruction = instruction_code;
    WR_PPT_REG(PRO_INSTRUCTION, inst.d16);
}

void ppt_set_crc_param(ppt_crc_param_t *param)
{
    PRO_PACKET_CONTROL_TYPE pkt_ctrl = {.d16 = RD_PPT_REG(PRO_PACKET_CONTROL)};
    pkt_ctrl.crc_field_length_2_0 = param->len;
    /* not support feature, shall not be set to true */
    //pkt_ctrl.crc_include_addr_enable = param->include_addr;
    WR_PPT_REG(PRO_PACKET_CONTROL, pkt_ctrl.d16);
}

void ppt_set_crc_entry_param(uint8_t entry, ppt_crc_entry_param_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    /* poly */
    uint16_t crc_poly_lower = param->value.poly & 0xffff;
    uint16_t crc_poly_upper = (param->value.poly >> 16) & 0xffff;
    uint16_t reg_offset_poly = PPT_REG_OFFSET(entry, PRO_CRC_POLYNOMIAL_REG0_LOWER,
                                              PRO_ONE_ENTRY_DW_SIZE, PRO_CRC_POLYNOMIAL_REG3_LOWER, 4);
    WR_PPT_REG(reg_offset_poly, crc_poly_lower);
    WR_PPT_REG(reg_offset_poly + 2, crc_poly_upper);

    /* init tx */
    uint16_t crc_init_lower = param->value.init & 0xffff;
    uint16_t crc_init_upper = (param->value.init >> 16) & 0xffff;
    uint16_t reg_offset_init = PPT_REG_OFFSET(entry, PRO_CRC_INIT0_LOWER, PRO_ONE_ENTRY_DW_SIZE,
                                              PRO_CRC_INIT3_LOWER, 4);
    WR_PPT_REG(reg_offset_init, crc_init_lower);
    WR_PPT_REG(reg_offset_init + 2, crc_init_upper);

    /* init rx */
    uint16_t reg_offset_init_rx = PPT_REG_OFFSET(entry, PRO_RX_CRC_INIT0_LOWER, 4,
                                                 PRO_RX_CRC_INIT3_LOWER, 4);
    WR_PPT_REG(reg_offset_init_rx, crc_init_lower);
    WR_PPT_REG(reg_offset_init_rx + 2, crc_init_upper);
}

void ppt_set_white_param(ppt_white_param_t *param)
{
    PRO_PACKET_CONTROL_TYPE packet_ctrl = {.d16 = RD_PPT_REG(PRO_PACKET_CONTROL)};
    packet_ctrl.white_dis = !param->enable;
    WR_PPT_REG(PRO_PACKET_CONTROL, packet_ctrl.d16);
}

void ppt_set_white_entry_param(uint8_t entry, ppt_white_entry_param_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_PACKET_RELATED_CONTROL0_TYPE pkt_related_ctrl = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0)};
        pkt_related_ctrl.white_field_length0 = param->len;
        WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0, pkt_related_ctrl.d16);
    }
    else
    {
        uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) << 1) + PRO_WHITE_FIELD_LENGTH_0;
        PRO_WHITE_FIELD_LENGTH_0_TYPE white0 = {.d16 = RD_PPT_REG(offset)};
        white0.white_field_length3 = param->len;
        WR_PPT_REG(offset, white0.d16);
    }

    uint16_t white_poly_l = param->value.poly & 0xffff;
    uint16_t white_poly_u = (param->value.poly >> 16) & 0xffff;
    uint16_t reg_offset_poly = PPT_REG_OFFSET(entry, PRO_WHITE_POLYNOMIAL_REG0_LOWER,
                                              PRO_ONE_ENTRY_DW_SIZE, PRO_WHITE_POLYNOMIAL_REG3_LOWER, 4);
    WR_PPT_REG(reg_offset_poly, white_poly_l);
    WR_PPT_REG(reg_offset_poly + 2, white_poly_u);

    uint16_t white_ini_l = param->value.init & 0xffff;
    uint16_t white_ini_u = (param->value.init >> 16) & 0xffff;
    uint16_t reg_offset_init = PPT_REG_OFFSET(entry, PRO_WHITE_INIT0_LOWER, PRO_ONE_ENTRY_DW_SIZE,
                                              PRO_WHITE_INIT3_LOWER, 4);
    WR_PPT_REG(reg_offset_init, white_ini_l);
    WR_PPT_REG(reg_offset_init + 2, white_ini_u);
}

void ppt_set_phy_rx_type(ppt_phy_type_t rx_phy)
{
    PRO_RX_CONTROL_TYPE rx_ctrl = {.d16 = RD_PPT_REG(PRO_RX_CONTROL)};
    rx_ctrl.rx_phy = rx_phy;
    WR_PPT_REG(PRO_RX_CONTROL, rx_ctrl.d16);
}

void ppt_set_phy_tx_type(uint8_t entry, ppt_phy_type_t tx_phy)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_PACKET_RELATED_CONTROL0_TYPE pkt_related_ctrl = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0)};
        pkt_related_ctrl.tx_phy0 = tx_phy;
        WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_PACKET_RELATED_CONTROL0, pkt_related_ctrl.d16);
    }
    else
    {
        PRO_WHITE_FIELD_LENGTH_0_TYPE white_field_len = {.d16 = RD_PPT_REG(2 * (entry - PPT_ENTRY_NUM_ORI) + PRO_WHITE_FIELD_LENGTH_0)};
        white_field_len.tx_phy3 = tx_phy;
        WR_PPT_REG(2 * (entry - PPT_ENTRY_NUM_ORI) + PRO_WHITE_FIELD_LENGTH_0, white_field_len.d16);
    }
}

void ppt_set_phy_mod_index_2m(bool change)
{
    if (ppt_fw_ret.mod_index_2m_flag != change)
    {
        if (change)
        {
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_7, TRANS_MODEM_REG(0x14), 0xfb34);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_7, TRANS_MODEM_REG(0x16), 0x3, 0x1);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x14), 0x3f, 0x28);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2c), 0x3fe, 0x50);
        }
        else
        {
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_7, TRANS_MODEM_REG(0x14), 0xffd3);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_7, TRANS_MODEM_REG(0x16), 0x3, 0x2);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x14), 0x3f, 0x1a);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2c), 0x3fe, 0x84);
        }
        ppt_fw_ret.mod_index_2m_flag = change;
    }
}

PPT_API_SECTION void ppt_set_phy_bank(uint8_t bank_idx)
{
    /* deprecated */
}

PPT_API_SECTION bool ppt_set_phy_channel(uint16_t freq_mhz)
{
    uint8_t bank = 0, channel = 0;
    if (false == ppt_get_bank_channel(freq_mhz, &bank, &channel))
    {
        return false;
    }
    PRO_CHANNEL_BANK_TYPE ch_bank;
    ch_bank.bank = bank;
    ch_bank.pro_channel = channel;
    WR_PPT_REG(PRO_CHANNEL_BANK, ch_bank.d16);
    /* update phy parameters along with rf bank */
    ppt_set_phy_bank(bank);
    return true;
}

void ppt_set_phy_param(ppt_phy_param_t *param)
{
    PRO_CHANNEL_BANK_TYPE ch_bank = {.d16 = RD_PPT_REG(PRO_CHANNEL_BANK)};
    ch_bank.bank = param->bank;
    ch_bank.pro_channel = param->channel;
    WR_PPT_REG(PRO_CHANNEL_BANK, ch_bank.d16);
    /* update phy parameters along with rf bank */
    ppt_set_phy_bank(param->bank);

    ppt_set_phy_rx_type(param->rx_phy);
}

void ppt_set_phy_entry_param(uint8_t entry, ppt_phy_entry_param_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    ppt_set_phy_tx_type(entry, param->tx_phy);
    ppt_set_preamble_len(entry, param->preamble_len);
}

void ppt_set_preamble_len(uint8_t entry, uint8_t preamble_len)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry == 0)
    {
        PRO_FIELD_LENGTH_TYPE field_len = {.d16 = RD_PPT_REG(PRO_FIELD_LENGTH)};
        field_len.preamble_field_length0_2_0 = preamble_len;
        WR_PPT_REG(PRO_FIELD_LENGTH, field_len.d16);
    }
    else if (entry == 1)
    {
        PRO_FIELD_LENGTH2_TYPE field_len2 = {.d16 = RD_PPT_REG(PRO_FIELD_LENGTH2)};
        field_len2.preamble_field_length1_2_0 = preamble_len;
        WR_PPT_REG(PRO_FIELD_LENGTH2, field_len2.d16);
    }
    else if (entry == 2)
    {
        PRO_GPIO_CONTROL_TYPE gpio_ctrl = {.d16 = RD_PPT_REG(PRO_GPIO_CONTROL)};
        gpio_ctrl.preamble_field_length2 = preamble_len;
        WR_PPT_REG(PRO_GPIO_CONTROL, gpio_ctrl.d16);
    }
    else
    {
        PRO_FIELD_LENGTH3_TYPE field_len = {.d16 = RD_PPT_REG(2 * (entry - PPT_ENTRY_NUM_ORI) + PRO_FIELD_LENGTH3)};
        field_len.preamble_field_length3_2_0 = preamble_len;
        WR_PPT_REG(2 * (entry - PPT_ENTRY_NUM_ORI) + PRO_FIELD_LENGTH3, field_len.d16);
    }
}

void ppt_set_pkt_format(ppt_pkt_format_t *param)
{
    PRO_RX_MATCH_CONTROL_TYPE rx_match_ctrl = {.d16 = RD_PPT_REG(PRO_RX_MATCH_CONTROL)};
    rx_match_ctrl.compare_prefix_option = param->addr_len == 5;
    WR_PPT_REG(PRO_RX_MATCH_CONTROL, rx_match_ctrl.d16);

    PRO_FIELD_LENGTH_TYPE field_len = {.d16 = RD_PPT_REG(PRO_FIELD_LENGTH)};
    field_len.address_field_length_2_0 = param->addr_len;
    field_len.hp_field_length_3_0 = param->hp_len;
    field_len.length_field_length_3_0 = param->length_len;
    WR_PPT_REG(PRO_FIELD_LENGTH, field_len.d16);

    PRO_HS_LENGTH_TYPE hs_len = {.d16 = RD_PPT_REG(PRO_HS_LENGTH)};
    hs_len.hs_field_length_4_0 = param->hs_len;
    WR_PPT_REG(PRO_HS_LENGTH, hs_len.d16);

    PRO_ORDER_CONTROL_TYPE order_ctrl = {.d16 = RD_PPT_REG(PRO_ORDER_CONTROL)};
    order_ctrl.hp_order = param->header_order;
    order_ctrl.hp_order_first = param->header_order;
    order_ctrl.length_order = param->header_order;
    order_ctrl.length_order_first = param->header_order;
    order_ctrl.hs_order = param->header_order;
    order_ctrl.hs_order_first = param->header_order;
    WR_PPT_REG(PRO_ORDER_CONTROL, order_ctrl.d16);
    WR_PPT_REG_FIELD(PRO_MAX_LENGTH, txfifo_out_split,
                     param->payload_order == PPT_FIELD_BIT_ORDER_MSB ? 1 : 0);
    WR_PPT_REG_FIELD(PRO_MAX_LENGTH, rxfifo_in_split,
                     param->payload_order == PPT_FIELD_BIT_ORDER_MSB ? 1 : 0);

    PRO_PREFIX6_7_TYPE prefix6_7 = {.d16 = RD_PPT_REG(PRO_PREFIX6_7)};
    prefix6_7.hp_order_first_rx = param->header_order;
    prefix6_7.length_order_first_rx = param->header_order;
    prefix6_7.hs_order_first_rx = param->header_order;
    WR_PPT_REG(PRO_PREFIX6_7, prefix6_7.d16);
}

void ppt_set_addon_len(uint8_t entry, ppt_addon_length_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_ADD_ON_LENGTH0_TYPE add_on_length = {.d16 = RD_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_ADD_ON_LENGTH0)};
        add_on_length.add_on_length0_7_0 = param->len;
        WR_PPT_REG(PRO_ENTRY_FIELD_BASE(entry) + PRO_ADD_ON_LENGTH0, add_on_length.d16);
    }
    else
    {
        uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) & (~BIT0)) + PRO_ADD_ON_LENGTH3;
        uint8_t field_offset = ((entry - PPT_ENTRY_NUM_ORI) & BIT0) << 3;
        OV_PPT_REG_FIELD(offset, field_offset, 8, param->len);
    }
}

void ppt_set_tx_power(uint8_t tx_gain)
{
    PRO_GAIN_TYPE gain = {.d16 = RD_PPT_REG(PRO_GAIN)};
    gain.rif_tx_gain = tx_gain;
    WR_PPT_REG(PRO_GAIN, gain.d16);
}

extern bool (*get_valid_txgain_index_from_tx_power)(int8_t, uint8_t, uint8_t, bool, uint8_t *,
                                                    int8_t *);
#define TXGAIN_LE_1M    0
#define TPM_MODE        0
void ppt_set_tx_power_dbm(float tx_power_dbm)
{
    uint8_t tx_gain;
    int8_t tx_power = tx_power_dbm;
    int8_t tx_power_act;
    //todo: set according to phy
    get_valid_txgain_index_from_tx_power(tx_power * 2, TXGAIN_LE_1M, TPM_MODE, TRUE, &tx_gain,
                                         &tx_power_act);
    ppt_set_tx_power(tx_gain);
}

void ppt_set_tx_addr(uint8_t entry, ppt_tx_addr_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    uint16_t tx_addr;
    tx_addr = param->tx_addr[0] | (param->tx_addr[1] << 8);
    WR_PPT_REG(ppt_lut_tx_addr[entry], tx_addr);

    tx_addr = param->tx_addr[2] | (param->tx_addr[3] << 8);
    WR_PPT_REG(ppt_lut_tx_addr[entry] + 2, tx_addr);

    OV_PPT_REG_FIELD(ppt_lut_tx_addr[entry] + 4, 0, 8, param->tx_addr[4]);
}

void ppt_check_rx_addr(uint8_t bit_th)
{
    PRO_RX_CONTROL_TYPE rx_ctrl = {.d16 = RD_PPT_REG(PRO_RX_CONTROL)};
    PRO_RX_MATCH_CONTROL_TYPE rx_match_ctrl = {.d16 = RD_PPT_REG(PRO_RX_MATCH_CONTROL)};
    uint32_t rx_addr0, rx_addr1;
    bool byte3_flag = false;
    bool similar_flag = false;
    if ((rx_match_ctrl.rx_match_enable_bit_7_0 & 0x1) == 0x1)
    {
        rx_addr0 = RD_PPT_REG(PRO_BASE0_LOWER) + (RD_PPT_REG(PRO_BASE0_UPPER) << 16);
        if ((rx_addr0 >> 24) == 0xff)
        {
            byte3_flag = true;
        }
    }
    if ((rx_match_ctrl.rx_match_enable_bit_7_0 & 0x2) == 0x2)
    {
        rx_addr1 = RD_PPT_REG(PRO_BASE1_LOWER) + (RD_PPT_REG(PRO_BASE1_UPPER) << 16);
        if ((rx_addr1 >> 24) == 0xff)
        {
            byte3_flag = true;
        }
    }

    uint8_t acc_byte3_flag = byte3_flag ? (rx_ctrl.rx_phy == PPT_PHY_TYPE_BLE_2M ? 2 : 1) : 0;
    if (acc_byte3_flag != ppt_fw_ret.acc_byte3_flag)
    {
        /* default value */
        if (ppt_fw_ret.acc_byte3_flag == 2) /* 2M PHY */
        {
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x10), 0x1 << 3, 0x1 << 3);
        }
        else if (ppt_fw_ret.acc_byte3_flag == 1)/* 1M PHY */
        {
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x68), 0x7 << 9, 0x4 << 9);
        }

        /* modified value */
        if (acc_byte3_flag == 2)/* 2M PHY */
        {
            /* need restore after dlps */
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x10), 0x1 << 3, 0x0 << 3);
        }
        else if (acc_byte3_flag == 1)/* 1M PHY */
        {
            /* need restore after dlps */
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x68), 0x7 << 9, 0x1 << 9);
        }

        ppt_fw_ret.acc_byte3_flag = acc_byte3_flag;
    }

    if ((rx_match_ctrl.rx_match_enable_bit_7_0 & 0x3) == 0x3)
    {
        uint32_t addr_diff = rx_addr0 ^ rx_addr1;
        uint8_t diff_count = 0;
        while (addr_diff)
        {
            diff_count += 1;
            addr_diff &= addr_diff - 1;
        }
        if (diff_count <= bit_th)
        {
            similar_flag = true;
        }
    }

    if (similar_flag != ppt_fw_ret.acc_similar_flag)
    {
        ppt_fw_ret.acc_similar_flag = similar_flag;
        if (similar_flag) /* modified value */
        {
            /* need restore after dlps */
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x4a), 0xff << 0, 0x33 << 0);
            /* 1M PHY */
            {
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x3C), 0x1 << 11, 0x1 << 11);
                /* need restore after dlps */
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x2C), 0x7 << 13, 0x0 << 13);
            }
            /* 2M PHY */
            {
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2a), 0x1 << 3, 0x1 << 3);
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2a), 0x7 << 4, 0x0 << 4);
            }
        }
        else /* default value */
        {
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x4a), 0xff << 0, 0xcc << 0);
            /* 1M PHY */
            {
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x3C), 0x1 << 11, 0x0 << 11);
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x2C), 0x7 << 13, 0x2 << 13);
            }
            /* 2M PHY */
            {
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2a), 0x1 << 3, 0x0 << 3);
                RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2a), 0x7 << 4, 0x6 << 4);
            }
        }
    }
}

#define ppt_set_prefix(entry, prefix) OV_PPT_REG_FIELD(ppt_lut_prefix[entry].reg_offset, ppt_lut_prefix[entry].field_offset, 8, prefix)

void ppt_set_rx_addr(uint8_t entry, ppt_rx_addr_t *param)
{
    assert_param(entry < PPT_ENTRY_RX_NUM);
    PRO_RX_MATCH_CONTROL_TYPE rx_match_ctrl = {.d16 = RD_PPT_REG(PRO_RX_MATCH_CONTROL)};
    if (param->enable)
    {
        rx_match_ctrl.rx_match_enable_bit_7_0 |= BIT(entry);
        ppt_fw_ret.modem_acc_enable |= BIT(entry);
        uint16_t addr = param->rx_addr[0] + (param->rx_addr[1] << 8);
        uint16_t addr1 = param->rx_addr[2] + (param->rx_addr[3] << 8);
        uint8_t prefix = param->rx_addr[4];
        uint32_t base_for_modem = ppt_swap_bits32(addr + (addr1 << 16));
        ppt_fw_ret.modem_acc[entry] = base_for_modem;
        if (entry == 0)
        {
            WR_PPT_REG(PRO_BASE0_LOWER, addr);
            WR_PPT_REG(PRO_BASE0_UPPER, addr1);
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x1c), base_for_modem);
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x1e), base_for_modem >> 16);
        }
        else
        {
            WR_PPT_REG(PRO_BASE1_LOWER, addr);
            WR_PPT_REG(PRO_BASE1_UPPER, addr1);
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x24), base_for_modem);
            RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x26), base_for_modem >> 16);
            // Set Bit0 to 1 if use 2 rx match filter
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x14), BIT0, 1);
            WR_PPT_REG_FIELD(PRO_TX_PKT_NUM2, modem_compare2_addr, 1);
            // disable acc false alarm protect scheme
            //RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x56), 0x3, 0);
        }
        ppt_set_prefix(entry, prefix);
    }
    else
    {
        rx_match_ctrl.rx_match_enable_bit_7_0 &= ~BIT(entry);
        ppt_fw_ret.modem_acc_enable &= ~BIT(entry);
        if (entry >= 1 && (ppt_fw_ret.modem_acc_enable & 0xfe) == 0)
        {
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x14), BIT0, 0);
            WR_PPT_REG_FIELD(PRO_TX_PKT_NUM2, modem_compare2_addr, 0);
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x56), 0x3, 0x3);
        }
    }
    WR_PPT_REG(PRO_RX_MATCH_CONTROL, rx_match_ctrl.d16);
}

PPT_API_SECTION void ppt_set_hp(uint8_t entry, uint8_t hp)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_HP_0_TYPE hp0 = {.d16 = RD_PPT_REG(PRO_HP_0 + PRO_ENTRY_FIELD_BASE(entry))};
        hp0.hp_0_7_0 = hp;
        WR_PPT_REG(PRO_HP_0 + PRO_ENTRY_FIELD_BASE(entry), hp0.d16);
    }
    else
    {
        uint16_t reg_offet = PRO_HP_3 + (((entry - PPT_ENTRY_NUM_ORI) >> 1) << 1);
        uint8_t field_offset = ((entry - PPT_ENTRY_NUM_ORI) & 0x1) << 3;
        OV_PPT_REG_FIELD(reg_offet, field_offset, 8, hp);
    }
}

PPT_API_SECTION void ppt_set_length(uint8_t entry, uint16_t length)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry < PPT_ENTRY_NUM_ORI)
    {
        PRO_LENGTH_0_TYPE len = {.d16 = RD_PPT_REG(PRO_LENGTH_0 + PRO_ENTRY_FIELD_BASE(entry))};
        len.length_0_14_0 = length;
        WR_PPT_REG(PRO_LENGTH_0 + PRO_ENTRY_FIELD_BASE(entry), len.d16);
    }
    else
    {
        uint16_t offset = ((entry - PPT_ENTRY_NUM_ORI) << 1) + PRO_LENGTH_3;
        PRO_LENGTH_3_TYPE len = {.d16 = RD_PPT_REG(offset)};
        len.length_3_14_0 = length;
        WR_PPT_REG(offset, len.d16);
    }
}

PPT_API_SECTION void ppt_set_hs(uint8_t entry, uint32_t hs)
{
    assert_param(entry < PPT_ENTRY_NUM);

    uint16_t reg_offet_l = PPT_REG_OFFSET(entry, PRO_HS_0_LOWER, PRO_ONE_ENTRY_DW_SIZE, PRO_HS_3_LOWER,
                                          4);
    WR_PPT_REG(reg_offet_l, hs & 0xffff);
    OV_PPT_REG_FIELD(reg_offet_l + 2, 1, 15, (hs >> 16) & 0xffff);
}

void ppt_set_tx_header(uint8_t entry, ppt_header_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    ppt_set_hp(entry, param->hp);
    /* length not used if dma_length_mode is enabled */
    ppt_set_length(entry, param->length);
    ppt_set_hs(entry, param->hs);
}

void ppt_set_tifs(uint8_t tifs_us)
{
    PRO_TX_TURN_AROUND_DELAY_TYPE tx_delay = {.d16 = RD_PPT_REG(PRO_TX_TURN_AROUND_DELAY)};
    tx_delay.pro_tx2rx_delay_base = tifs_us - 22;
    WR_PPT_REG(PRO_TX_TURN_AROUND_DELAY, tx_delay.d16);
    PRO_RX_TURN_AROUND_DELAY_TYPE rx_delay = {.d16 = RD_PPT_REG(PRO_RX_TURN_AROUND_DELAY)};
    rx_delay.pro_rx2tx_delay_base = tifs_us - 20;
    WR_PPT_REG(PRO_RX_TURN_AROUND_DELAY, rx_delay.d16);
}

void ppt_set_timing_latch(ppt_timing_latch_t *param)
{
    PRO_CBK_CTRL_TYPE cbk_ctrl = {.d16 = RD_PPT_REG(PRO_CBK_CTRL)};
    cbk_ctrl.tx_time_enable = param->tx_on;
    WR_PPT_REG(PRO_CBK_CTRL, cbk_ctrl.d16);

    PRO_RX_CONTROL_TYPE rx_ctrl = {.d16 = RD_PPT_REG(PRO_RX_CONTROL)};
    rx_ctrl.acchit_time_enable = param->acc_hit;
    WR_PPT_REG(PRO_RX_CONTROL, rx_ctrl.d16);
}

void ppt_set_gpio_param(ppt_gpio_param_t *param)
{
    PRO_RESET_CTRL_REG_TYPE reset_ctrl = {.d16 = RD_PPT_REG(PRO_RESET_CTRL_REG)};
    reset_ctrl.gpio_tx_enable = param->enable;
    reset_ctrl.gpio_in_enable = param->int_enable;
    WR_PPT_REG(PRO_RESET_CTRL_REG, reset_ctrl.d16);
}

void ppt_set_gpio_entry_param(uint8_t entry, ppt_gpio_entry_param_t *param)
{
    assert_param(entry < PPT_ENTRY_NUM);

    if (entry == 0)
    {
        PRO_GPIO_TX_DELAY_TYPE gpio_tx_delay = {.d16 = RD_PPT_REG(PRO_GPIO_TX_DELAY)};
        gpio_tx_delay.gpio_tx_delay_15_0 = param->gpio_delay;
        WR_PPT_REG(PRO_GPIO_TX_DELAY, gpio_tx_delay.d16);
    }
    else if (entry == 1)
    {
        PRO_GPIO_TX_DELAY1_TYPE gpio_tx_delay1 = {.d16 = RD_PPT_REG(PRO_GPIO_TX_DELAY1)};
        gpio_tx_delay1.gpio_tx_delay1_15_0 = param->gpio_delay;
        WR_PPT_REG(PRO_GPIO_TX_DELAY1, gpio_tx_delay1.d16);
    }
    else if (entry == 2)
    {
        PRO_GPIO_TX_DELAY2_TYPE gpio_tx_delay2 = {.d16 = RD_PPT_REG(PRO_GPIO_TX_DELAY2)};
        gpio_tx_delay2.gpio_tx_delay2_15_0 = param->gpio_delay;
        WR_PPT_REG(PRO_GPIO_TX_DELAY2, gpio_tx_delay2.d16);
    }
    else
    {
        /* no setting here */
    }
}

void ppt_set_ptx_mode(ppt_ptx_mode_t *param)
{
    PRO_TX_CONTROL_TYPE tx_ctrl = {.d16 = RD_PPT_REG(PRO_TX_CONTROL)};
    tx_ctrl.periodic_enable = param->periodic_mode;
    WR_PPT_REG(PRO_TX_CONTROL, tx_ctrl.d16);

    PRO_TRX_CONTROL_TYPE trx_ctrl = {.d16 = RD_PPT_REG(PRO_TRX_CONTROL)};
    trx_ctrl.ack_enable = param->ack_mode;
    WR_PPT_REG(PRO_TRX_CONTROL, trx_ctrl.d16);

    if (param->periodic_mode)
    {
        PRO_PERIODIC_INTERVAL_TYPE periodic_interval;
        periodic_interval.d16 = param->periodic_interval;
        WR_PPT_REG(PRO_PERIODIC_INTERVAL, periodic_interval.d16);
        PRO_TRX_CONTROL_TYPE trx_ctrl = {.d16 = RD_PPT_REG(PRO_TRX_CONTROL)};
        trx_ctrl.periodic_interval_19_16 = param->periodic_interval >> 16;
        WR_PPT_REG(PRO_TRX_CONTROL, trx_ctrl.d16);
    }
}

void ppt_set_prx_mode(ppt_prx_mode_t *param)
{
    PRO_RX_CONTROL_TYPE rx_ctrl = {.d16 = RD_PPT_REG(PRO_RX_CONTROL)};
    rx_ctrl.continuous_rx = param->continuous_mode;
    WR_PPT_REG(PRO_RX_CONTROL, rx_ctrl.d16);

    PRO_TRX_CONTROL_TYPE trx_ctrl = {.d16 = RD_PPT_REG(PRO_TRX_CONTROL)};
    trx_ctrl.ack_enable = param->ack_mode;
    WR_PPT_REG(PRO_TRX_CONTROL, trx_ctrl.d16);
}

void ppt_set_psd_mode(ppt_psd_mode_t *param)
{
    assert_param(param->chann_start <= param->chann_stop);
    assert_param(param->chann_stop < PSD_CHANN_NUM);
    assert_param(param->chann_step < PSD_CHANN_NUM);

    PRO_PSD_RANGE0_TYPE psd_range0 = {.d16 = RD_PPT_REG(PRO_PSD_RANGE0)};
    psd_range0.pro_psd_start = param->chann_start;
    psd_range0.pro_psd_end = param->chann_stop;
    WR_PPT_REG(PRO_PSD_RANGE0, psd_range0.d16);

    PRO_PSD_RANGE1_TYPE psd_range1 = {.d16 = RD_PPT_REG(PRO_PSD_RANGE1)};
    psd_range1.pro_psd_step = param->chann_step;
    psd_range1.pro_psd_mode = param->mode;
    WR_PPT_REG(PRO_PSD_RANGE1, psd_range1.d16);

    PRO_PSD_TIMEOUT_TYPE psd_timeout = {.d16 = RD_PPT_REG(PRO_PSD_TIMEOUT)};
    psd_timeout.pro_psd_timeout = param->timeout;
    WR_PPT_REG(PRO_PSD_TIMEOUT, psd_timeout.d16);
}

#define DIGI_DBG_BT     1
#define DBG_PIN_GROUP_A 0
extern void (*debug_port_open)(uint8_t debug_mode);
extern void (*debug_port_set_pin_bit_map)(uint8_t pin_group, uint32_t dbg_bitmap);

void ppt_dbg_port_open(uint16_t bt_port, uint16_t ppt_port, uint16_t ppt_port2)
{
    /* baseband port */
    VENDOR_WRITE(0x30, (VENDOR_READ(0x30)&BIT31) | 0x40);
    WR_16BIT_IO(BB_BASE_ADDR, 0x1FE, bt_port);

    /* 2.4g port */
    WR_PPT_REG(PRO_DEBUG_SEL, ppt_port);
    WR_PPT_REG(PRO_DEBUG_SEL2, ppt_port2);

    /* sys */
    debug_port_open(DIGI_DBG_BT);
}

void ppt_dbg_port_set_pin(uint8_t pin_group, uint32_t dbg_bitmap)
{
#if PPT_IS_ASIC
    debug_port_set_pin_bit_map(pin_group, dbg_bitmap);
#endif
}

void ppt_dbg_port(uint16_t bt_port, uint16_t ppt_port, uint16_t ppt_port2, uint32_t dbg_bitmap)
{
    ppt_dbg_port_set_pin(DBG_PIN_GROUP_A, dbg_bitmap);
    ppt_dbg_port_open(bt_port, ppt_port, ppt_port2);
}

void ppt_dump_nonzero_reg(void)
{
    DBG_DIRECT("ppt_dump_nonzero_reg: start");
    uint16_t offset = 0;
    while (offset < PRO_REG_TOTAL_SIZE)
    {
        uint16_t value = RD_PPT_REG(offset);
        if (value != 0)
        {
            DBG_DIRECT("ppt_dump_nonzero_reg: 0x%04x = 0x%04x", offset, value);
        }
        offset += 2;
    }
    DBG_DIRECT("ppt_dump_nonzero_reg: end");
}

bool ppt_get_bank_channel(uint16_t frequency, uint8_t *bank, uint8_t *channel)
{
    if (frequency > 2530)
    {
        return false;
    }
    else if (frequency > 2480)
    {
        *bank = 2;
        *channel = frequency - 2452;
    }
    else if (frequency >= 2402)
    {
        *bank = 0;
        *channel = frequency - 2402;
    }
    else if (frequency >= 2348)
    {
        *bank = 1;
        *channel = frequency - 2348;
    }
    else
    {
        return false;
    }
    return true;
}

static const uint8_t le_logic_chan[40] =
{
    37, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 38, 11, 12, 13, 14, 15, 16, 17, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 39
};

int8_t ppt_get_ble_logic_chan(uint8_t bank, uint8_t channel)
{
    if (bank == 0 && (channel & 0x1) == 0 && channel <= 78)
    {
        return le_logic_chan[channel / 2];
    }
    else
    {
        return -1;
    }
}

int8_t ppt_get_ble_logic_chan_via_freq(uint16_t frequency)
{
    if (frequency >= 2402 && frequency <= 2480 && ((frequency & 0x1) == 0))
    {
        return le_logic_chan[(frequency - 2402) / 2];
    }
    else
    {
        return -1;
    }
}

uint32_t ppt_get_ble_white_init(uint8_t le_logic_chan)
{
    uint32_t white_init = 1;
    le_logic_chan = ppt_swap_bits8(le_logic_chan);
    white_init |= le_logic_chan >> 1;
    return white_init;
}

#if PATCH_BTMAC_ISR
PPT_ISR_SECTION static uint8_t btmac_isr_handler(void *pbuf)
#else
PPT_ISR_SECTION static void btmac_isr_handler(void)
#endif
{
    BTMAC_ISR_STATUS_REG reg = btmac_isr_status_reg_read();
    //APP_PRINT_INFO1("btmac_isr_handler 0x%08x!", reg.d32);
    if (reg.pro_intr_r)
    {
        if (ppt_isr_handler)
        {
            ppt_isr_handler();
        }
        else
        {
            DBG_DIRECT("ppt: fail, ISR isn't registered!");
        }
    }
#if PATCH_BTMAC_ISR
    return false;
#endif
}

PPT_ISR_SECTION static uint8_t btmac_psd_isr_handler(void *buf)
{
    ppt_clear_radio();
    if (ppt_psd_isr_handler)
    {
        ppt_psd_isr_handler();
    }

    return true;
}

PPT_API_SECTION bool ppt_push_tx_fifo(uint8_t entry, uint8_t *payload, uint16_t payload_len)
{
    bool ret;
    assert_param(entry < PPT_ENTRY_NUM);
    uint32_t s = os_lock();
#if PPT_LOG_METHOD == 1
    UINT32 cur_bm = RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry));
#endif
    /* config bzdma */
    BZDMA_BLEONLY_TX_DESC_FRAGMENT TxFragCache = { .DWord = 0 };
    TxFragCache.start_addr_ex = (UINT32)payload;
    TxFragCache.len_ex = payload_len;
    TxFragCache.isLast = TRUE;
    ret = bzdma_send_packet_to_ble_data_ring_fifo(entry, LL_LLID_DATA_PDU_START,
                                                  &TxFragCache, 1, 0);
    ret = true;
#if PPT_LOG_METHOD == 1
    APP_PRINT_INFO5("ppt_push_tx_fifo: BZDMA bitmap 0x%x -> 0x%x, entry %d, wr ptr %d, len %d", cur_bm,
                    RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry)), entry,
                    Bzdma_Manager.BleTxEntSta[entry].seg_wptr, payload_len);
#endif
    os_unlock(s);
    return ret;
}

PPT_API_SECTION static UINT8 bzdma_update_packet_to_ble_data_ring_fifo(UINT8 conn_entry, UINT8 llid,
                                                                       BZDMA_BLEONLY_TX_DESC_FRAGMENT *pfrag, UINT8 Nfrag,
                                                                       UINT8 codeind)
{
    BZDMA_BLEONLY_DATA_TX_DESC_ENTRY_STATUS *pTxEnt;
    BZDMA_BLEONLY_TX_DESC_SEGMENT *pTxSeg;
    BZDMA_BLEONLY_TX_DESC_FRAGMENT *pTxFrag;
    UINT8 cur_wptr;
    UINT8 i;
    UINT8 PktSize = 0;

    pTxEnt = &Bzdma_Manager.BleTxEntSta[conn_entry];
    if (pTxEnt->free_segs == bzdma_supported_le_max_seg_num)
    {
        /* fifo is empty */
        return FALSE;
    }

    cur_wptr = (pTxEnt->seg_wptr + bzdma_supported_le_max_seg_num - 1) & (bzdma_supported_le_max_seg_num
                                                                          - 1);
    pTxSeg = &pTxEnt->pTxSegDesc[cur_wptr];
    pTxFrag = &pTxEnt->pTxFragDesc[cur_wptr * 2];

    /* fill all fragments */
    for (i = 0; i < Nfrag; i++)
    {
        pTxFrag[i].DWord = pfrag[i].DWord;
        UINT16 local_len;
        local_len = pfrag[i].len_ex;
        PktSize +=  local_len;
        if (local_len == 0)
        {
            /* hw can not handle zero length !! */
            return FALSE;
        }
    }

    /* fill full packet sie to tx segment */
    pTxSeg->len = PktSize;
    pTxSeg->llid = llid;
    return TRUE;
}

PPT_API_SECTION bool ppt_update_tx_fifo(uint8_t entry, uint8_t *payload, uint16_t payload_len)
{
    bool ret;
    assert_param(entry < PPT_ENTRY_NUM);
    uint32_t s = os_lock();
#if PPT_LOG_METHOD == 1
    UINT32 cur_bm = RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry));
#endif
    /* config bzdma */
    BZDMA_BLEONLY_TX_DESC_FRAGMENT TxFragCache = { .DWord = 0 };
    TxFragCache.start_addr_ex = (UINT32)payload;
    TxFragCache.len_ex = payload_len;
    TxFragCache.isLast = TRUE;
    ret = bzdma_update_packet_to_ble_data_ring_fifo(entry, LL_LLID_DATA_PDU_START,
                                                    &TxFragCache, 1, 0);
    ret = true;
#if PPT_LOG_METHOD == 1
    APP_PRINT_INFO5("ppt_update_tx_fifo: BZDMA bitmap 0x%x -> 0x%x, entry %d, wr ptr %d, len %d",
                    cur_bm,
                    RD_U32_BZDMA_REG(BZDMA_REG_BLEONLY_ENTRY_SEGMENT_VALID(entry)), entry,
                    Bzdma_Manager.BleTxEntSta[entry].seg_wptr, payload_len);
#endif
    os_unlock(s);
    return ret;
}

PPT_API_SECTION bool ppt_pop_rx_fifo_wait_done(void)
{
    bzdma_wait_rxcmd_complete();
    return true;
}

PPT_API_SECTION bool ppt_pop_rx_fifo_with_wait_option(uint8_t entry, uint8_t *pdu, uint16_t pdu_len,
                                                      bool wait)
{
    assert_param(entry < PPT_ENTRY_NUM);

    /* config bzdma */
    BZDMA_RX_DESC_SEGMENT rxdesc = { .DWord = 0 };
    rxdesc.addr = (uint32_t)pdu;
    rxdesc.len = pdu_len;
    rxdesc.flush = FALSE;
    bzdma_send_burst_rxcmd_and_wait_complete(&rxdesc, 1,
                                             BZDMA_RX_PID_ACL, entry, wait);
    return true;
}

PPT_API_SECTION bool ppt_pop_rx_fifo(uint8_t entry, uint8_t *pdu, uint16_t pdu_len)
{
    return ppt_pop_rx_fifo_with_wait_option(entry, pdu, pdu_len, TRUE);
}

uint8_t ppt_flush_tx_fifo(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);

    Bzdma_Manager.BleTxEntSta[entry].seg_wptr = Bzdma_Manager.BleTxEntSta[entry].seg_rptr;
    Bzdma_Manager.BleTxEntSta[entry].free_segs = bzdma_supported_le_max_seg_num;
    bzdma_ble_set_segment_valid_bitmap(entry, 0);
    return Bzdma_Manager.BleTxEntSta[entry].seg_rptr;
}

PPT_API_SECTION uint8_t ppt_get_tx_fifo_rdptr(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    return Bzdma_Manager.BleTxEntSta[entry].seg_rptr;
}

PPT_API_SECTION void ppt_set_tx_fifo_rdptr(uint8_t entry, uint8_t ptr)
{
    assert_param(entry < PPT_ENTRY_NUM);
    Bzdma_Manager.BleTxEntSta[entry].seg_rptr = ptr;
}

PPT_API_SECTION uint8_t ppt_get_tx_fifo_wrptr(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    return Bzdma_Manager.BleTxEntSta[entry].seg_wptr;
}

PPT_API_SECTION uint8_t ppt_get_tx_fifo_pend_pkt_num(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    return bzdma_supported_le_max_seg_num - Bzdma_Manager.BleTxEntSta[entry].free_segs;
}

PPT_API_SECTION uint8_t ppt_get_tx_fifo_free_pkt_num(uint8_t entry)
{
    assert_param(entry < PPT_ENTRY_NUM);
    return Bzdma_Manager.BleTxEntSta[entry].free_segs;
}

PPT_API_SECTION void ppt_reset_tx_fifo(uint8_t entry)
{
    ppt_set_tx_fifo_rdptr(entry, PPT_TX_FIFO_SIZE - 1);
    ppt_force_fw_ack(entry);
    ppt_flush_tx_fifo(entry);
}

PPT_API_SECTION int8_t ppt_get_rssi(uint16_t rssi_raw)
{
    return calculate_log_from_rssi(rssi_raw, 0xff, PPT_RF_MODE);
}

void ppt_init_dma(void)
{
    bzdma_supported_le_max_seg_num = PPT_TX_FIFO_SIZE;
    for (uint8_t loop = 0; loop < PPT_ENTRY_NUM; loop++)
    {
        Bzdma_Manager.BleTxEntSta[loop].seg_rptr = 0;
        Bzdma_Manager.BleTxEntSta[loop].seg_wptr = 0;
        Bzdma_Manager.BleTxEntSta[loop].free_segs = bzdma_supported_le_max_seg_num;
        bzdma_ble_set_segment_valid_bitmap(loop, 0);
    }
}

void ppt_deinit_dma(void)
{
    bzdma_supported_le_max_seg_num = 16;
    for (uint8_t loop = 0; loop < PPT_ENTRY_NUM; loop++)
    {
        Bzdma_Manager.BleTxEntSta[loop].seg_rptr = PPT_TX_FIFO_SIZE - 1;
        ppt_force_fw_ack(loop);
        Bzdma_Manager.BleTxEntSta[loop].seg_wptr = 0;
        Bzdma_Manager.BleTxEntSta[loop].free_segs = bzdma_supported_le_max_seg_num;
        bzdma_ble_set_segment_valid_bitmap(loop, 0);
    }
}

void ppt_init_misc(void)
{
#if PATCH_BTMAC_ISR
    btmac_handler_temp = rcp_BTMAC_Handler;
    rcp_BTMAC_Handler = (PF_ROM_CODE_PATCH_FUNC)btmac_isr_handler;
#else
    RamVectorTableUpdate(BTMAC_VECTORn, btmac_isr_handler);
#endif
    bb_psd_handler_temp = rcp_BB_handle_psd_end_intr_func;
    rcp_BB_handle_psd_end_intr_func = (PF_ROM_CODE_PATCH_FUNC)btmac_psd_isr_handler;

    /* modem init 1: proprietary mode, 0: normal mode */
    RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x66), BIT15, BIT15);
    /* force tx gain */
    //RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_3, TRANS_MODEM_REG(0x5c), 0x7840);

    /* workaround for rx empty pkt */
    //uint16_t reg = ((uint16_t) * ((volatile uint16_t *)(0x40050000 + 0x18e)));
    //TODO: BB_write_baseband_register(0x18e, reg | BIT15);
}

void ppt_deinit_misc(void)
{
    //uint16_t reg = ((uint16_t) * ((volatile uint16_t *)(0x40050000 + 0x18e)));
    //TODO: BB_write_baseband_register(0x18e, reg & (~BIT15));

    RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x66), BIT15, 0);
    //TODO:ll_hw_init(false);

#if PATCH_BTMAC_ISR
    rcp_BTMAC_Handler = (PF_ROM_CODE_PATCH_FUNC)btmac_handler_temp;
#else
    RamVectorTableUpdate(BTMAC_VECTORn, BTMAC_Handler);
#endif
    rcp_BB_handle_psd_end_intr_func = (PF_ROM_CODE_PATCH_FUNC)bb_psd_handler_temp;
}

PPT_API_SECTION uint32_t ppt_crc_eng_cal(ppt_crc_eng_param_t *param, uint8_t *data,
                                         uint32_t data_len)
{
    uint32_t crc_hw = 0;
    uint32_t init = param->init;

    WR_PPT_REG(PRO_CRC_ENGINE_POLY, param->poly & 0xffff);
    WR_PPT_REG(PRO_CRC_ENGINE_POLY1, (param->poly >> 16) & 0xffff);
    WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_field_length, param->length >> 3);
    WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_data_length_in_bit, param->bit_mode);
    WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_input_endian, param->input_endian);
    WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_output_endian, 0);

    uint8_t step_len = param->bit_mode ? 40 : 5;
    for (uint32_t loop = 0; loop < data_len; loop += step_len)
    {
        if (param->output_endian && loop + step_len >= data_len)
        {
            WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_output_endian, 1);
        }

        WR_PPT_REG(PRO_CRC_ENGINE_INIT, init & 0xffff);
        WR_PPT_REG(PRO_CRC_ENGINE_INIT1, (init >> 16) & 0xffff);

        uint32_t data_idx = param->bit_mode ? (loop >> 3) : loop;
        WR_PPT_REG(PRO_CRC_ENGINE_IN, data[data_idx + 0] + (data[data_idx + 1] << 8));
        WR_PPT_REG(PRO_CRC_ENGINE_IN1, data[data_idx + 2] + (data[data_idx + 3] << 8));
        WR_PPT_REG_FIELD(PRO_CRC_ENGINE_IN2, crc_engine_input_data_39_32, data[data_idx + 4]);

        uint8_t len = data_len - loop;
        if (len > step_len)
        {
            len = step_len;
        }
        WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_data_length, param->bit_mode ? len : len - 1);

        WR_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_en, 1);
        while (1)
        {
            bool run = RD_PPT_REG_FIELD(PRO_CRC_ENGINE_EN, crc_engine_en);
            if (run == false)
            {
                break;
            }
        }

        uint16_t crc_l = RD_PPT_REG(PRO_CRC_ENGINE_OUT);
        uint16_t crc_h = RD_PPT_REG(PRO_CRC_ENGINE_OUT1);
        crc_hw = (crc_h << 16) + crc_l;
        init = crc_hw;
    }

    if (param->output_endian == 1)
    {
        crc_hw >>= 32 - param->length;
    }
    return crc_hw;
}

void ppt_reg_handler(void (*handler)(void))
{
    ppt_isr_handler = handler;
}

void ppt_reg_psd_handler(void (*handler)(void))
{
    ppt_psd_isr_handler = handler;
}

#if TIME_DEBUG
void ppt_init_time_debug(void)
{
    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, (FunctionalState)ENABLE);
    for (uint8_t loop = 0; loop < sizeof(test_pin) / sizeof(test_pin[0]); loop++)
    {
        Pad_Config(test_pin[loop], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
        Pinmux_Config(test_pin[loop], DWGPIO);
        gpio_test_pin[loop] = GPIO_GetPin(test_pin[loop]);
        GPIO_InitTypeDef GPIO_InitStruct;
        GPIO_StructInit(&GPIO_InitStruct);
        GPIO_InitStruct.GPIO_Pin        = gpio_test_pin[loop];
        GPIO_InitStruct.GPIO_Mode       = GPIO_Mode_OUT;
        GPIO_Init(&GPIO_InitStruct);
        GPIO->DATAOUT |= gpio_test_pin[loop];
        GPIO->DATAOUT &= ~gpio_test_pin[loop];
    }
}

PPT_API_SECTION void ppt_store_time_debug(void)
{
    for (uint8_t loop = 0; loop < sizeof(test_pin) / sizeof(test_pin[0]); loop++)
    {
        Pad_Config(test_pin[loop], PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE,
                   level_test_pin[loop] ? PAD_OUT_HIGH : PAD_OUT_LOW);
    }
}

PPT_API_SECTION void ppt_restore_time_debug(void)
{
    for (uint8_t loop = 0; loop < sizeof(test_pin) / sizeof(test_pin[0]); loop++)
    {
        Pad_Config(test_pin[loop], PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE,
                   PAD_OUT_HIGH);
    }
}
#endif

PPT_API_SECTION static PMCheckResult ppt_dlps_mac_check(uint32_t cur_clk,
                                                        uint32_t pre_sys_lv_wakeup_time_diff,
                                                        uint32_t *unit_wakeup_time_diff)
{
    if (ppt_enable == false)
    {
        return PM_CHECK_PASS;
    }
    //DBG_DIRECT("ppt mac check");
    //DBG_DIRECT("ppt mac check %04x, %04x", RD_PPT_REG(PRO_DSM_READ), RD_PPT_REG(PRO_DSM_READ1));

    BT_SYS_CTRL_REG_S_TYPE bt_sys_reg;
    bt_sys_reg.d32 = VENDOR_READ(BT_SYS_CTRL_REG);
    if (bt_sys_reg.b.pro_rst_n != 0)
    {
        uint16_t pro_dsm = RD_PPT_REG(PRO_DSM_READ);
        if ((pro_dsm & (BIT6 + BIT11 + BIT12)) != (BIT6 + BIT11 + BIT12))
        {
            return PM_CHECK_FAIL;
        }
        uint16_t pro_dsm1 = RD_PPT_REG(PRO_DSM_READ1);
        if ((pro_dsm1 & (BIT0 + BIT7 + BIT13)) != (BIT0 + BIT7 + BIT13))
        {
            return PM_CHECK_FAIL;
        }
        if ((pro_dsm1 & (BIT1 + BIT2)) != 0)
        {
            return PM_CHECK_FAIL;
        }
        uint16_t isr_sta = RD_PPT_REG(PRO_MISR) & PPT_ISR_VALID_BITS_MASK;
        uint16_t isr_msk = RD_PPT_REG(PRO_IMR) & PPT_ISR_VALID_BITS_MASK;
        if (isr_sta & (~isr_msk))
        {
            return PM_CHECK_FAIL;
        }
    }

    if (ppt_dlps_mac_idle == false)
    {
        return PM_CHECK_FAIL;
    }
    if (ppt_dlps_mac_check_cb != NULL)
    {
        if (ppt_dlps_mac_check_cb(unit_wakeup_time_diff) == false)
        {
            return PM_CHECK_FAIL;
        }
    }
    return PM_CHECK_PASS;
}

PPT_API_SECTION static void ppt_dlps_mac_store(void)
{
    if (ppt_enable == false)
    {
        return;
    }

    if (ppt_dlps_mac_enter_cb)
    {
        ppt_dlps_mac_enter_cb();
    }

    ppt_read_reg = ppt_read_reg_dlps;
    ppt_write_reg = ppt_write_reg_dlps;
}

PPT_API_SECTION static void ppt_dlps_mac_restore(void)
{
    if (ppt_enable == false)
    {
        return;
    }
    //DBG_DIRECT("ppt mac restore");

    ppt_reset_core_domain(true);
    ppt_read_reg = ppt_read_reg_active;
    ppt_write_reg = ppt_write_reg_active;

    for (uint8_t loop = 0; loop < PPT_ENTRY_RX_NUM; loop++)
    {
        if (ppt_fw_ret.modem_acc_enable & BIT(loop))
        {
            uint32_t base_for_modem = ppt_fw_ret.modem_acc[loop];
            if (loop == 0)
            {
                RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x1c), base_for_modem);
                RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x1e), base_for_modem >> 16);
            }
            else if (loop == 1)
            {
                RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x24), base_for_modem);
                RTK_WRITE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x26), base_for_modem >> 16);
            }
        }
    }

    if (ppt_fw_ret.acc_byte3_flag == 2)/* 2M PHY */
    {
        /* need restore after dlps */
        RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x10), 0x1 << 3, 0x0 << 3);
    }
    else if (ppt_fw_ret.acc_byte3_flag == 1)/* 1M PHY */
    {
        /* need restore after dlps */
        RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x68), 0x7 << 9, 0x1 << 9);
    }

    if (ppt_fw_ret.acc_similar_flag)
    {
        RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x4a), 0xff << 0, 0x33 << 0);
        /* 1M PHY */
        {
            RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_0, TRANS_MODEM_REG(0x2C), 0x7 << 13, 0x0 << 13);
        }
    }

    if (ppt_fw_ret.mod_index_2m_flag)
    {
        RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_2, TRANS_MODEM_REG(0x14), 0x3f, 0x28);
        RTK_UPDATE_MODEM_REG_PI(MODEM_PI_PAGE_6, TRANS_MODEM_REG(0x2c), 0x3fe, 0x50);
    }

    if (ppt_dlps_mac_exit_cb)
    {
        ppt_dlps_mac_exit_cb();
    }
}

PPT_API_SECTION static void ppt_dlps_pf_enter(void)
{
    if (ppt_enable == false)
    {
        return;
    }
    //DBG_DIRECT("ppt pf enter");
    if (ppt_dlps_pf_enter_cb)
    {
        ppt_dlps_pf_enter_cb();
    }
#if TIME_DEBUG
    ppt_store_time_debug();
#endif
}

PPT_API_SECTION static void ppt_dlps_pf_exit(void)
{
    if (ppt_enable == false)
    {
        return;
    }
    //DBG_DIRECT("ppt pf exit");
#if TIME_DEBUG
    ppt_restore_time_debug();
#endif
    if (ppt_dlps_pf_exit_cb)
    {
        ppt_dlps_pf_exit_cb();
    }
}

void ppt_dlps_init(void)
{
    /* no way to unregister callback, so only register once */
    static bool ppt_dlps_pf_cb_reg = false;
    if (ppt_dlps_pf_cb_reg == false)
    {
        ppt_dlps_pf_cb_reg = true;
        platform_pm_register_callback_func(ppt_dlps_pf_enter, PLATFORM_PM_STORE);
        platform_pm_register_callback_func(ppt_dlps_pf_exit, PLATFORM_PM_PEND);

        PowerManagerSlaveUnit *ppt_pm = (PowerManagerSlaveUnit *)os_mem_zalloc(RAM_TYPE_DATA_ON,
                                                                               sizeof(PowerManagerSlaveUnit));
        ppt_pm->check_func = ppt_dlps_mac_check;
        ppt_pm->enter_func = ppt_dlps_mac_store;
        ppt_pm->exit_func = ppt_dlps_mac_restore;
        ppt_pm->unit_id = PM_UNIT_PROPRIETARY;
        power_manager_slave_register_unit(PM_UNIT_PROPRIETARY, ppt_pm);
    }
}

void ppt_dlps_deinit(void)
{
    ppt_read_reg = ppt_read_reg_def;
    ppt_write_reg = ppt_write_reg_def;
}

PPT_API_SECTION bool ppt_dlps_check_active(void)
{
    return (power_manager_interface_get_unit_status(PM_SLAVE_PROPRIETARY,
                                                    PM_UNIT_PROPRIETARY) == PM_UNIT_ACTIVE);
}

PPT_API_SECTION bool ppt_dlps_check_inactive(void)
{
    return (power_manager_interface_get_unit_status(PM_SLAVE_PROPRIETARY,
                                                    PM_UNIT_PROPRIETARY) == PM_UNIT_INACTIVE);
}

PPT_API_SECTION void ppt_dlps_initiate_wakeup(void)
{
    power_manager_master_initiate_wakeup(PM_SLAVE_PROPRIETARY,
                                         (PMSystemLevel)((PM_UNIT_PROPRIETARY >> 2) & 0x3),
                                         PM_UNIT_PROPRIETARY);
    while (ppt_dlps_check_inactive());
}

void ppt_dlps_reg_mac_check_cb(bool (*check_cb)(uint32_t *wakeup_time_diff))
{
    ppt_dlps_mac_check_cb = check_cb;
}

void ppt_dlps_reg_mac_enter_cb(void (*enter_cb)(void))
{
    ppt_dlps_mac_enter_cb = enter_cb;
}

void ppt_dlps_reg_mac_exit_cb(void (*exit_cb)(void))
{
    ppt_dlps_mac_exit_cb = exit_cb;
}

void ppt_dlps_reg_pf_enter_cb(void (*enter_cb)(void))
{
    ppt_dlps_pf_enter_cb = enter_cb;
}

void ppt_dlps_reg_pf_exit_cb(void (*exit_cb)(void))
{
    ppt_dlps_pf_exit_cb = exit_cb;
}
