/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      gap_scan.h
* @brief     Header file for GAP Observer role
* @details
* @author    Jane
* @date      2016-02-18
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_SCAN_H
#define GAP_SCAN_H

#ifdef __cplusplus
extern "C"
{
#endif


/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gap_le.h>


/** @defgroup Observer_Role GAP Observer Role
  * @brief GAP observer role
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup Observer_Exported_Types GAP Observer Exported Types
  * @{
  */

/** @brief LE scan parameter type */
typedef enum
{
    GAP_PARAM_SCAN_LOCAL_ADDR_TYPE   = 0x240,  /**< The type of address being used in the scan request packets. Read/Write. Size is 1 octet.
                                                    Value is @ref T_GAP_LOCAL_ADDR_TYPE. Default value is @ref GAP_LOCAL_ADDR_LE_PUBLIC. */
    GAP_PARAM_SCAN_MODE              = 0x241,  /**< The type of scan to perform. Read/Write. Size is 1 octet.
                                                    Value is @ref T_GAP_SCAN_MODE. Default value is @ref GAP_SCAN_MODE_ACTIVE. */
    GAP_PARAM_SCAN_INTERVAL          = 0x242,  /**< The time interval from when the Controller started its last LE scan until it begins the subsequent LE scan.
                                                    Read/Write. Size is 2 octets. Value range: 0x0004 - 0x4000 (2.5 ms - 10240 ms, 0.625 ms/step).
                                                    Default value is 0x10 (10 ms). Please refer to @ref GAP_PARAM_SCAN_WINDOW for details on usage. */
    GAP_PARAM_SCAN_WINDOW            = 0x243,  /**< The duration of the LE scan. Read/Write. Size is 2 octets.
                                                    Value range: 0x0004 - 0x4000 (2.5 ms - 10240 ms, 0.625 ms/step).
                                                    Default value is 0x10 (10 ms).

                                                    Usage:
                                                    - LE_Scan_Window shall be less than or equal to LE_Scan_Interval @ref GAP_PARAM_SCAN_INTERVAL.
                                                    */
    GAP_PARAM_SCAN_FILTER_POLICY     = 0x244,  /**< Scan Filter Policy. Read/Write. Size is 1 octet. Value is @ref T_GAP_SCAN_FILTER_POLICY.
                                                    Default value is @ref GAP_SCAN_FILTER_ANY. */
    GAP_PARAM_SCAN_FILTER_DUPLICATES = 0x245   /**< Whether to filter out duplicate advertising reports. Read/Write. Size is 1 octet.
                                                    Value is @ref T_GAP_SCAN_FILTER_DUPLICATE. Default value is @ref GAP_SCAN_FILTER_DUPLICATE_DISABLE. */
} T_LE_SCAN_PARAM_TYPE;


/** End of Observer_Exported_Types
  * @}
  */


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup Observer_Exported_Functions GAP Observer Exported Functions
  * @{
  */

/**
 * @brief       Set a scan parameter.
 *
 * This function can be called with a scan parameter type @ref T_LE_SCAN_PARAM_TYPE and it will set the scan parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
 *
 * @param[in]   param Scan parameter type @ref T_LE_SCAN_PARAM_TYPE.
 * @param[in]   len  Length of data to write.
 * @param[in]   p_value  Pointer to data to write.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others   Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t scan_mode = GAP_SCAN_MODE_ACTIVE;
        uint16_t scan_interval = DEFAULT_SCAN_INTERVAL;
        uint16_t scan_window = DEFAULT_SCAN_WINDOW;
        uint8_t scan_filter_policy = GAP_SCAN_FILTER_ANY;
        uint8_t scan_filter_duplicate = GAP_SCAN_FILTER_DUPLICATE_ENABLE;

        le_scan_set_param(GAP_PARAM_SCAN_MODE, sizeof(scan_mode), &scan_mode);
        le_scan_set_param(GAP_PARAM_SCAN_INTERVAL, sizeof(scan_interval), &scan_interval);
        le_scan_set_param(GAP_PARAM_SCAN_WINDOW, sizeof(scan_window), &scan_window);
        le_scan_set_param(GAP_PARAM_SCAN_FILTER_POLICY, sizeof(scan_filter_policy),
                         &scan_filter_policy);
        le_scan_set_param(GAP_PARAM_SCAN_FILTER_DUPLICATES, sizeof(scan_filter_duplicate),
                         &scan_filter_duplicate);
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_set_param(T_LE_SCAN_PARAM_TYPE param, uint8_t len, void *p_value);

/**
 * @brief       Get a scan parameter.
 *
 * This function can be called with a scan parameter type @ref T_LE_SCAN_PARAM_TYPE and it will get the scan parameter.
 * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
 * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
 *
 * @param[in]   param   Scan parameter type @ref T_LE_SCAN_PARAM_TYPE.
 * @param[in,out]  p_value Pointer to location to get the parameter value.
 *
 * @return Operation result.
 * @retval GAP_CAUSE_SUCCESS    Operation success.
 * @retval Others   Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint16_t scan_interval;
        le_scan_get_param(GAP_PARAM_SCAN_INTERVAL, &scan_interval);
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_get_param(T_LE_SCAN_PARAM_TYPE param, void *p_value);

/**
 * @brief   Start scan.
 *
 * If sending request operation is successful, the starting result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_DEV_STATE_CHANGE with new_state about gap_scan_state @ref GAP_SCAN_STATE.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG.
 *
 * If the device is in the scanning state @ref GAP_SCAN_STATE_SCANNING, the advertising data or scan response data will be returned by
 * the callback registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_SCAN_INFO.
 *
 * Applications can only call this API after Bluetooth Host is ready.
 *
 * Explanation: If Bluetooth Host is ready, the application will be notified by message @ref GAP_MSG_LE_DEV_STATE_CHANGE
 * with new_state about gap_init_state which is configured as @ref GAP_INIT_STATE_STACK_READY.
 *
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        le_scan_start();
    }

    void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
    {
        APP_PRINT_INFO5("app_handle_dev_state_evt: init state %d scan state %d adv state %d conn state %d cause 0x%x",
                       new_state.gap_init_state,
                       new_state.gap_scan_state, new_state.gap_adv_state, new_state.gap_conn_state, cause);

        if (gap_dev_state.gap_scan_state != new_state.gap_scan_state)
        {
            if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
            {
                APP_PRINT_INFO0("GAP scan stop");
                data_uart_print("GAP scan stop\r\n");
            }
            else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
            {
                APP_PRINT_INFO0("GAP scan start");
                data_uart_print("GAP scan start\r\n");
            }
        }
    }
    //Received advertising or scan rsp data will be handled in app_gap_callback
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        case GAP_MSG_LE_SCAN_INFO:
            APP_PRINT_INFO5("GAP_MSG_LE_SCAN_INFO:adv_type 0x%x, bd_addr %s, remote_addr_type %d, RSSI %d, data_len %d",
                           cb_data.p_le_scan_info->adv_type,
                           TRACE_BDADDR(cb_data.p_le_scan_info->bd_addr),
                           cb_data.p_le_scan_info->remote_addr_type,
                           cb_data.p_le_scan_info->rssi,
                           cb_data.p_le_scan_info->data_len);
            break;
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_start(void);

/**
  * @brief   Stop scan.
  *
  * If sending request operation is successful, the stopping result will be returned in one of the following ways:
  * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
  *   by message @ref GAP_MSG_LE_DEV_STATE_CHANGE with new_state about gap_scan_state @ref GAP_SCAN_STATE.
  * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
  *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG.
  *
  * Applications can only call this API after Bluetooth Host is ready.
  *
  * Explanation: If Bluetooth Host is ready, the application will be notified by message @ref GAP_MSG_LE_DEV_STATE_CHANGE
  * with new_state about gap_init_state which is configured as @ref GAP_INIT_STATE_STACK_READY.
  *
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        le_scan_stop();
    }
    void app_handle_dev_state_evt(T_GAP_DEV_STATE new_state, uint16_t cause)
    {
        APP_PRINT_INFO5("app_handle_dev_state_evt: init state %d scan state %d adv state %d conn state %d cause 0x%x",
                       new_state.gap_init_state,
                       new_state.gap_scan_state, new_state.gap_adv_state, new_state.gap_conn_state, cause);

        if (gap_dev_state.gap_scan_state != new_state.gap_scan_state)
        {
            if (new_state.gap_scan_state == GAP_SCAN_STATE_IDLE)
            {
                APP_PRINT_INFO0("GAP scan stop");
                data_uart_print("GAP scan stop\r\n");
            }
            else if (new_state.gap_scan_state == GAP_SCAN_STATE_SCANNING)
            {
                APP_PRINT_INFO0("GAP scan start");
                data_uart_print("GAP scan start\r\n");
            }
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_scan_stop(void);

/**
  * @brief   Set scan information filter.
  *
  * This function can be called before @ref gap_start_bt_stack is invoked.
  *
  * @param[in]  enable   Whether to open the scan info comparison function.
  * @param[in]  offset   The start offset of the scan info to compare.
  * @param[in]  len      Length of data to compare.
  * @param[in]  p_filter Pointer the data to compare with the scan info.
  * @return  Operation result.
  * @retval  true  Operation success.
  * @retval  false Operation Failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t type;
        uint8_t offset = 0;
        uint8_t len = 0;
        uint8_t filter_data[31];
        if(type == 0)
        {
            le_scan_info_filter(false, offset, len, filter_data);
        }
        else
        {
            offset = 5;
            len = 2;
            filter_data[0] = LO_WORD(GATT_UUID_SIMPLE_PROFILE);
            filter_data[1] = HI_WORD(GATT_UUID_SIMPLE_PROFILE);
            le_scan_info_filter(true, offset, len, filter_data);
        }
    }
  * \endcode
  */
bool le_scan_info_filter(bool enable, uint8_t offset, uint8_t len, uint8_t *p_filter);

/** @} */ /* End of group Observer_Exported_Functions */
/** @} */ /* End of group Observer_Role */


#ifdef __cplusplus
}
#endif

#endif /* GAP_SCAN_H */
