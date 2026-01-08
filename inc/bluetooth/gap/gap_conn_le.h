/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    gap_conn_le.h
  * @brief   This file contains all the constants and function prototypes for the GAP protocol.
  * @details
  * @author  jane
  * @date    2016-02-18
  * @version v1.0
  * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef GAP_CONN_LE_H
#define GAP_CONN_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gap.h>
#include <gap_le.h>
#include <gap_msg.h>

/** @defgroup GAP_LE_CONNECTION_MODULE GAP LE Connection Module
  * @brief GAP LE connection module
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup GAP_LE_CONNECTION_Exported_Types GAP LE Connection Exported Types
  * @{
  */

/** @brief GAP connection PHY types*/
typedef enum
{
    GAP_CONN_PARAM_1M = 0,      //!< LE 1M PHY.
    GAP_CONN_PARAM_2M = 1,      //!< LE 2M PHY.
    GAP_CONN_PARAM_CODED = 2,   //!< LE Coded PHY.
} T_GAP_CONN_PARAM_TYPE;

/** @brief GAP link roles */
typedef enum
{
    GAP_LINK_ROLE_UNDEFINED,    //!< Unknown.
    GAP_LINK_ROLE_MASTER,       //!< Role is Central.
    GAP_LINK_ROLE_SLAVE         //!< Role is Peripheral.
} T_GAP_ROLE;

/** @brief LE connection parameter types */
typedef enum
{
    GAP_PARAM_CONN_BD_ADDR         = 0x270,/**< Remote Bluetooth address for the connection. Read only. Size is 6 octets @ref GAP_BD_ADDR_LEN. */
    GAP_PARAM_CONN_BD_ADDR_TYPE    = 0x271,/**< Remote Bluetooth address type for the connection. Read only. Size is 1 octet.
                                                Value is @ref T_GAP_REMOTE_ADDR_TYPE. */
    GAP_PARAM_CONN_INTERVAL        = 0x272,/**< Connection interval. Read only. Size is 2 octets. Time = N * 1.25 ms. */
    GAP_PARAM_CONN_LATENCY         = 0x273,/**< Maximum Peripheral latency for the connection in number of connection events.
                                                Read only. Size is 2 octets. */
    GAP_PARAM_CONN_TIMEOUT         = 0x274,/**< Supervision timeout for the connection. Read only. Size is 2 octets. Time = N * 10 ms. */
    GAP_PARAM_CONN_MTU_SIZE        = 0x275,/**< MTU size for the connection. Read only. Size is 2 octets. */
    GAP_PARAM_CONN_LOCAL_BD_TYPE   = 0x276,/**< Local Bluetooth address type for the connection. Read only. Size is 1 octet. Value is @ref T_GAP_LOCAL_ADDR_TYPE. */
    GAP_PARAM_CONN_RX_PHY_TYPE     = 0x277,/**< Receiver PHY for the connection. Read only. Size is 1 octet. Value is @ref T_GAP_PHYS_TYPE. */
    GAP_PARAM_CONN_TX_PHY_TYPE     = 0x278,/**< Transmitter PHY for the connection. Read only. Size is 1 octet. Value is @ref T_GAP_PHYS_TYPE. */
    GAP_PARAM_CONN_REMOTE_FEATURES = 0x279,/**< Remote supported features for the connection. Read only. Size is @ref GAP_LE_SUPPORTED_FEATURES_LEN. */
    GAP_PARAM_CONN_CHANN_ALGORITHM = 0x27A,//!< LE Channel Selection Algorithm used. Read only. Size is 1 octet.
    GAP_PARAM_CONN_HANDLE          = 0x27B,/**< LE link connection handle. Read only. Size is 2 octets. */
} T_LE_CONN_PARAM_TYPE;

/** @brief  Connected device information.*/
typedef struct
{
    T_GAP_CONN_STATE conn_state;             //!< Connection state.
    T_GAP_ROLE       role;                   //!< GAP link role.
    uint8_t          remote_bd[GAP_BD_ADDR_LEN];  //!< Remote address.
    uint8_t          remote_bd_type;         //!< Remote address type.
} T_GAP_CONN_INFO;

/** @brief  Definition of LE connection request parameter.*/
typedef struct
{
    uint16_t scan_interval;/**< Time interval from when the Controller started its last scan
                                until it begins the subsequent scan on the primary advertising channel.
                                In default situation or @ref le_set_gap_param has been called to set
                                @ref GAP_PARAM_USE_EXTENDED_ADV to false:
                                - Range: 0x0004 to 0x4000.
                                - Time = N * 0.625 ms.
                                - Time Range: 2.5 ms to 10.24 s.

                                If @ref le_set_gap_param has been called to set
                                @ref GAP_PARAM_USE_EXTENDED_ADV to true:
                                - Range: 0x0004 to 0xFFFF.
                                - Time = N * 0.625 ms.
                                - Time Range: 2.5 ms to 40.959375 s. */
    uint16_t scan_window;/**< Duration of the scan on the primary advertising channel.
                              In default situation or @ref le_set_gap_param has been called to set
                              @ref GAP_PARAM_USE_EXTENDED_ADV to false:
                              - Range: 0x0004 to 0x4000.
                              - Time = N * 0.625 ms.
                              - Time Range: 2.5 ms to 10.24 s.

                              If @ref le_set_gap_param has been called to set
                              @ref GAP_PARAM_USE_EXTENDED_ADV to true:
                              - Range: 0x0004 to 0xFFFF.
                              - Time = N * 0.625 ms.
                              - Time Range: 2.5 ms to 40.959375 s. */
    uint16_t conn_interval_min;/**< Minimum value for the connection interval. This shall be less
                                    than or equal to Conn_Interval_Max[i].
                                    - Range: 0x0006 to 0x0C80.
                                    - Time = N * 1.25 ms.
                                    - Time Range: 7.5 ms to 4 s. */
    uint16_t conn_interval_max;/**< Maximum value for the connection interval. This shall be greater
                                    than or equal to Conn_Interval_Max[i].
                                    - Range: 0x0006 to 0x0C80.
                                    - Time = N * 1.25 ms.
                                    - Time Range: 7.5 ms to 4 s. */
    uint16_t conn_latency;/**< Peripheral latency for the connection in number of connection events.
                               - Range: 0x0000 to 0x01F3.
                               - This shall be less than or equal to (((supv_tout * 10 ms) / (conn_interval_max * 1.25 ms * 2)) - 1) */
    uint16_t supv_tout;/**< Supervision timeout for the LE Link.
                            - Range: 0x000A to 0x0C80.
                            - Time = N * 10 ms.
                            - Time Range: 100 ms to 32 s. */
    uint16_t ce_len_min;/**< Informative parameter recommending the minimum length of connection
                             event needed for this LE connection.
                             - Range: 0x0000 - 0xFFFF.
                             - Time = N * 0.625 ms. */
    uint16_t ce_len_max;/**< Informative parameter recommending the maximum length of connection
                             event needed for this LE connection.
                             - Range: 0x0000 - 0xFFFF.
                             - Time = N * 0.625 ms. */
} T_GAP_LE_CONN_REQ_PARAM;

/** End of GAP_LE_CONNECTION_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup GAP_LE_CONNECTION_COMMON_EXPORT_Functions GAP LE Connection Exported Functions
 *
 * @{
 */

/**
  * @brief  Get a GAP connection parameter.
  *
  * This function can be called with a connection parameter type @ref T_LE_CONN_PARAM_TYPE and it will get the connection parameter.
  * The 'p_value' field must point to an appropriate data type that meets the requirements for the corresponding parameter type.
  * (For example, if required data length for parameter type is 2 octets, p_value should be cast to a pointer of uint16_t.)
  *
  * @param[in]  param Connection parameter type @ref T_LE_CONN_PARAM_TYPE
  * @param[in,out] p_value Pointer to location to get the value.
  * @param[in]  conn_id Connection ID.
  *
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS    Operation success.
  * @retval Others   Operation failure.
  */
T_GAP_CAUSE le_get_conn_param(T_LE_CONN_PARAM_TYPE param, void *p_value, uint8_t conn_id);

/**
  * @brief  Get connection information.
  * @param[in]  conn_id Connection ID.
  * @param[in,out] p_conn_info Pointer to connection information @ref T_GAP_CONN_INFO.
  * @return Operation result.
  * @retval true    Operation success.
  * @retval false   Operation failure.
  */
bool        le_get_conn_info(uint8_t conn_id, T_GAP_CONN_INFO *p_conn_info);

/**
  * @brief  Get connection address information.
  * @param[in]  conn_id Connection ID.
  * @param[in,out] bd_addr Pointer to remote Bluetooth device address.
  * @param[in,out] bd_type Pointer to remote Bluetooth device address type.
  * @return Operation result.
  * @retval true    Operation success.
  * @retval false   Operation failure.
  */
bool        le_get_conn_addr(uint8_t conn_id, uint8_t *bd_addr, uint8_t *bd_type);

/**
  * @brief  Get connection ID.
  * @param[in]  bd_addr Pointer to remote Bluetooth device address.
  * @param[in]  bd_type Remote Bluetooth device address type.
  * @param[in,out] p_conn_id Pointer to connection ID.
  * @return Operation result.
  * @retval true    Operation success.
  * @retval false   Operation failure.
  */
bool        le_get_conn_id(uint8_t *bd_addr, uint8_t bd_type, uint8_t *p_conn_id);

/**
  * @brief  Get connection handle.
  * @param[in]  conn_id Connection ID.
  * @return Connection handle.
  * @retval 0xFFFF Operation failure.
  * @retval Others Connection handle.
  */
uint16_t    le_get_conn_handle(uint8_t conn_id);

/**
  * @brief  Get connection ID from connection handle.
  * @param[in]  conn_handle Connection handle.
  * @param[in,out] p_conn_id Pointer to connection ID.
  * @return Operation result.
  * @retval true    Operation success.
  * @retval false   Operation failure.
  */
bool        le_get_conn_id_by_handle(uint16_t conn_handle, uint8_t *p_conn_id);

/**
  * @brief  Get connection local address by connection handle.
  * @param[in]  conn_handle Connection handle.
  * @param[in, out] bd_addr Local bluetooth device address, size is 6 octets.
  * @param[in, out] bd_type Local bluetooth device address type.
  * @return Get result.
  * @retval true Success.
  * @retval false Get failed.
  */
bool le_get_conn_local_addr(uint16_t conn_handle, uint8_t *bd_addr, uint8_t *bd_type);

/**
* @brief   Get the active link number.
*
* @return  Active link number.
* @retval 0 No connection created.
* @retval Others Connected links number.
*/
uint8_t     le_get_active_link_num(void);

/**
* @brief   Get the idle link number.
*
* @return  Idle link number.
* @retval 0 Can't create new connection.
* @retval Others Can create new connection.
*/
uint8_t     le_get_idle_link_num(void);

/**
 * @brief   Terminate connection.
 *
 * If sending request operation is successful, the terminating result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_CONN_STATE_CHANGE with new_state @ref T_GAP_CONN_STATE.
 *   When calling the API, if state of connection identified by conn_id is @ref GAP_CONN_STATE_CONNECTING, and link role is @ref GAP_LINK_ROLE_MASTER,
 *   APP will be notified by message @ref GAP_MSG_LE_DEV_STATE_CHANGE with new_state about gap_conn_state @ref GAP_CONN_STATE.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG about Connection State.
 *   When calling the API, if state of connection identified by conn_id is @ref GAP_CONN_STATE_CONNECTING, and link role is @ref GAP_LINK_ROLE_MASTER,
 *   APP will be notified with the callback registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG
 *   about device state.
 *
 * The disconnection reason is @ref HCI_ERR_REMOTE_USER_TERMINATE.
 *
 * @param[in] conn_id Connection ID to be disconnected.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        le_disconnect(conn_id);
    }

    void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
    {
        APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                        conn_id, gap_conn_state, new_state, disc_cause);
        switch (new_state)
        {
        case GAP_CONN_STATE_DISCONNECTED:
            {
                if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                    && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
                {
                    APP_PRINT_ERROR1("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
                }

                le_adv_start();
            }
            break;

        case GAP_CONN_STATE_CONNECTED:
            {
                uint16_t conn_interval;
                uint16_t conn_latency;
                uint16_t conn_supervision_timeout;
                uint8_t  remote_bd[6];
                T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
                APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                                TRACE_BDADDR(remote_bd), remote_bd_type,
                                conn_interval, conn_latency, conn_supervision_timeout);
            }
            break;

        default:
            break;
        }
        gap_conn_state = new_state;
    }
 * \endcode
 */
T_GAP_CAUSE le_disconnect(uint8_t conn_id);

/**
 * @brief   Terminate connection with reason.
 *
 * If sending request operation is successful, the terminating result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_CONN_STATE_CHANGE with new_state @ref T_GAP_CONN_STATE.
 *   When calling the API, if state of connection identified by conn_id is @ref GAP_CONN_STATE_CONNECTING, and link role is @ref GAP_LINK_ROLE_MASTER,
 *   APP will be notified by message @ref GAP_MSG_LE_DEV_STATE_CHANGE with new_state about gap_conn_state @ref GAP_CONN_STATE.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG about Connection State.
 *   When calling the API, if state of connection identified by conn_id is @ref GAP_CONN_STATE_CONNECTING, and link role is @ref GAP_LINK_ROLE_MASTER,
 *   APP will be notified with the callback registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG
 *   about device state.
 *
 * @param[in] conn_id Connection ID to be disconnected.
 * @param[in] reason Disconnection reason.
 *           - @ref HCI_ERR_REMOTE_USER_TERMINATE.
 *           - @ref HCI_ERR_REMOTE_LOW_RESOURCE.
 *           - @ref HCI_ERR_REMOTE_POWER_OFF.
 *           - @ref HCI_ERR_UNSUPPORTED_REMOTE_FEAT.
 *           - @ref HCI_ERR_UNACCEPTABLE_CONN_INTERVAL.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0x01;
        le_disconnect_with_reason(conn_id, HCI_ERR_REMOTE_USER_TERMINATE);
    }
 * \endcode
 */
T_GAP_CAUSE le_disconnect_with_reason(uint8_t conn_id, uint8_t reason);

/**
 * @brief   Read the values for the version information for the remote device identified by the conn_id parameter.
 *
 * If sending request operation is successful, the reading result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_READ_REMOTE_VERSION.
 *
 * @param[in] conn_id Connection ID.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
   void test(void)
   {
       uint8_t conn_id = 0;
       le_read_remote_version(conn_id);
   }
 * \endcode
 */
T_GAP_CAUSE le_read_remote_version(uint8_t conn_id);

/**
 * @brief   Read RSSI value of the connection.
 *
 * If sending request operation is successful, the reading result will be returned by callback
 * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_READ_RSSI.
 *
 * @param[in] conn_id Connection ID.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        le_read_rssi(conn_id);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_READ_RSSI:
          APP_PRINT_INFO3("GAP_MSG_LE_READ_RSSI: conn_id %d, cause 0x%x, RSSI %d",
                          cb_data.p_le_read_rssi_rsp->conn_id,
                          cb_data.p_le_read_rssi_rsp->cause,
                          cb_data.p_le_read_rssi_rsp->rssi);
          break;
        }
        ...
    }
 * \endcode
 */
T_GAP_CAUSE le_read_rssi(uint8_t conn_id);

/**
  * @brief   Read the used channel map of the connection.
  *
  * If sending request operation is successful, the reading result will be returned by callback
  * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_READ_CHANN_MAP.
  *
  * @param[in] conn_id Connection ID.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        le_read_chann_map(conn_id);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_READ_CHANN_MAP:
           APP_PRINT_INFO7("GAP_MSG_LE_READ_CHANN_MAP: conn_id %d, cause 0x%x, map[0x%x:0x%x:0x%x:0x%x:0x%x]",
                           cb_data.p_le_read_chann_map_rsp->conn_id,
                           cb_data.p_le_read_chann_map_rsp->cause,
                           cb_data.p_le_read_chann_map_rsp->channel_map[0],
                           cb_data.p_le_read_chann_map_rsp->channel_map[1],
                           cb_data.p_le_read_chann_map_rsp->channel_map[2],
                           cb_data.p_le_read_chann_map_rsp->channel_map[3],
                           cb_data.p_le_read_chann_map_rsp->channel_map[4]);
           break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_read_chann_map(uint8_t conn_id);

/**
  * @brief   Set the data length used in Controller for data length extension.
  *
  * If sending request operation is successful, APP will be notified by callback registered
  * by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_SET_DATA_LEN.
  * If cause in @ref T_LE_DISABLE_SLAVE_LATENCY_RSP is 0 (success) and data length is changed,
  * APP will be notified by callback with msg type @ref GAP_MSG_LE_DATA_LEN_CHANGE_INFO.
  *
  * @param[in] conn_id Connection ID.
  * @param[in] tx_octets    Preferred maximum number of payload octets that the local Controller
  *                         should include in a single Link Layer packet on this connection.
                            - Range 0x001B-0x00FB (all other values reserved for future use).
  * @param[in] tx_time      Preferred maximum number of microseconds that the local Controller
                            should use to transmit a single Link Layer packet on this connection.
                            - Range 0x0148-0x4290 (all other values reserved for future use).
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        uint16_t tx_octets = 251;
        uint16_t tx_time = 2120;
        le_set_data_len(conn_id, tx_octets, tx_time);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_SET_DATA_LEN:
           APP_PRINT_INFO2("GAP_MSG_LE_SET_DATA_LEN: conn_id %d, cause 0x%x",
                           cb_data.p_le_set_data_len_rsp->conn_id,
                           cb_data.p_le_set_data_len_rsp->cause);
           break;
        case GAP_MSG_LE_DATA_LEN_CHANGE_INFO:
           APP_PRINT_INFO5("GAP_MSG_LE_DATA_LEN_CHANGE_INFO: conn_id %d, max_tx_octets 0x%x, max_tx_time 0x%x,
           max_rx_octets 0x%x, max_rx_time 0x%x",
                           cb_data.p_le_data_len_change_info->conn_id,
                           cb_data.p_le_data_len_change_info->max_tx_octets,
                           cb_data.p_le_data_len_change_info->max_tx_time,
                           cb_data.p_le_data_len_change_info->max_rx_octets,
                           cb_data.p_le_data_len_change_info->max_rx_time);
           break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_set_data_len(uint8_t conn_id, uint16_t tx_octets, uint16_t tx_time);

/**
  * @brief   Set the PHY preferences for the connection identified by the Connection_Handle.
  *          The Controller might not be able to make the change (e.g. because the peer does
  *          not support the requested PHY) or may decide that the current PHY is preferable.
  *
  * If sending request operation is successful, APP will be notified by callback with msg type @ref GAP_MSG_LE_PHY_UPDATE_INFO.
  *
  * @param[in] conn_id Connection ID.
  * @param[in] all_phys A bit field that allows the Host to specify, for each direction,
  *                     whether it has no preference among the PHYs that the Controller
  *                     supports in a given direction or whether it has specified particular PHYs that it
  *                     prefers in the TX_PHYS or RX_PHYS parameter.
  * @param[in] tx_phys A bit field that indicates the transmitter PHYs that
  *                    the Host prefers the Controller to use. If the ALL_PHYS parameter specifies
  *                    that the Host has no preference, the TX_PHYS parameter is ignored;
  *                    otherwise at least one bit shall be set to 1.
  * @param[in] rx_phys A bit field that indicates the receiver PHYs that the
  *                    Host prefers the Controller to use. If the ALL_PHYS parameter specifies that
  *                    the Host has no preference, the RX_PHYS parameter is ignored; otherwise at
  *                    least one bit shall be set to 1.
  * @param[in] phy_options A bit field that allows the Host to specify options
  *                        for PHYs. Value is @ref T_GAP_PHYS_OPTIONS.
  *                        The default value for a new connection shall be all zero bits. The
  *                        Controller may override any preferred coding for transmitting on the LE Coded
  *                        PHY.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        uint8_t all_phys = GAP_PHYS_NO_PREFER_TX_BIT;
        uint8_t tx_phys = GAP_PHYS_PREFER_CODED_BIT;
        uint8_t rx_phys = GAP_PHYS_PREFER_CODED_BIT;
        uint16_t phy_options = GAP_PHYS_OPTIONS_CODED_PREFER_S8;
        le_set_phy(conn_id, all_phys, tx_phys, rx_phys, phy_options);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_PHY_UPDATE_INFO:
            APP_PRINT_INFO4("GAP_MSG_LE_PHY_UPDATE_INFO:conn %d, cause 0x%x, rx_phy %d, tx_phy %d",
                       cb_data.p_le_phy_update_info->conn_id,
                       cb_data.p_le_phy_update_info->cause,
                       cb_data.p_le_phy_update_info->rx_phy,
                       cb_data.p_le_phy_update_info->tx_phy);
        break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_set_phy(uint8_t conn_id, uint8_t all_phys, uint8_t tx_phys, uint8_t rx_phys,
                       T_GAP_PHYS_OPTIONS phy_options);

/**
  * @brief   Disable the Peripheral latency used in Peripheral role.
  *
  * If sending request operation is successful, the disabling result will be returned by callback
  * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_DISABLE_SLAVE_LATENCY.
  *
  * @note When Peripheral latency is non-zero, and APP disable Peripheral latency, the Peripheral will listen
  * to every connection event.
  *
  * @param[in] conn_id Connection ID.
  * @param[in] disable Disable or enable Peripheral latency.
  *                    - true: Disable Peripheral latency.
  *                    - false: Enable Peripheral latency.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        bool disable = true;
        le_disable_slave_latency(conn_id);
    }

    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_DISABLE_SLAVE_LATENCY:
           APP_PRINT_INFO1("GAP_MSG_LE_DISABLE_SLAVE_LATENCY: cause 0x%x",
                           cb_data.p_le_disable_slave_latency_rsp->cause);
           break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_disable_slave_latency(uint8_t conn_id, bool disable);

/**
  * @brief  Update instant passed channel map.
  *
  * When update instant passed channel map feature is enabled, if device receives an instant passed channel map,
  * the device will not disconnect the link.
  * The feature is enabled by default.
  *
  * If sending request operation is successful, the updating result will be returned by callback
  * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_UPDATE_PASSED_CHANN_MAP.
  *
  * @param[in] enable Enable or disable update instant passed channel map feature.
  *                   - true: Enable update instant passed channel map feature.
  *                   - false: Disable update instant passed channel map feature.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        bool enable = true;
        le_update_passed_chann_map(enable);
    }
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_UPDATE_PASSED_CHANN_MAP:
            APP_PRINT_INFO1("GAP_MSG_LE_UPDATE_PASSED_CHANN_MAP:cause 0x%x",
                       cb_data.p_le_update_passed_chann_map_rsp->cause);
        break;
        }
        ...
    }
  * \endcode
  */
T_GAP_CAUSE le_update_passed_chann_map(bool enable);

/**
  * @brief  Set connection parameter.
  *
  * @param[in] type  Connection parameter type @ref T_GAP_CONN_PARAM_TYPE.
  * @param[in] p_conn_param  Pointer to connection parameters @ref T_GAP_LE_CONN_REQ_PARAM.
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS    Operation success.
  * @retval Others   Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
     void test(void)
     {
         T_GAP_LE_CONN_REQ_PARAM conn_req_param;
         T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
         uint8_t bd_addr[BD_ADDR_SIZE] = {0x00, 0xe0, x04c, 0x23, 0x99, 0x87};
         conn_req_param.scan_interval = 0x10;
         conn_req_param.scan_window = 0x10;
         conn_req_param.conn_interval_min = 80;
         conn_req_param.conn_interval_max = 80;
         conn_req_param.conn_latency = 0;
         conn_req_param.supv_tout = 1000;
         conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
         conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
         le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

         le_connect(0, bd_addr,
                   bd_type,
                   GAP_LOCAL_ADDR_LE_PUBLIC,
                   1000);
     }
  * \endcode
  */
T_GAP_CAUSE le_set_conn_param(T_GAP_CONN_PARAM_TYPE type,
                              T_GAP_LE_CONN_REQ_PARAM *p_conn_param);

/**
 * @brief   Create a connection as Central. @ref le_set_conn_param shall be called before calling this API.
 *
 * Applications can only call this API when gap_conn_state of @ref T_GAP_DEV_STATE is @ref GAP_CONN_DEV_STATE_IDLE.
 *
 * If sending request operation is successful, the creating result will be returned in one of the following ways:
 * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
 *   by message @ref GAP_MSG_LE_CONN_STATE_CHANGE with new_state @ref T_GAP_CONN_STATE.
 *   And APP will be notified by message @ref GAP_MSG_LE_DEV_STATE_CHANGE with new_state about gap_conn_state @ref GAP_CONN_STATE.
 * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
 *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG about Connection State.
 *   And APP will be notified with the callback registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG
 *   about device state.
 *
 * @param[in] init_phys  A bit field that indicates the PHY(s) on which the advertising packets should be received on the primary
 *                       advertising channel and the PHYs for which connection parameters have been specified. @ref GAP_PHYS_CONN_INIT.
 * @param[in] remote_bd Pointer to Peer's Public Device Address, Random (static) Device Address, Non-Resolvable Private Address, or
                        Resolvable Private Address depending on remote_bd_type parameter.
 * @param[in] remote_bd_type Peer's device address type. Value is @ref T_GAP_REMOTE_ADDR_TYPE.
 * @param[in] local_bd_type  Local device address type. Value is @ref T_GAP_LOCAL_ADDR_TYPE.
 * @param[in] scan_timeout  Initiating timeout value. Time = N * 10 ms.
 *                          - 0: Bluetooth Host will not cancel create connection.
 *                          - Others: If time has expired before connection has been established, Bluetooth Host will
 *                                    cancel create connection.
 * @return The result of sending request.
 * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
 * @retval Others Sending request operation is failed.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_LE_CONN_REQ_PARAM conn_req_param;
        T_GAP_REMOTE_ADDR_TYPE bd_type = GAP_REMOTE_ADDR_LE_PUBLIC;
        uint8_t bd_addr[BD_ADDR_SIZE] = {0x00, 0xe0, x04c, 0x23, 0x99, 0x87};
        conn_req_param.scan_interval = 0x10;
        conn_req_param.scan_window = 0x10;
        conn_req_param.conn_interval_min = 80;
        conn_req_param.conn_interval_max = 80;
        conn_req_param.conn_latency = 0;
        conn_req_param.supv_tout = 1000;
        conn_req_param.ce_len_min = 2 * (conn_req_param.conn_interval_min - 1);
        conn_req_param.ce_len_max = 2 * (conn_req_param.conn_interval_max - 1);
        le_set_conn_param(GAP_CONN_PARAM_1M, &conn_req_param);

        le_connect(0, bd_addr,
                  bd_type,
                  GAP_LOCAL_ADDR_LE_PUBLIC,
                  1000);
    }

    void app_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state, uint16_t disc_cause)
    {
        APP_PRINT_INFO4("app_handle_conn_state_evt: conn_id %d old_state %d new_state %d, disc_cause 0x%x",
                        conn_id, gap_conn_state, new_state, disc_cause);
        switch (new_state)
        {
        case GAP_CONN_STATE_DISCONNECTED:
            {
                if ((disc_cause != (HCI_ERR | HCI_ERR_REMOTE_USER_TERMINATE))
                    && (disc_cause != (HCI_ERR | HCI_ERR_LOCAL_HOST_TERMINATE)))
                {
                    APP_PRINT_ERROR1("app_handle_conn_state_evt: connection lost cause 0x%x", disc_cause);
                }

                le_adv_start();
            }
            break;

        case GAP_CONN_STATE_CONNECTED:
            {
                uint16_t conn_interval;
                uint16_t conn_latency;
                uint16_t conn_supervision_timeout;
                uint8_t  remote_bd[6];
                T_GAP_REMOTE_ADDR_TYPE remote_bd_type;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                le_get_conn_addr(conn_id, remote_bd, &remote_bd_type);
                APP_PRINT_INFO5("GAP_CONN_STATE_CONNECTED:remote_bd %s, remote_addr_type %d, conn_interval 0x%x, conn_latency 0x%x, conn_supervision_timeout 0x%x",
                                TRACE_BDADDR(remote_bd), remote_bd_type,
                                conn_interval, conn_latency, conn_supervision_timeout);
            }
            break;

        default:
            break;
        }
        gap_conn_state = new_state;
    }

    //When link is connected, Bluetooth Host will read remote used features, and the result will be notified by msg GAP_MSG_LE_REMOTE_FEATS_INFO
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
       T_APP_RESULT result = APP_RESULT_SUCCESS;
       T_LE_CB_DATA cb_data;
       memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
       APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
       switch (cb_type)
       {
       ...
       case GAP_MSG_LE_REMOTE_FEATS_INFO:
           APP_PRINT_INFO3("GAP_MSG_LE_REMOTE_FEATS_INFO: conn id %d, cause 0x%x, remote_feats %b",
                           cb_data.p_le_remote_feats_info->conn_id,
                           cb_data.p_le_remote_feats_info->cause,
                           TRACE_BINARY(8, cb_data.p_le_remote_feats_info->remote_feats));
           break;
       }
       ...
    }
 *
 * \endcode
 */
T_GAP_CAUSE le_connect(uint8_t init_phys, uint8_t *remote_bd,
                       T_GAP_REMOTE_ADDR_TYPE remote_bd_type,
                       T_GAP_LOCAL_ADDR_TYPE local_bd_type, uint16_t scan_timeout);

/**
  * @brief      Confirm @ref GAP_MSG_LE_CONN_UPDATE_IND for specified connection when the previous result of @ref GAP_MSG_LE_CONN_UPDATE_IND
  *             is @ref APP_RESULT_PENDING.
  *
  * @note The function should only be used by @ref GAP_LINK_ROLE_MASTER when the previous result of @ref GAP_MSG_LE_CONN_UPDATE_IND
  *                   is @ref APP_RESULT_PENDING.
  *
  * @param[in] conn_id  Connection ID.
  * @param[in] result   @ref APP_RESULT_SUCCESS.
  *                     @ref APP_RESULT_REJECT.
  * @return Operation result.
  * @retval GAP_CAUSE_SUCCESS    Operation success.
  * @retval Others   Operation failure.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause = le_conn_update_cfm(conn_id, result);
    }
  *
  * \endcode
  */
T_GAP_CAUSE le_conn_update_cfm(uint8_t conn_id, uint16_t result);

/**
  * @brief      Send connection parameter update request.
  *
  * If sending request operation is successful, the updating result will be returned in one of the following ways:
  * - In the default situation, or when @ref le_gap_msg_info_way (true) has been called, APP will be notified
  *   by message @ref GAP_MSG_LE_CONN_PARAM_UPDATE with status @ref GAP_CONN_PARAM_UPDATE_STATUS.
  * - When @ref le_gap_msg_info_way (false) has been called, APP will be notified with the callback registered
  *   by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_GAP_STATE_MSG.
  *
  * @param[in] conn_id  Connection ID for this link.
  * @param[in] conn_interval_min  Minimum value for the connection interval.
  *                               Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @param[in] conn_interval_max  Maximum value for the connection interval.
  *                               Value range: 0x0006 - 0x0C80 (7.5ms - 4000ms, 1.25ms/step).
  * @param[in] conn_latency  Maximum Peripheral latency for the connection. Value range: 0x0000 - 0x01F3.
               This shall be less than or equal to (((supervision_timeout * 10ms) / (conn_interval_max * 1.25ms * 2)) - 1).
  * @param[in] supervision_timeout  Supervision timeout for the LE Link.
  *                                 Value range: 0x000A - 0x0C80 (100ms - 32000ms, 10ms/step).
  * @param[in] ce_length_min  Information parameter about minimum length of connection event needed for this LE connection.
  *                           - Range: 0x0000 to 0xFFFF
  *                           - Time = N * 0.625 ms.
  * @param[in] ce_length_max  Information parameter about maximum length of connection event needed for this LE connection.
  *                           - Range: 0x0000 to 0xFFFF
  *                           - Time = N * 0.625 ms.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
    void test(void)
    {
        uint8_t conn_id = 0;
        T_GAP_CAUSE cause;
        uint16_t conn_interval_min = 0x10;
        uint16_t conn_interval_max = 0x10;
        uint16_t conn_latency = 50;
        uint16_t supervision_timeout = 1000;
        uint16_t ce_length_min = 2 * (conn_interval_min - 1);
        uint16_t ce_length_max = 2 * (conn_interval_max - 1);


        cause = le_update_conn_param(conn_id,
                                    conn_interval_min,
                                    conn_interval_max,
                                    conn_latency,
                                    supervision_timeout,
                                    ce_length_min,
                                    ce_length_max
                                   );

    }

    void app_handle_conn_param_update_evt(uint8_t conn_id, uint8_t status, uint16_t cause)
    {
        switch (status)
        {
        case GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS:
            {
                uint16_t conn_interval;
                uint16_t conn_slave_latency;
                uint16_t conn_supervision_timeout;

                le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_LATENCY, &conn_slave_latency, conn_id);
                le_get_conn_param(GAP_PARAM_CONN_TIMEOUT, &conn_supervision_timeout, conn_id);
                APP_PRINT_INFO3("app_handle_conn_param_update_evt update success:conn_interval 0x%x, conn_slave_latency 0x%x, conn_supervision_timeout 0x%x",
                                conn_interval, conn_slave_latency, conn_supervision_timeout);
            }
            break;

        case GAP_CONN_PARAM_UPDATE_STATUS_FAIL:
            {
                APP_PRINT_ERROR1("app_handle_conn_param_update_evt update failed: cause 0x%x", cause);
            }
            break;

        case GAP_CONN_PARAM_UPDATE_STATUS_PENDING:
            {
                APP_PRINT_INFO0("app_handle_conn_param_update_evt update pending.");
            }
            break;

        default:
            break;
        }
    }


    //If APP receive connection parameter update request from remote device, APP will receive GAP_MSG_LE_CONN_UPDATE_IND
    T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_APP_RESULT result = APP_RESULT_SUCCESS;
        T_LE_CB_DATA cb_data;
        memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
        APP_PRINT_TRACE1("app_gap_callback: cb_type %d", cb_type);
        switch (cb_type)
        {
        ...
        case GAP_MSG_LE_CONN_UPDATE_IND:
           APP_PRINT_INFO4("GAP_MSG_LE_CONN_UPDATE_IND: conn_interval_max 0x%x, conn_interval_min 0x%x, conn_latency 0x%x
        , supervision_timeout 0x%x",
                           cb_data.p_le_conn_update_ind->conn_interval_max,
                           cb_data.p_le_conn_update_ind->conn_interval_min,
                           cb_data.p_le_conn_update_ind->conn_latency,
                           cb_data.p_le_conn_update_ind->supervision_timeout);
           if (cb_data.p_le_conn_update_ind->conn_latency > 60)
           {
               result = APP_RESULT_REJECT;
           }
           else
           {
               result = APP_RESULT_ACCEPT;
           }
           break;
           ...
        }
    }
  * \endcode
  */
T_GAP_CAUSE le_update_conn_param(uint8_t   conn_id,
                                 uint16_t  conn_interval_min,
                                 uint16_t  conn_interval_max,
                                 uint16_t  conn_latency,
                                 uint16_t  supervision_timeout,
                                 uint16_t  ce_length_min,
                                 uint16_t  ce_length_max);

/**
  * @brief  Set the link tx power for the device.
  *
  * Set LE link tx power. Or reset link tx power to default value.
  * Tx power is set to default every time a link is connect. Use this API after the link is connected.
  *
  * @note @parblock Different types of IC support different tx power levels. For the actual supported levels,
  * please refer to the IC manual and related documentation.
  *
  * The resolution of tx power is 0.5 dBm, not 1 dBm. Therefore, when setting the tx_power_double
  * parameter, it should be set to twice the actual tx power.
  * @endparblock
  *
  * If sending request operation is successful, the setting result will be returned by callback
  * registered by @ref le_register_app_cb with msg type @ref GAP_MSG_LE_SET_CONN_TX_PWR.
  *
  * @param[in] conn_id  Connection ID for this link.
  * @param[in] reset    Whether to reset to default value.
  * @param[in] tx_power Index for power level.
  * @return The result of sending request.
  * @retval GAP_CAUSE_SUCCESS Sending request operation is successful.
  * @retval Others Sending request operation is failed.
  *
  * <b>Example usage</b>
  * \code{.c}
     void test(void)
     {
        T_GAP_CAUSE cause;
        uint8_t conn_id = 0;
        bool reset = false;
        uint8_t tx_pwr = 0x80;

        cause = le_set_conn_tx_power(conn_id, reset, tx_pwr);
     }

     T_APP_RESULT app_gap_callback(uint8_t cb_type, void *p_cb_data)
     {
         T_APP_RESULT result = APP_RESULT_SUCCESS;
         T_LE_CB_DATA cb_data;
         memcpy(&cb_data, p_cb_data, sizeof(T_LE_CB_DATA));
         APP_PRINT_TRACE1("app_gap_callback: msgType %d", cb_type);
         switch (cb_type)
         {
         ...
            case GAP_MSG_LE_SET_CONN_TX_PWR:
                APP_PRINT_INFO1("GAP_MSG_LE_SET_CONN_TX_PWR: cause 0x%x",
                                p_data->le_cause.cause);
                break;
         ...
         }
     }
  * \endcode
  */
T_GAP_CAUSE le_set_conn_tx_power(uint8_t conn_id, bool reset, uint8_t tx_power);

/** @} */ /* End of group GAP_LE_CONNECTION_COMMON_EXPORT_Functions */
/** @} */ /* End of group GAP_LE_CONNECTION_MODULE */


/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* GAP_CONN_LE_H */


