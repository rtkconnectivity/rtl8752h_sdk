/**
*****************************************************************************************
*     Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.

*
*     SPDX-License-Identifier: LicenseRef-Realtek-5-Clause
*****************************************************************************************
  * @file     ppt_simple.h
  * @brief    Head file for simple wrapper of 2.4G module common driver.
  * @details  data structs and external functions declaration.
  * @author   Bill
  * @date     2025-12-1
  * @version  v1.0
  ***************************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2025 Realtek Semiconductor Corporation</center></h2>
  ***************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _PPT_SIMPLE_H_
#define _PPT_SIMPLE_H_

/* Add Includes here */
#include <stdbool.h>
#include <stdint.h>
#include "ppt_driver.h"

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

/** @addtogroup PPT_Simple
  * @{
  */

/** @defgroup PPT_Simple_Exported_Macros Exported Macros
  * @brief
  * @{
  */

/* payload length */
#ifndef PPT_TX_BUFFER_NUM
#define PPT_TX_BUFFER_NUM           2
#endif
#define PPT_TX_BUFFER_SIZE          255
/* header + payload length */
#define PPT_RX_BUFFER_SIZE          (PPT_TX_BUFFER_SIZE + 8)

/** @} End of PPT_Simple_Exported_Macros */

/** @defgroup PPT_Simple_Exported_Types Exported Types
  * @brief
  * @{
  */

typedef struct
{
    ppt_ptx_mode_t base;
    uint16_t retransmit_times;
} ppt_ptx_mode_ext_t;

typedef struct
{
    ppt_prx_mode_t base;
} ppt_prx_mode_ext_t;

typedef struct
{
    ppt_psd_mode_t base;
} ppt_psd_mode_ext_t;

typedef struct
{
    volatile ppt_fsm_t fsm;
    bool ack;
    bool oneshot;
    uint16_t retransmit_times;
    volatile uint16_t retransmit_counter;
    uint8_t *tx_buffer[PPT_TX_BUFFER_NUM];
    uint8_t *rx_buffer;
    volatile bool sync_flag;
    void (*async_cb)(void);
    struct
    {
        ppt_psd_mode_t psd_mode;
        int16_t psd_result[PSD_CHANN_NUM];
        uint16_t psd_tmp_flag;
    };
} ppt_ctx_t;

extern ppt_ctx_t *ppt_ctx;
/** @} End of PPT_Simple_Exported_Types */

/** @defgroup PPT_Simple_Exported_Functions Exported Functions
  * @brief
  * @{
  */

/**
  * @brief Initialize the radio
  */
void ppt_init(void);

/**
  * @brief Deinitialize the radio
  */
void ppt_deinit(void);

/**
  * @brief Set the ptx modes
  * @param[in] param: parameters
  */
void ppt_set_ptx_mode_ext(ppt_ptx_mode_ext_t *param);

/**
  * @brief Set the prx modes
  * @param[in] param: parameters
  */
void ppt_set_prx_mode_ext(ppt_prx_mode_ext_t *param);

/**
  * @brief Set the psd modes
  * @param[in] param: parameters
  * @return parameters set result
  */
bool ppt_set_psd_mode_ext(ppt_psd_mode_ext_t *param);

/**
  * @brief Get tx buffer used by DMA for sending data
  *
  * The RAM type used by DMA is limited to a specific RAM type.
  * This driver manages the memory for easy to use at the application.
  * @param[in] payload_len: length of payload
  * @return buffer address
  */
uint8_t *ppt_get_tx_buffer(uint16_t payload_len);

/**
  * @brief Get tx buffer used by DMA for sending data
  *
  * The RAM type used by DMA is limited to a specific RAM type.
  * This driver manages the memory for easy to use at the application.
  * @param[in] entry: channel index
  * @param[in] payload_len: length of payload
  * @return buffer address
  */
uint8_t *ppt_get_tx_buffer_by_entry(uint8_t entry, uint16_t payload_len);

/**
  * @brief Get rx buffer used by DMA for receiving data
  *
  * The RAM type used by DMA is limited to a specific RAM type.
  * This driver manages the memory for easy to use at the application.
  * @param[in] pdu_len: length of PDU
  * @return buffer address
  */
uint8_t *ppt_get_rx_buffer(uint16_t pdu_len);

/**
  * @brief Copy and push the data to the hw fifo
  *
  * The data will be copied, so the RAM type of data can be any type.
  * @param[in] len: length of data
  * @param[in] data: data to send
  */
void ppt_push_tx_data(uint16_t len, uint8_t *data);

/**
  * @brief Copy and update the data to the hw fifo
  *
  * The last pushed data can be updated by this function.
  * @param[in] len: length of data
  * @param[in] data: data to send
  */
void ppt_update_tx_data(uint16_t len, uint8_t *data);

/**
  * @brief Copy and push the data to the hw fifo of the specific channel
  *
  * The data will be copied, so the RAM type of data can be any type.
  * @param[in] entry: channel index
  * @param[in] len: length of data
  * @param[in] data: data to send
  */
void ppt_push_tx_data_by_entry(uint8_t entry, uint16_t len, uint8_t *data);

/**
  * @brief Pop and copy the data from the hw fifo
  *
  * The data will be copied to the buffer RAM, its pointer will be returned.
  * @param[in] len: length of data
  * @return received data pointer
  */
uint8_t *ppt_pop_rx_data(uint16_t len);

/**
  * @brief Pop and copy the data from the hw fifo of the specific channel
  *
  * The data will be copied to the buffer RAM, its pointer will be returned.
  * @param[in] entry: channel index
  * @param[in] len: length of data
  * @return received data pointer
  */
uint8_t *ppt_pop_rx_data_by_entry(uint8_t entry, uint16_t len);

/**
  * @brief Async callback template
  */
void ppt_async_cb_template(void);

/**
  * @brief Enable ptx state
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  * @param[in] async_cb: the callback function pointer
  */
void ppt_enable_ptx(void (*async_cb)(void));

/**
  * @brief Disable ptx state
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  * @param[in] async_cb: the callback function pointer
  */
void ppt_disable_ptx(void (*async_cb)(void));

/**
  * @brief Enable prx state
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  * @param[in] async_cb: the callback function pointer
  */
void ppt_enable_prx(void (*async_cb)(void));

/**
  * @brief Disable prx state
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  * @param[in] async_cb: the callback function pointer
  */
void ppt_disable_prx(void (*async_cb)(void));

/**
  * @brief Reset the finite state machine of the hardware module
  *
  * The reset feature shall be enabled previously by fw_rst_enable flag in @ref PRO_RESET_CTRL_REG.
  * This API will generate mac interrupt, so it shall be called in critical section if at lower priority.
  * All the internal states will be reset except for the interface registers in ppt_hw_reg.h.
  */
void ppt_reset_hw_fsm(void);

/**
  * @brief Kill the finite state machine of the hardware module
  *
  * This API will generate mac interrupt, so it shall be called in critical section if at lower priority.
  * @return the result
  */
bool ppt_kill_hw_fsm(ppt_fsm_t fsm);

/**
  * @brief Enable psd procedure
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  *
  * @param[in] async_cb: the callback function pointer
  *
  * <b>Example usage</b>
  * \code{.c}
    ppt_psd_mode_ext_t param =
    {
        {
            .chann_start = 0,
            .chann_stop = 0,
            .chann_step = 1,
            .mode = 0,
            .timeout = PSD_TIMEOUT_DEFAULT
        }
    };
    ppt_set_psd_mode_ext(&param);
    ppt_enable_psd(NULL);
    int16_t rssi = ppt_get_psd_result(0);
  * \endcode
  */
void ppt_enable_psd(void (*async_cb)(void));

/**
  * @brief Disable psd procedure
  *
  * If the callback is null, then the api returns after the procedure is done in a sync way.
  * Otherwise, the caller will be notified via the callback when the procedure is done in an async way.
  * @param[in] async_cb: the callback function pointer
  */
void ppt_disable_psd(void (*async_cb)(void));

/**
  * @brief Get the stored psd result
  * @param[in] chann: the channel index, shall be less than @ref PSD_CHANN_NUM
  * @return dbm value
  */
int16_t ppt_get_psd_result(uint8_t chann);

/**
  * @brief Clear the stored psd result
  */
void ppt_clear_psd_result(void);

/** @} End of PPT_Simple_Exported_Functions */

/** @} End of PPT_Simple */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
