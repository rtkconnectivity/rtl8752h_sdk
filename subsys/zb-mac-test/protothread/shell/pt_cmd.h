/**************************************************************************//**
 * @file      pt_cmd.h
 * @brief     Header file for pt_cmd definition
 * @author    felix
 * @version   V1.00
 * @date      2025-01-17
 *
 * @note
 *
 ******************************************************************************
 *
 * Copyright (c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************/

#ifndef _PT_CMD_H_
#define _PT_CMD_H_

/***************************************/
void shell_register_cmd_sys(void);
int cmd_reset(int argc, char *argv[]);
int cmd_time(int argc, char *argv[]);
int cmd_dbgport(int argc, char *argv[]);
extern int g_dbg_port;
extern int g_rf_dbg_port;
/***************************************/
extern bool ptmac_dump_rx;
void shell_register_cmd_mac(void);
int cmd_mac_init(int argc, char *argv[]);
int cmd_mac_enable(int argc, char *argv[]);
int cmd_mac_disable(int argc, char *argv[]);
int cmd_radio(int argc, char *argv[]);
/***************************************/
void shell_register_cmd_ble(void);
int cmd_ble_adv(uint32_t argc, uint8_t  *argv[]);
/***************************************/
void shell_register_cmd_netconf(void);
int cmd_panid(int argc, char *argv[]);
int cmd_channel(int argc, char *argv[]);
int cmd_freq(int argc, char *argv[]);
int cmd_shortaddr(int argc, char *argv[]);
int cmd_extaddr(int argc, char *argv[]);
int cmd_mackey(int argc, char *argv[]);
/***************************************/
void shell_register_cmd_netstat(void);
int cmd_ifconfig(int argc, char *argv[]);
int cmd_retrycnt(int argc, char *argv[]);
int cmd_pendack(int argc, char *argv[]);
/***************************************/
void shell_register_cmd_txconf(void);
int cmd_txpower(int argc, char *argv[]);
int cmd_txretry(int argc, char *argv[]);
int cmd_ccamode(int argc, char *argv[]);
int cmd_csma(int argc, char *argv[]);
int cmd_ccacnt(int argc, char *argv[]);
int cmd_txgain(int argc, char *argv[]);
int cmd_notxcrc(int argc, char *argv[]);
int cmd_enh_ack_early(int argc, char *argv[]);
int cmd_tx_interval(int argc, char *argv[]);
int cmd_preampt(int argc, char *argv[]);
int cmd_phyprio(int argc, char *argv[]);
int cmd_gntmode(int argc, char *argv[]);
extern uint32_t g_phy_arbitration_method;
/***************************************/
void shell_register_cmd_rxconf(void);
int cmd_promiscuous(int argc, char *argv[]);
int cmd_recverrpkt(int argc, char *argv[]);
int cmd_scanmode(uint32_t argc, uint8_t  *argv[]);
int cmd_rxframetype(int argc, char *argv[]);
int cmd_test_enh_ack_late(int argc, char *argv[]);
/***************************************/
void shell_register_cmd_srcmatch(void);
int cmd_srcmatchmode(int argc, char *argv[]);
int cmd_srcmatchfilter(int argc, char *argv[]);
/***************************************/
#include "net/pt_mac.h" // definition of pt_mac_callback_t 
typedef struct
{
    uint32_t cnt;
    uint32_t interval;
    uint32_t defer_time;
    uint64_t daddr;
    uint8_t data_len;
    uint8_t pan_idx : 2;
    uint8_t sec_level : 3;
    uint8_t key_id_mode : 2;
    uint8_t random : 1;
    uint8_t frm_type : 3;
    uint8_t frm_ver : 1; // 0: 2006, 1: 2015
    uint8_t silent : 2;
    uint8_t rsvd1 : 2;
    uint8_t cmd_id;
    uint8_t *user_raw; // user defined raw data
    uint8_t raw_len;
    pt_mac_callback_t user_cb;
} pktgen_conf_t;
void shell_register_cmd_pktgen(void);
int pktgen(pktgen_conf_t *conf);
/***************************************/
void shell_register_cmd_pktrate(void);
/***************************************/
void shell_register_cmd_example(void);
/***************************************/
void shell_register_cmd_ping(void);
PROCESS_NAME(ping_reply_process);
/***************************************/
void shell_register_cmd_sec(void);
/***************************************/
void shell_register_cmd_sniffer(void);
/***************************************/
#endif /* _PT_CMD_H_ */
