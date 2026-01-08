/**************************************************************************//**
 * @file     mac_csl.h
 * @brief    Include file for proprietary Coordinated Sample Listening implementation.
 * @author   felix
 * @version  V1.00
 * @date     2025-01-17
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


#ifndef _MAC_CSL_H_
#define _MAC_CSL_H_

#define PROTOCOL_NONE 0
#define PROTOCOL_154  1
#define PROTOCOL_24g  2

#define CSL_ROLE_COORD      0x0
#define CSL_ROLE_ENDPOINT   0x1
#define CSL_ROLE_INVALID    0x2

#if TEST_GPIO_DBG_EN
void trigger_gpio_at_anchor(bool reverse);
#else
#define trigger_gpio_at_anchor(reverse)
#endif

struct protocol_fn
{
    /* Mandatory */
    uint8_t id; // PROTOCOL_154 or PROTOCOL_24g
    uint32_t (*get_curr_timestamp)(void);
    int (*send)(uint8_t *data, uint8_t data_len, uint32_t *tx_timestamp);
    void (*pm_init)(void *enter_callback, void *exit_callback);
    int (*pm_set)(uint32_t time);
    void (*pm_init_wakeup)(void);

    /* Mandatory for coordinator, Optional for endpoint */
    void *(*timer_init)(void);
    void (*timer_stop)(void *timer);
    void (*timer_start)(void *timer, uint32_t timeout, void *callback, void *arg);

    /* Optional */
    void (*config_init)(void);
    void (*start_periodic_tx_sched_cb)(void);
    void (*stop_periodic_tx_sched_cb)(void);
    void (*pm_exit_cb)(void);
};

extern struct protocol_fn PROTOCOL;
extern uint16_t g_csl_peer_addr;
extern int g_csl_ppm;
extern uint32_t g_csl_prepare_to_rx;
extern uint8_t g_csl_role;
extern int g_csl_tx_check_offest;
extern uint32_t g_csl_ack_require_time;
extern uint32_t g_csl_listen_window;
void csl_input(uint8_t *data, uint8_t len, uint32_t rx_timestamp, uint16_t saddr);
uint32_t load_cfg_shortaddr(uint16_t *addr);
void p154_init(void);
#endif /*_MAC_CSL_H_*/
