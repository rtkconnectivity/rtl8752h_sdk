/*
 * Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
 */

#include "trace.h"
#include "ppt_cfg.h"
#include "ppt_simple.h"

void ppt_cfg(void)
{
    uint16_t frequency = FREQUENCY;
    uint8_t bank, channel;
    ppt_get_bank_channel(frequency, &bank, &channel);
    APP_PRINT_INFO3("ppt_cfg: %dMHz %d %d", frequency, bank, channel);

    ppt_init();

    {
        ppt_phy_param_t param =
        {
            .rx_phy = PHY_TYPE
        };
        param.bank = bank;
        param.channel = channel;
        ppt_set_phy_param(&param);
    }

    {
        ppt_phy_entry_param_t param =
        {
            .preamble_len = PREAMBLE_LEN,
            .tx_phy = PHY_TYPE
        };
        ppt_set_phy_entry_param(0, &param);
    }

    {
        ppt_white_entry_param_t param =
        {
            .len = 7,
            .value = {0x91, 0x53}
        };
        int8_t logic_idx = ppt_get_ble_logic_chan(bank, channel);
        if (logic_idx >= 0)
        {
            param.value.init = ppt_get_ble_white_init(logic_idx);
        }
        ppt_set_white_entry_param(0, &param);
    }

    {
        ppt_pkt_format_t param =
        {
            .addr_len = ACCESS_ADDRESS_LEN,
            .hp_len = HEADER_PREFIX_LEN * 8,
            .length_len = HEADER_LENGTH_LEN * 8,
            .hs_len = HEADER_SUFFIX_LEN * 8,
            .header_order = BIT_ORDER,
            .payload_order = BIT_ORDER
        };
        ppt_set_pkt_format(&param);
    }

    {
        ppt_crc_param_t param =
        {
            .len = CRC_LEN,
            .include_addr = false
        };
        ppt_set_crc_param(&param);
    }

    {
        ppt_crc_entry_param_t param =
        {
            .value = {0x0100065b, 0x555555}
        };
        ppt_set_crc_entry_param(0, &param);
    }

    {
        ppt_tx_addr_t param =
        {
            .tx_addr = {ACCESS_ADDRESS}
        };
        ppt_set_tx_addr(0, &param);
    }

    {
        ppt_rx_addr_t param =
        {
            .enable = true,
            .rx_addr = {ACCESS_ADDRESS}
        };
        ppt_set_rx_addr(0, &param);
    }

#if TURNAROUND_DELAY != 0
    ppt_set_tifs(TURNAROUND_DELAY);
#endif

    ppt_set_tx_power_dbm(4);
}

