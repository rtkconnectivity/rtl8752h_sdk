/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      zb_cfg.c
   * @brief     IEEE802.15.4 config file source
   * @author    felix
   * @date      2025-01-17
   * @version   v1.0
  **************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  **************************************************************************************
  */
#include "cfg_item_api.h"
#include "occd_parser.h"
#include "patch_header_check.h"
#include "zb_cfg.h"
extern uint32_t get_img_header_format_size(void);

/* If MAC_PRINTF is not defined, use DBG_DIRECT */
extern int dbg_printf(const char *format, ...);
#ifndef MAC_PRINTF
#include <trace.h>
#define MAC_PRINTF DBG_DIRECT
#endif

#ifndef MAC_LINE_ENDING
#define MAC_LINE_ENDING ""
#endif

#define ZB_MAC_ADDR_MODULE_ID        (MODULE_ID_ZB)
#define ZB_MAC_ADDR_OFFSET           (0x0)
#define ZB_MAC_ADDR_LEN              (0x8)
#define GET_ZB_MODULE_INDEX(id) ((id) - MODULE_ID_ZB)
#define ZB_CFG_MODULE_NUM (MODULE_ID_ZB_MAX - MODULE_ID_ZB)

typedef struct _zb_config_s
{
    uint8_t euid[8];  // Offset 0: EUI64
} __attribute__((packed)) zb_config_t __attribute__((aligned(4)));

static zb_config_t zb_cfg =
{
    .euid = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
};

static const MODULE_INFO zb_cfg_info[ZB_CFG_MODULE_NUM] =
{
    [GET_ZB_MODULE_INDEX(MODULE_ID_ZB)] = {
        .base = (uint32_t *) &zb_cfg,
        .size = sizeof(zb_cfg),
    },
};

void zb_cfg_euid_get(uint8_t *p_mac_addr)
{
    if (p_mac_addr)
    {
        memcpy(p_mac_addr, zb_cfg.euid, ZB_MAC_ADDR_LEN);
    }
}

void parse_zigbee_cfg_from_occd(void)
{
    PARSING_PARAMETER_FORMAT parsing_info =
    {
        .Signature = SYS_CFG_SIGNATURE,
        .max_size = OCCD_PAYLOAD_SIZE,
        .sys_cfg = (uint8_t *)get_header_addr_by_img_id(OCCD) + get_img_header_format_size(),
    };

    if ((sizeof(zb_cfg_info) / sizeof(MODULE_INFO)) != ZB_CFG_MODULE_NUM)
    {
        MAC_PRINTF("parse_zigbee_cfg_from_occd fail. ZBMAC_CFG_MODULE_NUM"MAC_LINE_ENDING);
        return;
    }

    for (size_t i = 0; i < ZB_CFG_MODULE_NUM; i++)
    {
        uint32_t ret = parse_sys_cfg_to_mem(&parsing_info, (i + MODULE_ID_ZB),
                                            (uint8_t *)zb_cfg_info[i].base,
                                            zb_cfg_info[i].size);
        if (ret != PARSE_SUCCESS)
        {
            MAC_PRINTF("parse_zigbee_cfg_from_occd fail. ret %u id 0x%x"MAC_LINE_ENDING, ret,
                       (i + MODULE_ID_ZB));
        }
    }
}

bool cfg_update_zb_mac(uint8_t *p_mac_addr)
{
    bool ret = false;

    ConfigEntry_T cfg_item;

    uint8_t data[ZB_MAC_ADDR_LEN];
    uint8_t mask[ZB_MAC_ADDR_LEN];

    memcpy(data, p_mac_addr, ZB_MAC_ADDR_LEN);
    memset(mask, 0xFF,       ZB_MAC_ADDR_LEN);

    cfg_item.Offset = ZB_MAC_ADDR_OFFSET;
    cfg_item.Length = ZB_MAC_ADDR_LEN;
    cfg_item.pData = data;
    cfg_item.pMask = mask;

    //MAC_PRINTF("Ready to update MAC!"MAC_LINE_ENDING);

    ret = cfg_add_item(ZB_MAC_ADDR_MODULE_ID, &cfg_item);

    //MAC_PRINTF("update cfg MAC %d"MAC_LINE_ENDING, ret);
    return ret;
}

int cfg_read_zb_mac(uint8_t *p_mac_addr)
{
    CFG_SERACH_RESULT_E ret;
    ConfigEntry_T cfg_item;

    uint8_t data[ZB_MAC_ADDR_LEN];
    uint8_t mask[ZB_MAC_ADDR_LEN];

    memset(data, 0xFF, ZB_MAC_ADDR_LEN);
    memset(mask, 0xFF, ZB_MAC_ADDR_LEN);

    cfg_item.Offset = ZB_MAC_ADDR_OFFSET;
    cfg_item.Length = ZB_MAC_ADDR_LEN;
    cfg_item.pData = data;
    cfg_item.pMask = mask;

    ret = cfg_read_item(ZB_MAC_ADDR_MODULE_ID, &cfg_item);
    if (ret == CFG_SERACH_ENTRY_SUCCESS)
    {
        memcpy(p_mac_addr, data, ZB_MAC_ADDR_LEN);
    }

    /*MAC_PRINTF("cfg_read_zb_mac: mac %02X %02X %02X %02X %02X %02X %02X %02X"MAC_LINE_ENDING,
        p_mac_addr[0], p_mac_addr[1], p_mac_addr[2], p_mac_addr[3],
        p_mac_addr[4], p_mac_addr[5], p_mac_addr[6], p_mac_addr[7]);*/
    return ret;
}
