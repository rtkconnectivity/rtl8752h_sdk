/***************************************************************************
 Copyright (C) RealTek Ltd.
 This module is a confidential and proprietary property of RealTek and
 a possession or use of this module requires written permission of RealTek.
 ***************************************************************************/
#ifndef BT_SYSCALL_OPCODE_H
#define BT_SYSCALL_OPCODE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  \brief BT system call op-code for 15.4 definition.
*/
typedef enum
{
    LOWERSTACK_SYSCALL_OPCODE_SET_ZB_ACTIVE = 10, // set ZB active for PHY arbitration (same PHY)
    LOWERSTACK_SYSCALL_OPCODE_GET_ZB_ENTRY_WINDOW = 11,
    LOWERSTACK_SYSCALL_OPCODE_REGISTER_MAC_LPS_CALLBACK = 14,
    LOWERSTACK_SYSCALL_OPCODE_ZB_CHNL_INFO = 15,
    LOWERSTACK_SYSCALL_OPCODE_ZB_GET_BT_INFO = 16,
    LOWERSTACK_SYSCALL_OPCODE_ZB_2_WL_MBOX = 17,
    LOWERSTACK_SYSCALL_OPCODE_SET_ZB_RADIO_STATE = 18,  // set ZB radio for coexistence with Wi-Fi, BT (different PHY)
    LOWERSTACK_SYSCALL_OPCODE_ZB_PTA_INI = 19,  // for PTA init through
    LOWERSTACK_SYSCALL_OPCODE_ZB_READ_PTA_REG = 20, // read PTA register through BT SysCall
    LOWERSTACK_SYSCALL_OPCODE_ZB_WRITE_PTA_REG = 21,  // write PTA register through BT SysCall
    LOWERSTACK_SYSCALL_OPCODE_SET_BT_SCOREBOARD = 22,  // set BT to WIFI scoreboard
} LOWERSTACK_SYSCALL_OPCODE;

#endif  /* BT_SYSCALL_OPCODE_H */
