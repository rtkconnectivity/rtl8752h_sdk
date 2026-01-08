/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
   * @file      occd_parser.h
   * @brief     Routines to create App task and handle events & messages
   * @author    Justin.Wu
   * @date      2023-6-12
   * @version   v1.0
  **************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  **************************************************************************************
  */
#ifndef OCCD_PARSER
#define OCCD_PARSER

#include <stdint.h>


#define OCCD_ADDRESS                                0x00801000
#define OCCD_PAYLOAD_SIZE                           (0xc00)


typedef struct ConfigHeaderSection
{
    uint32_t Signature_Field;
    uint16_t Data_Length;
    uint16_t Module_ID;
} __attribute__((packed)) ConfigHeaderSection_T;

typedef struct ConfigEntry
{
    uint16_t Offset;
    uint8_t  Length;
    uint8_t  *pData;
    uint8_t  *pMask;
} __attribute__((packed)) ConfigEntry_T;

typedef struct
{
    uint32_t *base;
    uint32_t size;
} MODULE_INFO;

typedef struct
{
    uint32_t Signature;
    uint32_t max_size;
    void *sys_cfg;
} PARSING_PARAMETER_FORMAT;

enum
{
    // Boot ROM
    MODULE_ID_BOOT_CFG = 0,
    MODULE_ID_CLOCK_TREE,
    MODULE_ID_BOOT_ROM_MAX,    //The BOOT ROM config will be parsed and updated earlier in the Boot code, so flash layout info should be in BOOT_CFG
    // Platform
    MODULE_ID_SYS_INIT = MODULE_ID_BOOT_ROM_MAX,
    MODULE_ID_OS,
    MODULE_ID_RFC,
    MODULE_ID_MODEM,
    MODULE_ID_PHY,
    MODULE_ID_PMU,
    MODULE_ID_SYS_ROM_MAX,   //The SYS ROM config will be parsed and updated in patch entry.
    // lowerstack
    MODULE_ID_LOWERSTACK = MODULE_ID_SYS_ROM_MAX,
    MODULE_ID_STACK_ROM_MAX,
    // upperstack
    MODULE_ID_UPPERSTACK = MODULE_ID_STACK_ROM_MAX,
    MODULE_ID_UPPERSTACK_ROM_MAX,
    // ZB
    MODULE_ID_ZB = MODULE_ID_UPPERSTACK_ROM_MAX,
    MODULE_ID_ZB_MAX,
    MODULE_ID_MAX = MODULE_ID_ZB_MAX,
};

enum
{
    PARSE_SUCCESS = 0,
    PARSE_ERROR_ID_MISMATCH,
    PARSE_ERROR_NO_CONFIG,               /*did not do the parsing operation*/
    PARSE_ERROR_ENTRY_SIZE_MISMATCH,     /*entry size do not match header legnth*/
    PARSE_ERROR_HEADER_LENGTH_MISMATCH,  /*header length > occd max size */
    PARSE_ERROR_MEM_OFFSET_EXCEED,       /*memory offset > memory size*/
};

typedef enum
{
    SYSTEM_CALL_GET_SYS_PATCH_EXIST_OPCODE = 0x01,
    SYSTEM_CALL_PARSE_SYS_CFG = 0x02,
    SYSTEM_CALL_ENTER_POWERDOWN_DIRECTLY = 0x04,
    SYSTEM_CALL_PHY_MODEM_SWITCH_TPMK_BANK = 0x09,
    SYSTEM_CALL_PMU_REQUEST_LDO_AUDIO_REF = 0x0A,
} T_SYSTEM_CALL_OPCODE;


typedef struct
{
    PARSING_PARAMETER_FORMAT *p_parse_info;
    uint16_t id;
    uint8_t *mem;
    uint32_t size;
} SYSTEM_CALL_PARSE_SYS_CFG_PARAM;

uint32_t parse_sys_cfg_to_mem(PARSING_PARAMETER_FORMAT *p_parse_info, uint16_t id, uint8_t *mem,
                              uint32_t size);

#endif // OCCD_PARSER
