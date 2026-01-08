/**************************************************************************//**
 * @file     platform_port.h
 * @brief    Porting layer of platform function and wrapper function of mac driver
 * @author   felix.yu
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

#ifndef _PLATFORM_PORT_H_
#define _PLATFORM_PORT_H_
#if defined (__cplusplus)
extern "C" {
#endif

#include "mac_test_config.h"

// Including platform-specific headers
#include "rtl876x_pinmux.h"                 // Pin mux settings
#if (TEST_GPIO_DBG_EN == 1)
#include "rtl876x_gpio.h"                   // GPIO management
#endif
#include "rtl876x_rcc.h"                    // RCC (Reset and Clock Control) setup
#include "rtl876x_tim.h"                    // Timer configurations
#include "rtl876x_wdg.h"                    // Watchdog timer management
#include "rtl876x_nvic.h"                   // Nested Vector Interrupt Controller
#include "rtl876x_lib_platform.h"           // Platform-specific auxiliary functions
#include "rtl876x_hw_sha256.h"              // SHA256 hardware support
#include "rtl876x_uart.h"

#define TASK_SCHEDULE_OVERHEAD 175          // Scheduling overhead time

// Definitions for Debugging GPIO
#define GPIO_OUTPUT_PIN_0   P0_1 // LED 0
#define GPIO_OUTPUT_PIN_1   P0_2 // LED 1

// Debugging UART configuration
#define ZB_DBG_UART                         UART2
#define ZB_DBG_UART_TX                      UART2_TX
#define ZB_DBG_UART_RX                      UART2_RX
#define ZB_DBG_UART_IRQn                    UART2_IRQn
#define ZB_DBG_UART_VECTORn                 UART2_VECTORn

// CLI UART configuration
#define ZB_CLI_UART                         UART3
#define ZB_CLI_UART_TX                      UART3_TX
#define ZB_CLI_UART_RX                      UART3_RX
#define ZB_CLI_UART_IRQn                    UART3_IRQn
#define ZB_CLI_UART_VECTORn                 UART3_VECTORn

#define ZB_TIM                              TIM2 // Timer configuration

// UART PIN Mux definitions based on BOARD types
#ifdef BOARD_DONGLE // For Dongle board
#define ZB_DBG_UART_TX_PIN                  P2_3
#define ZB_DBG_UART_RX_PIN                  P2_2
#else // For Evaluation Board (EVB)
#define ZB_DBG_UART_TX_PIN                  P3_0
#define ZB_DBG_UART_RX_PIN                  P3_1
#endif
#define ZB_CLI_UART_TX_PIN                  P2_4
#define ZB_CLI_UART_RX_PIN                  P2_5

// Pins for reset and interrupts
#define ZB_RESET_PIN                        P0_5
#define ZB_INTERRUPT_PIN                    P0_4

// Task priorities and stack sizes
#define ZB_TASK_PRIORITY                    4
#define ZB_TASK_STACK_SIZE                  (1024 * 4)
#define PT_TASK_PRIORITY                    4
#define PT_TASK_STACK_SIZE                  (1024 * 4)

// System reset using watchdog timer
#define WDG_SystemReset() WDG_SystemReset(RESET_ALL, SW_RESET_APP_END)

// MAC driver interface section
#include "mac_driver_interface.h"

// Function prototype for MAC driver settings
extern void modem_set_zb_cca_combination_rom(uint8_t comb);
#define set_zb_cca_combination modem_set_zb_cca_combination_rom

// Define a standard alias for the platform-specific priority setting function
extern void set_zigbee_priority(uint16_t priority, uint16_t priority_min);
#define set_zb_priority set_zigbee_priority

// Initialize the PHY arbitration in the MAC layer
static inline void mac_InitPHYArbitration(mac_attribute_t *p_attr,
                                          mac_phy_arbitration_method method)
{
    p_attr->phy_arbitration_en = MAC_PHY_ARBI_ANCH; // Enable arbitration
    extern uint32_t (*lowerstack_SystemCall)(uint32_t opcode, uint32_t param, uint32_t param1,
                                             uint32_t param2);
    lowerstack_SystemCall(10, 1, 512, -1); // Make a system call with specific parameters
}


extern void stdio_putc_wrap(void *adapter, const char data);
extern void zb_periheral_drv_init(void);
extern void ZB_DBG_UART_Handler(void);
extern void zb_pin_mux_init(void);
extern void _out_char(char character, void *buffer, size_t idx, size_t maxlen);

#if defined (__cplusplus)
}
#endif
#endif /* _PLATFORM_PORT_H_ */
