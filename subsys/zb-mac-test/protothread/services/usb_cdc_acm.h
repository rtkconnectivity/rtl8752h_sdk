/**************************************************************************//**
 * @file      usb_cdc_acm.h
 * @brief     Header file for usb cdc acm
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

#ifndef _USB_CDC_ACM_H_
#define _USB_CDC_ACM_H_

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * RX callback type
 * ============================================================ */
/**
 * @brief USB CDC-ACM receive callback
 *
 * @note
 * - The data buffer is only valid during the callback execution.
 * - The driver does NOT retain or copy RX data.
 * - Applications must copy the data if persistence is required.
 *
 * @param[in] data      Pointer to received data buffer
 * @param[in] len       Number of bytes received
 * @param[in] user_ctx  User-defined context pointer
 */
typedef void (*usb_cdc_acm_rx_cb_t)(
    const uint8_t *data,
    uint16_t       len,
    void          *user_ctx
);

/* ============================================================
 * Configuration
 * ============================================================ */
/**
 * @brief USB CDC-ACM configuration structure
 *
 * This structure is provided by the application when initializing
 * the CDC-ACM driver.
 */
typedef struct
{
    /**
     * @brief RX callback (mandatory)
     *
     * This callback is invoked whenever data is received from the
     * USB host over the CDC-ACM bulk OUT endpoint.
     */
    usb_cdc_acm_rx_cb_t rx_cb;

    /**
     * @brief USB manufacturer string (optional)
     *
     * Set to NULL to use the default string.
     */
    const char *manufacturer;

    /**
     * @brief USB product string (optional)
     *
     * Set to NULL to use the default string.
     */
    const char *product;

    /**
     * @brief USB serial number string (optional)
     *
     * Set to NULL to use the default string.
     */
    const char *serial;

    /**
     * @brief User context passed to RX callback (optional)
     */
    void *rx_cb_ctx;

} usb_cdc_acm_config_t;

#if TEST_USB_CDC_ACM
/* ============================================================
 * Lifecycle control
 * ============================================================ */
/**
 * @brief Initialize the USB CDC-ACM device
 *
 * This function:
 *  - Initializes USB descriptors
 *  - Registers CDC class instances
 *  - Prepares internal driver state
 *
 * @param[in] cfg  Pointer to CDC-ACM configuration structure
 *
 * @retval true   Initialization successful
 * @retval false  Invalid parameters or already initialized
 */
bool usb_cdc_acm_init(const usb_cdc_acm_config_t *cfg);

/**
 * @brief Start the USB device and attach to the USB bus
 *
 * @retval true   Device started successfully
 * @retval false  Driver not initialized
 */
bool usb_cdc_acm_start(void);

/**
 * @brief Stop the USB device and detach from the USB bus
 *
 * @retval true   Device stopped successfully
 * @retval false  Driver not initialized
 */
bool usb_cdc_acm_stop(void);

/* ============================================================
 * Status
 * ============================================================ */
/**
 * @brief Check whether the CDC-ACM device is ready for data transfer
 *
 * @retval true   Device is enumerated and data pipes are ready
 * @retval false  Device not ready
 */
bool usb_cdc_acm_is_ready(void);

/* ============================================================
 * Data transfer
 * ============================================================ */
/**
 * @brief Send data over USB CDC-ACM (blocking)
 *
 * @note
 * - This function blocks until all data has been transmitted.
 * - The caller may safely pass a stack-allocated buffer.
 * - Data is transmitted in USB bulk packets internally.
 *
 * @param[in] data  Pointer to data buffer to send
 * @param[in] len   Number of bytes to send
 *
 * @retval true   Transmission successful
 * @retval false  Device not ready or invalid parameters
 */
bool usb_cdc_acm_send(const uint8_t *data, uint16_t len);
#else
static inline bool usb_cdc_acm_init(const usb_cdc_acm_config_t *cfg) { (void)cfg; return false; }
static inline bool usb_cdc_acm_start(void) { return false; }
static inline bool usb_cdc_acm_stop(void) { return false; }
static inline bool usb_cdc_acm_is_ready(void) { return false; }
static inline bool usb_cdc_acm_send(const uint8_t *d, uint16_t l) { (void)d; (void)l; return false; }
#endif /*TEST_USB_CDC_ACM*/

#ifdef __cplusplus
}
#endif

#endif /* _USB_CDC_ACM_H_ */
