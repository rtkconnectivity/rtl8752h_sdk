/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      tp_test_client_app.h
   * @brief     This file handles Throughput Client application routines.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

#ifndef _TP_TEST_CLIENT_APP_H_
#define _TP_TEST_CLIENT_APP_H_

#ifdef __cplusplus
extern "C" {
#endif
/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <profile_client.h>
#include <app_msg.h>

/*============================================================================*
 *                              Variables
 *============================================================================*/
extern T_CLIENT_ID   gaps_client_id;        /**< Simple ble service client id*/
extern T_CLIENT_ID   tp_client_id;   /**< throughput test client id*/

typedef struct
{
    uint8_t opcode;
    uint16_t length;
    uint16_t time;
    uint16_t con_interval_min;
    uint16_t con_interval_max;
    uint8_t flag;

    uint32_t tx_rate;
    uint32_t rx_rate;
} T_TP_TEST_PARAM;

/*============================================================================*
 *                              Functions
 *============================================================================*/

/**
 * @brief    All the application messages are pre-handled in this function
 * @note     All the IO MSGs are sent to this function, then the event handling
 *           function shall be called according to the MSG type.
 * @param[in] io_msg  IO message data
 * @return   void
 */
void app_handle_io_msg(T_IO_MSG io_msg);
/**
  * @brief Callback for gap le to notify app
  * @param[in] cb_type callback msy type @ref GAP_LE_MSG_Types.
  * @param[in] p_cb_data point to callback data @ref T_LE_CB_DATA.
  * @retval result @ref T_APP_RESULT
  */
T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data);

/**
 * @brief  Callback will be called when data sent from profile client layer.
 * @param  client_id the ID distinguish which module sent the data.
 * @param  conn_id connection ID.
 * @param  p_data  pointer to data.
 * @retval   result @ref T_APP_RESULT
 */
T_APP_RESULT app_client_callback(T_CLIENT_ID client_id, uint8_t conn_id, void *p_data);

/**
 * @brief  Config test params.
 * @param  direction Test direction, @ref T_TP_TEST_DIR.
 * @param  con_interval_us Connection interval, uint: us.
 * @param  length Data length.
 * @param  phy A bit field that indicates PHYs that the Host prefers the Controller to use.
 *             @arg 1: 1M.
*              @arg 2: 2M.
 * @param  time Test time, uint: s.
 * @return void.
 */
void tp_set_test_param(uint8_t direction, uint32_t con_interval_us, uint16_t length, uint8_t phy,
                       uint16_t time);

#ifdef __cplusplus
}
#endif

#endif

